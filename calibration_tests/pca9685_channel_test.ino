// Sweeps one PCA9685 channel at a time through a gentle range so you
// can visually match each channel number to a physical servo.
// Open Serial Monitor (9600 baud) to see which channel is active.

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(); // default I2C addr 0x40

#define SERVO_MIN  102   // same defaults as the main sketch
#define SERVO_MAX  512

// Only test channels you've actually got servos plugged into.
// Adjust this range to match how many you've wired up.
const int FIRST_CHANNEL = 0;
const int LAST_CHANNEL  = 4;

// Narrow, safe sweep range -- won't hit mechanical limits before
// you've calibrated each joint's real range.
const int SWEEP_MIN = 70;
const int SWEEP_MAX = 110;

int angleToTicks(int angle) {
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

void setup() {
  Serial.begin(9600);
  pca.begin();
  pca.setPWMFreq(50);

  // center every channel we're testing so nothing is left mid-sweep
  for (int ch = FIRST_CHANNEL; ch <= LAST_CHANNEL; ch++) {
    pca.setPWM(ch, 0, angleToTicks(90));
  }
  delay(1000);
}

void loop() {
  for (int ch = FIRST_CHANNEL; ch <= LAST_CHANNEL; ch++) {
    Serial.print("Testing channel ");
    Serial.println(ch);

    sweep(ch, SWEEP_MIN, SWEEP_MAX);
    sweep(ch, SWEEP_MAX, SWEEP_MIN);

    // return this channel to center before moving to the next
    pca.setPWM(ch, 0, angleToTicks(90));
    delay(500);
  }

  Serial.println("--- full pass done, repeating ---");
  delay(1000);
}

void sweep(int channel, int fromAngle, int toAngle) {
  int step = (toAngle > fromAngle) ? 1 : -1;
  for (int a = fromAngle; a != toAngle; a += step) {
    pca.setPWM(channel, 0, angleToTicks(a));
    delay(15);
  }
}
