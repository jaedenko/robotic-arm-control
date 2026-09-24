// Cycles the claw (channel 4) open and closed repeatedly so you can
// watch it and dial in the right angles + direction.
// Open Serial Monitor at 9600 baud to see which state it's in.

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(); // default I2C addr 0x40

#define SERVO_MIN  102
#define SERVO_MAX  512

const int CH_GRIPPER = 4;

// PLACEHOLDER angles -- tune these. Assumption: increasing angle =
// clockwise (viewed from top) = closing. If yours does the opposite,
// just swap the two values below, or flip GRIPPER_INVERTED to true.
int OPEN_ANGLE  = 90;
int CLOSE_ANGLE = 30;
bool GRIPPER_INVERTED = false;

int angleToTicks(int angle) {
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

void setup() {
  Serial.begin(9600);
  pca.begin();
  pca.setPWMFreq(50);
}

void loop() {
  Serial.println("OPEN");
  writeGripper(OPEN_ANGLE);
  delay(1500);

  Serial.println("CLOSE");
  writeGripper(CLOSE_ANGLE);
  delay(1500);
}

void writeGripper(int angle) {
  if (GRIPPER_INVERTED) angle = 180 - angle;
  pca.setPWM(CH_GRIPPER, 0, angleToTicks(angle));
}
