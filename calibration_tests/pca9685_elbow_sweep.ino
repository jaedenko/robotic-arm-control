// Sweeps the elbow servo (channel 3, MG995) back and forth between
// two angles you can easily tweak below. Use this to find the real
// safe range of motion for the elbow joint.

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(); // default I2C addr 0x40

#define SERVO_MIN  102
#define SERVO_MAX  512

const int CH_ELBOW = 3;

// ---- Change these to test different ranges ----
int ANGLE_A = 60;
int ANGLE_B = 120;

int angleToTicks(int angle) {
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

void setup() {
  pca.begin();
  pca.setPWMFreq(50);
}

void loop() {
  sweep(CH_ELBOW, ANGLE_A, ANGLE_B);
  delay(300);
  sweep(CH_ELBOW, ANGLE_B, ANGLE_A);
  delay(300);
}

void sweep(int channel, int fromAngle, int toAngle) {
  int step = (toAngle > fromAngle) ? 1 : -1;
  for (int a = fromAngle; a != toAngle; a += step) {
    pca.setPWM(channel, 0, angleToTicks(a));
    delay(15);
  }
}
