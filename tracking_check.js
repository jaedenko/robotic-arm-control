// Sanity check for the arm-tracking angle math in arm_control.html.
// Run: node tracking_check.js
const fs = require('fs');
const assert = require('assert');

const html = fs.readFileSync(__dirname + '/arm_control.html', 'utf8');
const src = html.match(/\/\/ <armToJoints>([\s\S]*?)\/\/ <\/armToJoints>/)[1];
const armToJoints = new Function(src + '; return armToJoints;')();

const near = (a, b, msg) => assert(Math.abs(a - b) < 0.5, `${msg}: got ${a}, want ${b}`);
const P = (x, y) => ({ x, y });

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

console.log('tracking_check: all good');
