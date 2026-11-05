// Sanity checks for the pure logic in arm_control.html: the arm-angle math, the open/closed hand
// test, and the simulator's command parser (which must match the firmware).
// Run: node tracking_check.js
const fs = require('fs');
const assert = require('assert');

const html = fs.readFileSync(__dirname + '/arm_control.html', 'utf8');
const block = (name) => html.match(new RegExp(`// <${name}>([\\s\\S]*?)// </${name}>`))[1];
const LIMITS = { T: [0, 180], S: [0, 180], E: [30, 175] }; // same as the page
const { armToJoints, extendedFingers, pickHand, applyCommand } = new Function('LIMITS',
  block('armToJoints') + block('handOpenness') + block('simCommand') +
  '; return { armToJoints, extendedFingers, pickHand, applyCommand };')(LIMITS);

const near = (a, b, msg) => assert(Math.abs(a - b) < 0.5, `${msg}: got ${a}, want ${b}`);
const P = (x, y) => ({ x, y });

// ---- arm angles ----
// arm hanging straight down: shoulder 0, elbow straight, turret centered
let r = armToJoints(P(.5, .3), P(.5, .5), P(.5, .7), 1);
near(r.S, 0, 'down S'); near(r.E, 180, 'down E'); near(r.T, 90, 'down T');

// arm straight up
r = armToJoints(P(.5, .5), P(.5, .3), P(.5, .1), 1);
near(r.S, 180, 'up S'); near(r.E, 180, 'up E');

// right arm held out sideways (image-left): horizontal, straight, turret pushed to one end
r = armToJoints(P(.5, .3), P(.3, .3), P(.1, .3), 1);
near(r.S, 90, 'side S'); near(r.E, 180, 'side E'); near(r.T, 180, 'side T (right of mirror view)');

// left arm held out sideways (image-right): mirror image -> other end
r = armToJoints(P(.5, .3), P(.7, .3), P(.9, .3), 1);
near(r.T, 0, 'left-arm side T');

// forearm bent 90 degrees at the elbow
r = armToJoints(P(.5, .3), P(.5, .5), P(.7, .5), 1);
near(r.E, 90, 'bent E');

// non-square frame must not skew angles: same physical pose at 2:1 aspect
r = armToJoints(P(.5, .3), P(.5, .5), P(.6, .5), 2);
near(r.E, 90, 'aspect E');

// ---- open vs closed hand ----
// 21 landmarks: wrist at (.5,.8). Each finger runs [knuckle, middle joint, tip] straight up when
// open; when curled the tip folds back toward the palm, closer to the wrist than the middle joint.
function hand(open, wristX = 0.5) {
  const h = Array.from({ length: 21 }, () => P(wristX, 0.8));
  [[5, 6, 8], [9, 10, 12], [13, 14, 16], [17, 18, 20]].forEach(([k, m, t], i) => {
    const x = wristX - 0.06 + i * 0.04;
    h[k] = P(x, 0.62); h[m] = P(x, 0.52);
    h[t] = open ? P(x, 0.42) : P(x, 0.66);
  });
  return h;
}
assert.strictEqual(extendedFingers(hand(true), 1), 4, 'open hand');
assert.strictEqual(extendedFingers(hand(false), 1), 0, 'fist');
assert.strictEqual(extendedFingers(hand(true), 1.78), 4, 'open hand, wide frame');
const half = hand(true); half[12] = P(.5, .66); half[16] = P(.5, .66); // two fingers curled
assert.strictEqual(extendedFingers(half, 1), 2, 'two fingers');

// the hand chosen is the one at the tracked wrist, not just any hand
const near1 = hand(true, 0.3), far1 = hand(false, 0.8);
assert.strictEqual(pickHand([far1, near1], P(0.31, 0.8), 1), near1, 'pick nearest hand');
assert.strictEqual(pickHand([far1], P(0.3, 0.8), 1), null, 'no hand at the wrist');
assert.strictEqual(pickHand([], P(0.3, 0.8), 1), null, 'no hands at all');

// ---- simulator parser = firmware rules ----
const t = { T: 90, S: 90, E: 90, g: 90 };
applyCommand('T200', t); assert.strictEqual(t.T, 180, 'turret clamps to 180');
applyCommand('E10', t);  assert.strictEqual(t.E, 30, 'elbow clamps to 30');
applyCommand('E999', t); assert.strictEqual(t.E, 175, 'elbow clamps to 175');
applyCommand('S-20', t); assert.strictEqual(t.S, 0, 'shoulder clamps to 0');
applyCommand('G0', t);   assert.strictEqual(t.g, 35, 'claw close');
applyCommand('G1', t);   assert.strictEqual(t.g, 90, 'claw open');
applyCommand('M10,20,300\r', t); assert.deepStrictEqual([t.T, t.S, t.E], [10, 20, 175], 'combined move');
applyCommand('M50,60', t); assert.deepStrictEqual([t.T, t.S, t.E], [10, 20, 175], 'incomplete M ignored');
applyCommand('Tabc', t); assert.strictEqual(t.T, 0, 'junk number reads as 0, like toInt()');
applyCommand('X5', t);   applyCommand('', t); applyCommand('T', t); // unknown / too short: ignored, no throw

console.log('tracking_check: all good');
