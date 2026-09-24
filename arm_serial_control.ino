// Listens for serial commands from the control page and drives the
// arm accordingly. Enforces the calibrated limits from the servo
// limits table so the web UI can't send anything unsafe.
//
// Command format (one per line, newline-terminated):
//   T<angle>   e.g. T90   -> turret
//   S<angle>   e.g. S120  -> shoulder pair (mirrored)
//   E<angle>   e.g. E100  -> elbow
//   G1 / G0    -> claw open / close

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(); // default I2C addr 0x40

#define SERVO_MIN 102
#define SERVO_MAX 512

const int CH_TURRET     = 0;
const int CH_SHOULDER_L = 1;
const int CH_SHOULDER_R = 2;
const int CH_ELBOW      = 3;
const int CH_GRIPPER    = 4;

// ---- Calibrated limits (from the servo limits table) ----
const int TURRET_MIN   = 0,  TURRET_MAX   = 180;
const int SHOULDER_MIN = 0,  SHOULDER_MAX = 180;
const int ELBOW_MIN    = 30, ELBOW_MAX    = 175;
const int CLAW_OPEN    = 90, CLAW_CLOSE   = 35;

String inputBuffer = "";

int angleToTicks(int angle) {
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

void writeTurret(int angle) {
  angle = constrain(angle, TURRET_MIN, TURRET_MAX);
  pca.setPWM(CH_TURRET, 0, angleToTicks(angle));
}

void writeShoulder(int angle) {
  angle = constrain(angle, SHOULDER_MIN, SHOULDER_MAX);
  pca.setPWM(CH_SHOULDER_L, 0, angleToTicks(angle));
  pca.setPWM(CH_SHOULDER_R, 0, angleToTicks(180 - angle)); // mirrored
}

void writeElbow(int angle) {
  angle = constrain(angle, ELBOW_MIN, ELBOW_MAX);
  pca.setPWM(CH_ELBOW, 0, angleToTicks(angle));
}

void writeClaw(bool open) {
  int angle = open ? CLAW_OPEN : CLAW_CLOSE;
  pca.setPWM(CH_GRIPPER, 0, angleToTicks(angle));
}

void setup() {
  Serial.begin(9600);
  pca.begin();
  pca.setPWMFreq(50);

  // safe starting pose
  writeTurret(90);
  writeShoulder(90);
  writeElbow(90);
  writeClaw(true);
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      processCommand(inputBuffer);
      inputBuffer = "";
    } else if (c != '\r') {
      inputBuffer += c;
    }
  }
}

void processCommand(String cmd) {
  cmd.trim();
  if (cmd.length() < 2) return;

  char type = cmd.charAt(0);
  String valueStr = cmd.substring(1);

  if (type == 'T') {
    writeTurret(valueStr.toInt());
  } else if (type == 'S') {
    writeShoulder(valueStr.toInt());
  } else if (type == 'E') {
    writeElbow(valueStr.toInt());
  } else if (type == 'G') {
    writeClaw(valueStr.toInt() == 1);
  } else if (type == 'M') {
    // combined move: M<turret>,<shoulder>,<elbow> -- applies all three in one
    // command so simultaneous jog inputs always land together atomically
    // instead of racing as separate lines.
    int firstComma = valueStr.indexOf(',');
    int secondComma = valueStr.indexOf(',', firstComma + 1);
    if (firstComma > 0 && secondComma > firstComma) {
      int t = valueStr.substring(0, firstComma).toInt();
      int s = valueStr.substring(firstComma + 1, secondComma).toInt();
      int e = valueStr.substring(secondComma + 1).toInt();
      writeTurret(t);
      writeShoulder(s);
      writeElbow(e);
    }
  }
}
