# Changelog

## November 2026: body tracking, simulator, new look, hosted site

The control page can now follow your arm with a webcam, can be tried without any
hardware, has a new layout, and is hosted online at
**https://robotic-arm-control.vercel.app**. The Arduino firmware
(`arm_serial_control.ino`) did not change.

### Added
- **Body tracking.** "Follow my arm" uses your webcam (MediaPipe Pose, runs in the
  browser; video never leaves it). Upper-arm elevation drives the shoulder, elbow
  bend drives the elbow, wrist sideways offset drives the turret. Movement speed
  is capped by the Speed slider, and STOP or Esc ends tracking. Choose which arm to
  follow and flip any joint that moves the wrong way; the choices are remembered.
- **Hand-controlled claw.** An open hand opens the claw and a fist closes it
  (MediaPipe Hand Landmarker). It needs a few steady frames, so a flicker won't
  toggle it, ignores hands away from the tracked wrist, and can be switched off.
- **Simulator.** The **Simulator** button runs the whole page against a virtual
  arm with a claw, no Arduino or serial port needed. It follows the same commands
  and joint limits as the firmware, moves at servo-like speed, and shows a serial
  monitor of every line the Arduino would receive. Works in any browser.
- **Live arm view** with a side-view drawing, claw jaws, turret dial and angle
  readouts. It stays pinned while you scroll.
- **Hosted version** on Vercel, served over HTTPS so Web Serial and the webcam work
  without a local server. Pushes to `main` redeploy automatically.
- `tracking_check.js`: a runnable check for the arm-angle math, the open/closed
  hand test and the simulator's command parser (`node tracking_check.js`).

### Changed
- **Redesigned control page.** Sticky header with an always-visible STOP and
  Connect button, a first-run guide, a two-column layout, controls that stay
  dimmed until connected, and a layout that works on phones and tablets.
- Presets are now called **saved poses**, and messages say what to do next.
- Esc is a second STOP, and unsupported browsers get a clear message.

### Not tested on hardware yet
The tracking mappings (especially elbow direction, and the overlap between turret
and shoulder in a 2D camera view) and the hand detection have been checked with
scripted landmarks and the simulator, but not with a real webcam and arm. Try
tracking in the simulator first, then on the arm at a low Speed with a hand near
STOP.

### Pull requests
- #1 Add webcam arm tracking, redesign the control page, and host it on Vercel
- #2 Add a simulator and hand-controlled claw
- #3 Show the camera in the tracking card and keep the arm view pinned
