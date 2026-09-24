// Sweeps the mirrored shoulder pair together: channel 1 goes 0->180
// while channel 2 goes 180->0 at the same time, so they move as one
// joint instead of fighting each other.

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(); // default I2C addr 0x40

#define SERVO_MIN  102
#define SERVO_MAX  512

const int CH_SHOULDER_L = 1;
const int CH_SHOULDER_R = 2;

int angleToTicks(int angle) {
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

void setup() {
  pca.begin();
  pca.setPWMFreq(50);
}

void loop() {
  sweepMirrored(0, 180);
  delay(300);
  sweepMirrored(180, 0);
  delay(300);
}

// Moves CH_SHOULDER_L from fromAngle->toAngle while CH_SHOULDER_R
// moves the mirrored (180 - angle) at the same time.
void sweepMirrored(int fromAngle, int toAngle) {
  int step = (toAngle > fromAngle) ? 1 : -1;
  for (int a = fromAngle; a != toAngle; a += step) {
    pca.setPWM(CH_SHOULDER_L, 0, angleToTicks(a));
    pca.setPWM(CH_SHOULDER_R, 0, angleToTicks(180 - a));
    delay(15);
  }
}
