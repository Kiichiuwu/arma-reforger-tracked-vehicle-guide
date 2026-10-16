> [🇧🇷 Português](../pt-BR/08-internals.md) · 🇬🇧 English

[← Attribute reference](07-attribute-reference.md) · [Contents](../../README.md) · [Bugs and limitations (1.8.0.13) →](09-bugs.md)

# How it works inside

This chapter summarizes the reverse engineering of the 1.8.0.13 executable and checks it against measurements on the Leopard 2A6: telemetry from the log and the native vehicle debug overlay in the test videos. Each part says how it is known. Knowing the signal path helps you understand why each value matters.

### Driver inputs

The tracked vehicle has its own set of input actions, separate from the car:

| Action | Function |
| --- | --- |
| `TrackedThrottle` | Accelerate |
| `TrackedBrake` | Brake; when stopped, it becomes reverse |
| `TrackedSteeringLeft` / `TrackedSteeringRight` | Turn |
| `TurboToggle` / `TurboHold` | Turbo (100% throttle) |
| `HandBrake` / `HandBrakePersistent` | Handbrake |
| `EngineStart` / `EngineStop` | Start and stop the engine |

There is **no** action to shift gears, select reverse or use the clutch. The car has `CarShift` and `CarShiftReverse`; the tracked vehicle does not.

Inputs only go through if the driver is alive, is not using an item, and is not getting in, getting out or switching seats.

### The controller (SCR\_TrackedControllerComponent)

The controller turns the inputs into throttle, brake, gear and clutch.

**Throttle.** W delivers `1 − ThrottleTurbo`. With the default of 0.2, that gives 80%. The turbo unlocks the remaining 20%.

**Direction selection (automatic):**

- From **neutral** to **reverse**: brake above 1/32 and speed below 1.5 m/s. The switch is immediate.
- From **forward** to **reverse**: brake above 1/32, throttle below 1/32 and speed below 1.5 m/s for 0.5 s.
- In reverse, the pedals are swapped every frame. `TrackedBrake` becomes the throttle, with the value `ThrottleReverseTarget`.

**Gears.** Index 0 is reverse, index 1 is neutral, and the forward gears start at index 2. The first value of `Forward { }` is index 2. The debug panel shows them as R, N and 1, 2, 3… (confirmed against the telemetry on the 2A6).

**Clutch.** It is a multiplier from 0 to 1, driven by time (`ClutchCoupleTime`, `ClutchUncoupleTime`). `ClutchCouple*Rpm` only applies when pulling away. `MaxClutchTorque` is not used.

**Shifts.** Automatic shifts use the filtered RPM (`RpmSmoothing`), the slope (`SlopeSmoothing`) and the upshift and downshift factors.

What the controller did on the 2A6 (telemetry and debug panel, 2026-10-05 and 10-06; the last row is from a test on 2026-10-04):

| Situation | Observed |
| --- | --- |
| Standing still after a stop | Gear R (index 0), brake 6.0, clutch 0, engine at `RpmIdle` 700 rpm |
| Pulling away | Brake 6.0 stays on while the clutch engages, and drops to 0 when the clutch reaches about 0.8 |
| Upshifts at throttle 0.8 | The throttle is lifted and the clutch slips for about 0.4–0.6 s. Medians: 1→2 at 2924 rpm (12.9 km/h), 2→3 at 2812 rpm (23.2 km/h), 3→4 at 2860 rpm (39.1 km/h) |
| End of 1st gear | It holds 1st on the governor at 13.6 km/h (about 2935 rpm) until the upshift |
| Downshifts | Medians 1318 rpm (8.4 km/h), 1758 rpm (21.4 km/h) and 1626 rpm (34.2 km/h) |
| Pivot turn (full steering, nearly stopped) | Either clutch 0 with brake 6.0 and the engine on the governor at 2700 rpm (3409 N·m), or clutch 1 with the engine dragged down to 700 rpm (4601 N·m). The track running backward was usually faster, and the hull crept backward at 0.1–3.8 km/h (cause open) |
| Driver leaves the seat | The controller pulls the handbrake and, at a standstill, selects reverse. The reverse bug then pins the engine far above `RpmMax`. [`LEO_TrackedReverseComponent`](../../scripts/LEO_TrackedReverseComponent.c) stops the engine when the seat is empty (`m_bStopEngineOnExit`) |

### The simulation (VehicleTrackedSimulation)

On each substep, the simulation computes engine → clutch → gearbox → differential → tracks → wheels. Then it applies the forces to the `RigidBody`. The link chain has its own fixed step (see [below](#the-link-chain-rope-step)).

**Time step:**

```math
\Delta t = \frac{1}{60 \cdot \text{SolverSubsteps}} \quad (\text{4 substeps} \Rightarrow 1/240\ \text{s})
```

**Gearbox table.** Internally, the list becomes `[−|Reverse|, 0, Forward...]`. The reverse ratio is **always negative**, even if you write a negative value.

**Track speed.** It uses the **drive sprocket** radius (`Sprocket.Radius`), not the road wheel radius:

```math
v\,[\text{km/h}] = \frac{\text{rpm}}{g \cdot R_d} \cdot \frac{2\pi}{60} \cdot r_{\text{sprocket}} \cdot 3.6
```

Here, *g* is the gear ratio and *R\_d* is the `Ratio` of the `TripleDifferential`. This is the speed of the track (the link chain). The hull's ground speed differs from it by a few percent of slip; see [Speeds cross-check](#speeds-cross-check).

**Load reaction on the engine.** The wheel load goes back to the engine through the same chain of ratios:

```math
\text{reaction} = \frac{\sum \text{load}_{\text{wheels}}}{g \cdot R_d}
```

```math
\text{rpm} \mathrel{-}= \frac{\text{clutch} \cdot \text{reaction}}{\text{Inertia} + \text{clutch} \cdot \text{InertiaCoupled}} \cdot \Delta t \cdot 9.5493
```

The load is always positive; when stopped, it equals the vehicle's weight. So the sign of *g* decides whether the load brakes or accelerates the engine. In reverse, *g* is negative, and the load **accelerates** the engine. The [next chapter](09-bugs.md) explains the effect. On the 2A6 (62300 kg, `Reverse` 2.3, `Ratio` 4.6, `Inertia` 3) the formula predicts the engine stuck at 3766.1 rpm in reverse; the telemetry and the debug panel showed 3766–3768 rpm.

**Engine torque.** The curve runs from `RpmIdle` to `RpmMax`, with its peak at `RpmMaxTorque`. At `RpmMax`, the torque is zero. Observed on the 2A6 (debug panel):

- With `HasGovernor 1`, the torque collapses above `RpmRedline`: 2462 N·m at 2719 rpm, 229 N·m at 2922 rpm, 86 N·m at 2935 rpm.
- With the throttle at 0, the engine brakes with a torque that fit −`Friction` · (rpm − `RpmIdle`) / (`RpmMax` − `RpmIdle`) within 2 N·m at ten readings, and stays at −471 N·m above `RpmMax`.
- The Engine panel's *Feedback* is the drivetrain-side rpm: sprocket rpm × gear ratio × `Ratio`. With the clutch locked, the engine rpm equals it. The script API has `EngineGetRPMFeedback()`, "RPM that is feeded back from rest of the powertrain"; in the telemetry it equalled the engine rpm with the clutch locked.

**Steering.** On the move, the differential interpolates up to `SteeringRatio` according to the steering input. When stopped, it uses `NeutralSteeringRatio`. `PivotSteering` locks the inner track; `NeutralSteering` makes it spin backward. Observed on the 2A6: at full steering in 1st gear the inner track turned at 1/6 of the outer one (99.08 against 16.51 rpm, `SteeringRatio` 6), and the hull turned at about 44°/s around a point about 1.6 m from its centre. In a pivot turn, *Feedback* was the mean of the two tracks' absolute rpm × `NeutralSteeringRatio` 12.

**Friction.** The contact friction uses the `FrictionCurve*` curves and the `V2Friction*` values.

### Wheels and suspension

Each `TrackPart` is one wheel of the simulation, with the template of its `Type` ([attributes](07-attribute-reference.md#wheels-and-suspension)). Only road wheels have a suspension. The values below were measured on the 2A6 with the `LEOSUSP` and `LEOTEL` telemetry ([Vehicle debug diag and telemetry](16-debugging.md)), unless marked otherwise.

![The vehicle debug overlay on the stopped Leopard 2A6: white circles at each wheel's sim radius and the native rope line along the bottom of the links](../images/debug-overlay-stationary.jpg)

*Test video A, 1:49, tank stopped, vehicle debug on. The white circles are drawn at each part's sim `Radius`: they reach the ground face of the links, below the rubber rims. The red line with cyan ticks is the native rope, lying along the bottom of the scripted links. Model "Leopard 2 A6" by buh-late, CC BY 4.0, modified.*

**One belt speed per track.** All the parts of a track share one belt speed. A road wheel's `WheelGetRPM` is that speed divided by its `Radius`: road wheel rpm = sprocket rpm × 0.34 / 0.418 (ratio 0.9999 over 42 frames). Whether the idler, sprocket and return rollers report belt speed / their own `Radius` was not tested. The road wheels of a track reported identical values in 90 of 131 frames, and otherwise a smooth front-to-rear gradient below 2%. A wheel without ground contact can report a very different value.

**The ray and the wheel centre.** Each road wheel casts a ray along the hull's down axis. The ray starts `RayStartOffsetUp` above the top position of the suspension (help text in the executable). The wheel centre sits `Radius` above the contact point, along the hull's up axis: the script value (contact height + `WheelGetRadius`) matched the engine's own suspension signal within 1.25 mm on average. The travel from the modelled position (+ = up, hull coordinates, bone = the `Pivot` bone at rest) is:

```math
x = y_{\text{contact}} + \text{Radius} - y_{\text{bone}}, \qquad -\text{MaxTravelDown} \le x \le \text{MaxTravelUp}
```

The engine clamps its own signal to this range: from exactly −0.150 to +0.300 with `MaxTravelDown` 0.15 and `MaxTravelUp` 0.30. Because the ray starts above the bump stop, the contact itself can lie higher: up to +0.382 m on rocks and mines. A script that computes the travel from the contact must clamp it.

**Spring and dampers.** `SpringRate` is in N/mm (help text), so k = 1000 · `SpringRate` N/m. The spring is free at full droop:

```math
F_{\text{spring}} = k \cdot (x + \text{MaxTravelDown})
```

The zero point at full droop was measured with two `MaxTravelDown` values; the linear shape follows the help text. At rest each road wheel carries m·g/n, so the rest travel is m·g/(n·k) − `MaxTravelDown` (2A6: 0.097 m − 0.10 = −3 mm; table in the [attribute reference](07-attribute-reference.md#suspension-inside-roadwheel-only)). The per-wheel rest travel, front to rear, was −1, −2, −2, −3, −4, −4, −5 mm on both tracks; the same model with the 2A6 centre of mass predicts −1.0 to −5.1 mm. The dampers apply `CompressionDamper` while compressing and `RelaxationDamper` while decompressing, in N·s/m. With m/14 per wheel the natural frequency is about 1.6 Hz and the damping ratios are about 0.28 and 0.39 (computed, not measured).

**Contact loss.** When a wheel's ray finds nothing, `WheelHasContact` is false and the wheel hangs at full droop: the engine's signal is exactly −`MaxTravelDown`. On the flipped tank every logged wheel showed no contact for the 8.5 s it lay on its roof.

**Hull pitch at rest.** The hull tilts until the spring moments balance the centre of mass:

```math
\theta \approx \frac{M g\,(\bar z - z_{\text{COM}})}{k \sum_i (z_i - \bar z)^2}
```

θ in radians, + = nose up, z forward. The sum runs over all road wheels; z̄ is their mean z (0.056 m on the 2A6). With k = 450 kN/m this gives 2.06° per metre of centre of mass shift (model). Two measured points give 2.18°/m: +0.954° with `CenterOfMass 0 1 0`, −0.049° with `0 1 0.46`. The z offset that levels the hull exactly is 0.437 (predicted). Under acceleration the hull pitched about 0.2° per m/s², nose up when accelerating ([Suspension and ride height](13-suspension.md)).

**Centre of mass and the physics frame.** The centre of mass is the model's own one (its `COM_` point) plus the `RigidBody` `CenterOfMass` offset. `WheelGetPosition` uses the internal physics frame, whose origin is that centre of mass. `GetWorldTransform` is documented as "internal physics space", probably the same frame (not tested). With displacement 0 (full droop), every 2A6 wheel returned y = −1.899 = (0.4186 − 0.15) − 2.1678. That is how the 2A6 centre of mass was located: (0, 2.168, 0.079) in hull coordinates, 2.17 m above the ground.

**Wheel animation.** The simulation also feeds `CarProcAnimComponent` with a rotation and a suspension input per wheel, but in 1.8.0.13 most tracked wheel bones received another wheel's values. The best-fitting model (inferred): the component finds the wheel by its pivot index g = 13 · track + part, but reads the values from a table laid out per axle (track g mod 2, part g div 2). One alternative order of the parameters was never tested in a freshly opened world. The 2A6 drives its bones from the wheel getters instead ([Road wheels, idler, sprocket and return rollers](14-running-gear.md)).

### The link chain (rope) step

Everything in this section was reverse-engineered from the 1.8.0.13 executable, unless it says "observed". The chain is optional: the simulation drives without it.

**Built once.** When `TrackPositions` has at least 2 entries and `TrackLength` is greater than 0, the component creates two ropes, one per track. It spawns 2 × N plain entities with the `TrackSegment` model as children of the hull, without physics; N is the number of `TrackPositions` entries. The 2A6 has 146 (counted in the log). Both ropes start from the same `TrackPositions` loop.

**One rope step.** Each simulation step adds its dt to an accumulator. While the accumulator holds at least h = 0.0083 s (a fixed step, about 1/120.5 s), the rope runs one substep and subtracts h:

1. Verlet integration of every point with step h and gravity (the world gravity turned into the hull frame).
2. 25 iterations of:
   - relaxing the distance constraint between each point and the next one (closing the loop) towards `TrackLength`, with stiffness 1.0;
   - colliding the points with the wheel circles.

**Wheel circles.** There is one circle per `TrackPart`. Its radius is the part's sim `Radius`, and its centre is the part's current simulated position, so the road wheel circles move with the suspension. A point inside a circle is pushed radially out to the circle. Nothing else collides: no terrain, no objects, no hull. The rope has no notion of which side of a wheel it belongs on.

**Snap to grounded wheels.** For every circle that is not a sprocket or an idler and whose wheel has ground contact, each point within ±0.1 m (in z) of the centre and not above it is set to the bottom of the circle. This holds the bottom run under the road wheels on the ground. Without contact (airborne, flipped) there is no snap.

**Sprocket drive.** Only the sprocket drives the rope, and it grips one point at a time: the first point found inside its circle. The gripped point is rotated about the sprocket centre by −dt · ω each step. It is released as soon as it lies behind or below the sprocket centre, and the next point that touches the circle is gripped. Observed: the debug overlay prints a number at each sprocket that counts down by about one per 0.21 m of travel forward, up in reverse, and wraps at 73; it is probably the gripped point's index.

**Speed reference.** ω comes from the change of `TrackPart` 1's angle per step. The chain drive at a part of radius *r* is r₀ · ω / *r*, and the track speed the engine computes is r₀ · ω · 3.6 km/h, where r₀ is the `Radius` of `TrackPart` 0. That is why parts 0 and 1 must be road wheels ([attribute reference](07-attribute-reference.md#tracks-and-trackparts)).

**Link placement.** Every frame, in the component's post-frame event, each link is placed at the midpoint of a point *p* and the next point *q*. With Δ = *q* − *p*:

- position = (`TrackOffset.x`, mid.y + `TrackOffset.y`, mid.z + `TrackOffset.z`);
- axes X = (1, 0, 0), Y = (0, −Δz, Δy) / |Δ|, Z = −Δ / |Δ|, then multiplied by the vehicle's world matrix.

On the bottom run this frame is the identity, which is why the link model is authored as a bottom-run link. Both sides use the same frame, not mirrored. Because a link sits on a chord, where the chain wraps a circle its ground face lies inside the circle by r − √(r² − (p/2)²): 13 mm on a road wheel, 17 mm on the idler or the sprocket, 42 mm on a return roller (computed for p = 0.2101 m).

**What it does not have.**

- No collision with the ground.
- No script API: the getters never touch the chain, and nothing can move or reset it from script.
- The only reset is *Reset tracks*, an entry of the Diag build's menu (not tried). The same menu has *Draw tracks*, which switches the chain overlay (wheel circles, rope line and stat boxes) on and off (observed), *TrackSim* and *Track gravity* (switched off briefly in the video, with no visible change) and *Track dynamic* (not tried). See [Vehicle debug diag and telemetry](16-debugging.md).

The per-track box of the debug overlay shows *Sim*, *Substep*, *Momentum*, *Energy*, *AngVel* and *Speed*. In every frame read from the videos it said "Sim: Paused", "Substep: 0/25", Momentum 0 and Energy 0, even at 27 km/h, while the rope line visibly moved. Only *AngVel* and *Speed* changed. What the first four fields report is open.

**Why it sags and comes off (inferred from the rules above and the observed failures).** Gravity pulls every point, the ground never pushes back, and the snap only holds points near grounded road wheels. Slack therefore hangs in the gaps between road wheels and around the sprocket and the idler. When the wheels lose contact (a flip, a mine blast), the snap switches off and the wheels drop to full droop, which needs more chain than there is. The constraint relaxation then drags points across the circles, and the collision pushes each point out on whichever side is nearer. A chain pulled past the sprocket centre stays outside, and nothing puts it back.

The arithmetic for the 2A6 (arc length around the sim circles, computed): the path at rest is 15.284 m. It grows by 0.035 m when all road wheels droop 3 cm, 0.059 m at 5 cm, 0.122 m at 10 cm and 0.189 m at 15 cm. A chain of 73 × 0.2101 = 15.337 m had 5 cm to spare at rest and was 13.5 cm short at the flip's full 15 cm droop; 73 × 0.2110 = 15.403 m has 12 cm to spare and sags visibly. As a polygon of straight segments the rope needs a little less length, if its constraints act like rigid links; whether they are rigid or only resist stretching is open. Tables and video frames: [Track links](15-track-links.md).

Observed failures:

| Setup | What happened | Where |
| --- | --- | --- |
| 5 cm of slack, hull 5 cm high | After a flip, the left rope came off the sprocket and stayed off | Diagnostic video 1, flip at 3:09–3:14, sprocket bare from about 3:48 |
| 12 cm of slack, hull at its modelled height | The slack hung below the road wheels and piled up behind the sprocket at about 27 km/h | Diagnostic video 3, 0:06 and 0:15–0:19 |
| 12 cm of slack, mines | The tank was thrown; the left rope came off and lay across a rock, attached only at the rear | Diagnostic video 3, 0:23–0:28 |

The Leopard keeps the native rope running but hides its links, and draws its own links with a scripted chain that is rebuilt from the wheel positions every frame. That chain cannot derail by construction; it was not yet tested through a flip or a mine blast ([Track links](15-track-links.md)). Open: whether the wheel RPM or the drive depend on the native rope (it was never removed to test), and the frame cost of the 146 hidden rope links plus the 146 scripted ones.

![Side view with vehicle debug on: the hidden native rope's top run whips above the return rollers while the scripted links stay in place](../images/hero-debug-side.gif)

*Test video A, 3:06–3:11, 11 to 21 km/h. The hidden native rope (red and green line) keeps running under the scripted links, and its top run whips above the return rollers. Model "Leopard 2 A6" by buh-late, CC BY 4.0, modified.*

### Speeds cross-check

**The chain moves at sprocket ω × 0.34.** In every frame read from the test videos, the rope box's *Speed* divided by *AngVel* was 0.34000, the sprocket `Radius` (measured). Example from test video A at 1:45, 3rd gear: AngVel 18.413 rad/s (175.83 rpm) and Speed 6.2605 m/s; the engine turned at 1456 rpm, and 1456 / (1.8 × 4.6) = 175.8 rpm. So *AngVel* is the sprocket's angular velocity and *Speed* the chain speed in m/s. In that frame the chain ran at 22.5 km/h and the HUD showed 23.0 km/h, with the engine braking (see the slip table below).

**The formula holds for the chain.** *Feedback* = sprocket rpm × gear ratio × `Ratio` was checked in 1st (27.6), 2nd (15.18), 3rd (8.28) and reverse (10.58). At the end of 1st gear the engine ran at 2935 rpm: 2935 / 27.6 × 2π/60 × 0.34 × 3.6 = 13.63 km/h, and the HUD showed 13.6 km/h. The road wheels give the same belt speed: `WheelGetRPM` × 2π/60 × 0.418.

Conversion constants for the 2A6, all for the belt speed:

| Quantity | Per km/h |
| --- | --- |
| Road wheel (r 0.418) | 6.346 rpm |
| Sprocket (r 0.34) | 7.802 rpm |
| Engine in 1st / 2nd / 3rd / 4th | 215.3 / 118.4 / 64.6 / 35.9 rpm |
| Engine in reverse | 82.5 rpm |

**Earlier check on the 2A7 build.** On the earlier Leopard 2A7 build (sprocket 0.355, `Ratio` 6.0 at the time), a single reading showed 45 km/h at about 2000 rpm in 4th gear, and the formula gives 44.6 km/h.

**The hull does not move at the chain speed (open).** On the 2A6, the hull speed differs from the chain speed by a few percent, and the sign depends on whether the engine drives or brakes:

| Situation | Hull speed against chain speed | Source |
| --- | --- | --- |
| Pulling away in 1st | The tracks run 20–100% faster than the hull | Debug panel, test video A |
| Under throttle, 2nd–4th gear | The hull is about 6% slower (effective radius 0.320 m, median of 519 samples; middle half 0.303–0.328) | `LEOTEL` telemetry |
| Coasting on engine braking | The hull is about 2% faster (effective radius 0.347 m, median of 114 samples) | `LEOTEL` telemetry; test videos A and B |
| Hard braking (brake 6.0) | The tracks skid: 1.5 m/s against 3.75 m/s | Debug panel, test video B, 0:11 |

The effective radius is the hull speed divided by the sprocket ω computed from the engine rpm, the gear and `Ratio` (clutch 1, no brake, above 8 km/h). The telemetry has no steering input, so these samples include turns and slopes. The sign change fits longitudinal slip of the tracks, but the slip model of the simulation was not reverse-engineered. This gap is open: re-measure it in a straight line on flat ground, where the debug overlay shows both speeds at once.

**Top speed.** The formula gives a 4th-gear chain speed of 72.4 km/h at 2600 rpm (`RpmMaxPower`) and 75.2 km/h at 2700 rpm (`RpmRedline`). The 2A6 top speed was not measured; the highest logged speed was 47.6 km/h. The earlier 2A7 build measured 74.3 km/h ([Tuning](10-tuning.md)).

![Launch and upshifts with the vehicle debug on: each track's box shows AngVel and Speed rising with the engine rpm](../images/launch-upshifts.gif)

*Test video A, 1:34–1:39: launch in 1st and upshifts to 3rd. In each track's box, Speed = AngVel × 0.34. Under throttle the chain runs ahead of the hull: at 1:37, 4.77 and 4.56 m/s against 15.9 km/h (4.42 m/s). Model "Leopard 2 A6" by buh-late, CC BY 4.0, modified.*

### Network (multiplayer)

`NwkTrackedMovementComponent` sends the driver's commands to the server, which runs them again. A force applied by script only on the driver's machine is **not** part of these commands. In multiplayer, it must also run on the server. This applies to the scripted reverse, which runs only on the driver's machine today.

Visual scripts are different. `LEO_RunningGearComponent` (wheels and scripted links) and `LEO_TrackBeltComponent` (static belts) need no replication: each machine reads its own simulation and moves its own bones and its own link entities, which are spawned locally and never replicated. The running gear component skips the bone and link updates when the hull is more than 300 m from the camera, and does nothing on a dedicated server: no bone updates and no scripted links there.

Not tested: multiplayer as a whole. In particular, whether the wheel getters return valid contacts and RPM on proxies (other players' clients), whether the scripted chain animates there, and whether proxies step the native rope.

---

[← Attribute reference](07-attribute-reference.md) · [Contents](../../README.md) · [Bugs and limitations (1.8.0.13) →](09-bugs.md)
