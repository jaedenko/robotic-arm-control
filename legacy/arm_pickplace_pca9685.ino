// Requires the Adafruit PWM Servo Driver library.
// Arduino IDE: Sketch > Include Library > Manage Libraries > search
// "Adafruit PWM Servo Driver" > Install

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(); // default I2C addr 0x40

// ==================== CHANNEL ASSIGNMENTS ====================
// PCA9685 has 16 channels (0-15). Assign each servo to one.
const int CH_TURRET     = 0;
const int CH_SHOULDER_L = 1;   // shoulderR mirrors this one
const int CH_SHOULDER_R = 2;
const int CH_ELBOW      = 3;
const int CH_GRIPPER    = 4;

// If shoulderR is mounted facing the opposite way from shoulderL,
// it needs to move to (180 - angle) to actually mirror the motion.
bool SHOULDER_R_INVERTED = true;

// ==================== SERVO PULSE CALIBRATION ====================
// PCA9685 doesn't take angles directly -- it takes a "tick" value
// (0-4095) representing pulse width at 50Hz. These are typical
// starting points for standard hobby servos; if your servos don't
// hit their full physical range, nudge these in small steps.
#define SERVO_MIN  102   // tick count at 0 degrees  (~0.5ms pulse)
#define SERVO_MAX  512   // tick count at 180 degrees (~2.5ms pulse)

int angleToTicks(int angle) {
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

// ==================== POSES ====================
// angles are PLACEHOLDERS. Test each joint individually first (see
// calibration notes below) and replace these with real safe values.
struct Pose {
  int turret;
  int shoulder;  // single value -- both shoulder servos derive from this
  int elbow;
  int gripper;   // e.g. ~40 = open, ~90 = closed -- tune to your gripper
};

Pose HOME       = {90, 90, 90, 40};
Pose OVER_OBJ   = {60, 60, 100, 40};
Pose GRAB_LOW   = {60, 40, 110, 40};
Pose GRAB_CLOSE = {60, 40, 110, 90};
Pose LIFT       = {60, 90, 90,  90};
Pose OVER_DROP  = {130, 60, 100, 90};
Pose DROP_LOW   = {130, 40, 110, 90};
Pose RELEASE    = {130, 40, 110, 40};

void setup() {
  pca.begin();
  pca.setPWMFreq(50); // standard hobby servos run at 50Hz

  writePoseInstant(HOME);
  delay(1000);
}

void loop() {
  moveToPose(OVER_OBJ,   800);
  delay(300);
  moveToPose(GRAB_LOW,   600);
  delay(300);
  moveToPose(GRAB_CLOSE, 400);
  delay(400);
  moveToPose(LIFT,       600);
  delay(300);
  moveToPose(OVER_DROP,  900);
  delay(300);
  moveToPose(DROP_LOW,   600);
  delay(300);
  moveToPose(RELEASE,    400);
  delay(400);
  moveToPose(HOME,       800);
  delay(1500);

  while (true); // remove this line once you want it to repeat continuously
}

// ==================== MOTION HELPERS ====================

Pose currentPose = {90, 90, 90, 40}; // tracks last commanded pose

// Smoothly steps every servo from currentPose to target over durationMs
void moveToPose(Pose target, int durationMs) {
  const int steps = 30;
  int stepDelay = durationMs / steps;

  for (int i = 1; i <= steps; i++) {
    float t = (float)i / steps;
    int tTurret   = currentPose.turret   + (target.turret   - currentPose.turret)   * t;
    int tShoulder = currentPose.shoulder + (target.shoulder - currentPose.shoulder) * t;
    int tElbow    = currentPose.elbow    + (target.elbow    - currentPose.elbow)    * t;
    int tGripper  = currentPose.gripper  + (target.gripper  - currentPose.gripper)  * t;

    writeServos(tTurret, tShoulder, tElbow, tGripper);
    delay(stepDelay);
  }
  currentPose = target;
}

// Jumps straight to a pose with no interpolation (used once at boot)
void writePoseInstant(Pose p) {
  writeServos(p.turret, p.shoulder, p.elbow, p.gripper);
  currentPose = p;
}

void writeServos(int t, int s, int e, int g) {
  pca.setPWM(CH_TURRET,     0, angleToTicks(t));
  pca.setPWM(CH_SHOULDER_L, 0, angleToTicks(s));
  pca.setPWM(CH_SHOULDER_R, 0, angleToTicks(SHOULDER_R_INVERTED ? 180 - s : s));
  pca.setPWM(CH_ELBOW,      0, angleToTicks(e));
  pca.setPWM(CH_GRIPPER,    0, angleToTicks(g));
}
