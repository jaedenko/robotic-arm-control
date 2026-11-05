# Robotic Arm Control

A 5-servo robotic arm (turret, mirrored shoulder pair, elbow, gripper) built on
an Arduino (Uno or Mega) with a PCA9685 servo driver board, controlled from a
browser over USB serial (Web Serial API).

## Hardware

- Arduino Uno or Mega
- PCA9685 16-channel PWM/servo driver (I2C)
- 5x hobby servos: turret, shoulder (x2, mirrored), elbow, gripper (MG995 or similar)
- Separate 5-6V power supply for the servos (do not power them from the Arduino)

### Wiring

- PCA9685 **VCC** → Arduino 5V (logic only)
- PCA9685 **GND** → Arduino GND
- PCA9685 **SDA/SCL** → Arduino I2C pins (A4/A5 on Uno, pins 20/21 on Mega)
- PCA9685 **V+** → separate servo power supply (this is what actually powers the servos)

### Channel mapping (as configured)

| Servo         | PCA9685 Channel | Notes                          |
|---------------|:---------------:|---------------------------------|
| Turret        | 0                | 0-180°                          |
| Shoulder L    | 1                | mirrors Shoulder R              |
| Shoulder R    | 2                | inverted (180 - angle)          |
| Elbow         | 3                | 30-175° safe range              |
| Claw/Gripper  | 4                | open ≈ 90°, closed ≈ 35°        |

## Software

### `arm_serial_control.ino`
Upload this to the Arduino. Requires the **Adafruit PWM Servo Driver** library
(Arduino IDE: Sketch → Include Library → Manage Libraries → search "Adafruit
PWM Servo Driver"). Listens on Serial (9600 baud) for simple text commands:

- `T<angle>` — turret
- `S<angle>` — shoulder pair (mirrored automatically)
- `E<angle>` — elbow
- `G1` / `G0` — claw open / close
- `M<turret>,<shoulder>,<elbow>` — combined atomic move for all three, used by
  the jog controls so simultaneous multi-axis movement doesn't race as
  separate serial lines

Enforces calibrated joint limits so nothing sent from the control page can
exceed the arm's safe range.

### `arm_control.html`
The browser control page. Open it in **Chrome or Edge** (Web Serial isn't
supported in Firefox/Safari). Features:

- Sliders for turret, shoulder, and elbow, with nudge (±1/±5) and reset buttons
- A vertical joystick (shoulder) plus keyboard jog: **A/D** rotates the
  turret, **W/S** moves the elbow — all three work simultaneously
- Adjustable jog speed with a reset button
- A live side-view diagram of the arm and a turret compass
- User-defined presets (pose + claw state) with optional single-key hotkeys
- Sequences: chain presets together with per-step move duration and repeat/loop,
  editable after saving
- Export/import of presets & sequences as a JSON backup file
- **Body tracking**: the "Track my arm" button uses your webcam (MediaPipe Pose,
  runs in the browser) to drive the arm. Upper-arm elevation → shoulder, elbow
  bend → elbow, wrist sideways offset → turret. Speed is capped by the jog-speed
  slider. Choose which arm to follow and flip any joint that moves the wrong way
  via `TRACK_ARM` / `TRACK_INVERT` at the top of the tracking block. The claw
  stays on the space bar. `node tracking_check.js` sanity-checks the angle math.
- A STOP button that instantly halts any running animation, sequence, jog, or tracking

If double-clicking the HTML file doesn't allow it to connect (some Chrome
versions restrict Web Serial on `file://` pages), serve it locally instead:

```
python3 -m http.server
```

then visit `http://localhost:8000/arm_control.html`.

### `calibration_tests/`
Standalone sketches used to find safe servo ranges and verify wiring before
running the main sketch:

- `pca9685_channel_test.ino` — sweeps one channel at a time so you can match
  channel numbers to physical servos
- `pca9685_all_neutral.ino` — sets every servo to a safe 90° neutral position
- `pca9685_claw_test.ino` — cycles the claw open/closed to find real angles
- `pca9685_elbow_sweep.ino` — sweeps the elbow between two adjustable angles
- `pca9685_shoulder_mirror_test.ino` — verifies the mirrored shoulder pair
  moves symmetrically rather than fighting itself

### `legacy/`
- `arm_pickplace_pca9685.ino` — an earlier hardcoded pick-and-place sequence,
  superseded by the serial-controlled sketch + web UI's sequence builder.
  Kept for reference.

## Calibrated servo limits

| Servo    | Safe Range         |
|----------|---------------------|
| Claw     | Open 90° / Close 35°|
| Elbow    | 30° - 175°           |
| Shoulder | 0° - 180° (mirrored) |
| Turret   | 0° - 180°            |
