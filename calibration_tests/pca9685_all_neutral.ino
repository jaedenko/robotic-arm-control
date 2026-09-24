// Sets all servos to a safe neutral position (90 degrees) and holds
// them there. Useful as a known starting point before wiring checks
// or calibration.

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(); // default I2C addr 0x40

#define SERVO_MIN  102
#define SERVO_MAX  512

// Adjust to match how many channels you actually have servos on.
const int FIRST_CHANNEL = 0;
const int LAST_CHANNEL  = 4;

const int NEUTRAL_ANGLE = 90;

int angleToTicks(int angle) {
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

void setup() {
  pca.begin();
  pca.setPWMFreq(50);

  for (int ch = FIRST_CHANNEL; ch <= LAST_CHANNEL; ch++) {
    pca.setPWM(ch, 0, angleToTicks(NEUTRAL_ANGLE));
  }
}

void loop() {
  // nothing to do -- servos just hold neutral position
}
