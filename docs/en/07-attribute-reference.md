> [🇧🇷 Português](../pt-BR/07-attribute-reference.md) · 🇬🇧 English

[← Converting an existing vehicle](06-converting.md) · [Contents](../../README.md) · [How it works inside →](08-internals.md)

# Attribute reference

The names, defaults and ranges below were read from the attribute registration code of the 1.8.0.13 executable. "—" means the default does not appear in the binary. Quoted descriptions are the help texts stored in the executable.

The *Leopard* column shows the values of the worked example, the **Leopard 2A6** (complete block in [`examples/leopard2a6`](../../examples/leopard2a6/VehicleTrackedSimulation_Leopard2A6.et.txt)). The *Effect* column says what was measured in game, observed in the test videos or read in the code. A few results were measured on the earlier Leopard 2A7 build of this guide, and they say so.

> When you change any of these values, close the world in the World Editor and open it again. A vehicle that is already in an open world keeps its old simulation values: with `MaxTravelDown` changed from 0.15 to 0.10 in the prefab, the tank in the open world still rested at the old height (measured).

### Simulation Tracked (top level)

| Attribute | Default | Leopard | Effect |
| --- | --- | --- | --- |
| `SolverSubsteps` | — (range 1–20) | 4 | Substeps per frame. With 60 Hz and 4 substeps, dt = 1/240 s |
| `FrictionCurveLongitudinal` | — | flat curve 1.0 | **Required.** Friction in the direction of travel |
| `FrictionCurveLateral` | — | flat curve 1.0 | **Required.** Lateral friction |
| `FrictionBaseLongitudinal` / `FrictionBaseLateral` | — (range 0–inf) | inherited | Friction base of the old model |
| `V2FrictionBaseLng` | — | 0.95 (vanilla) | Longitudinal friction of the V2 model |
| `V2FrictionBaseLat` | — | 2 (vanilla) | Lateral friction of the V2 model |
| `V2FrictionSurfaceLng` / `V2FrictionSurfaceLat` | `Tread` | Lat = `NonTread` (vanilla) | `Tread` or `NonTread` enum |
| `V2FrictionSteerEnabled` | true | inherited | Reduces lateral friction when turning |
| `V2FrictionSteerFactorCurve` | — | vanilla (1 → 0.2) | Friction factor as a function of the turn |
| `V2FrictionSteerFactorLngCurve` | — | vanilla (1, 0, 1) | Factor by wheel position, from front to rear |
| `RegenerativeSteering` | — | 1 | Torque goes to the outer track in a turn |
| `NeutralSteering` | — | 1 | In a pivot turn, the inner track spins backward. Observed on the 2A6: the two tracks counter-rotate |
| `PivotSteering` | — | 1 | In a pivot turn, the inner track locks. **With 0, the earlier 2A7 build did not turn in place** |
| `PivotSteeringThreshold` | 0.5 (0–1) | 0.5 | Minimum steering input for a pivot turn |
| `BrakeSteering` | — | 0 | Brakes the inner track in a turn |
| `BrakeSteeringThreshold` | — | inherited | Minimum input for brake steering |
| `RetarderBaseTorque` | 10 | 0 | Brake that grows with speed. **With 10, the earlier 2A7 build did not go past about 54 km/h** |
| `RetarderThrottleSpeed` | 14.5 m/s | 20 | With 14.5, the earlier 2A7 build locked at about 51 km/h |
| `InertiaOverrideEnabled` / `InertiaOverride` | false / 1 1 1 | inherited | Overrides the inertia tensor |
| `RaycastLayer` / `LiquidsLayers` | — | `VehicleCast` / `Liquids` (vanilla) | Layers for the wheel rays and for water |

### Engine, clutch, gearbox and differential

| Block | Attribute | Default | Leopard | Effect |
| --- | --- | --- | --- | --- |
| `Engine` | `Inertia` | — (>0) | 3 | Engine inertia. It must be greater than zero |
| `Engine` | `InertiaCoupled` | — | not used | Extra inertia with the clutch engaged |
| `Engine` | `MaxPower` | 100 kW | 1100 | Maximum power |
| `Engine` | `MaxTorque` | 186 N·m | 4700 | Maximum torque. The debug panel showed up to 4682 N·m on the 2A6 |
| `Engine` | `RpmMaxPower` | 5800 | 2600 | RPM at maximum power |
| `Engine` | `RpmMaxTorque` | 4400 | 1600 | RPM at maximum torque |
| `Engine` | `Steepness` | — (>0) | 50 | Shape of the torque curve. Vanilla engines use 4 to 50 |
| `Engine` | `Friction` | — (>0) | 470 | Internal friction. **1400 ate a third of the torque** (earlier 2A7 build). With the throttle at 0, the engine-braking torque in the debug panel fit −`Friction` · (rpm − `RpmIdle`) / (`RpmMax` − `RpmIdle`), down to −471 N·m (observed on the 2A6) |
| `Engine` | `BrakingTorqueRatio` | 0.15 | 0.15 | Engine braking as a fraction of `MaxTorque`, going by the name. Its effect was not isolated: the engine braking seen on the 2A6 matched `Friction` |
| `Engine` | `RpmIdle` / `RpmRedline` / `RpmMax` | 1250 / 6250 / 7000 | 700 / 2700 / 3000 | Idle, cutoff and limit. The order must be ascending |
| `Engine` | `HasGovernor` | — | 1 | RPM limiter. Above `RpmRedline` the torque collapses: 2462 N·m at 2719 rpm, 86 N·m at 2935 rpm (observed, 2A6, 1st gear) |
| `Clutch` | `MaxClutchTorque` | 225 | 9000 | **Ignored** by the tracked simulation: the clutch is just a 0–1 multiplier |
| `Gearbox` | `Forward { ... }` | — | 6.0 3.3 1.8 1.0 | Forward gear ratios |
| `Gearbox` | `Reverse` | — | 2.3 | **One** reverse ratio. The sign is forced to negative (see [Bugs](09-bugs.md)) |
| `Gearbox` | `Efficiency` | — | 0.9 | Transmission efficiency |
| `TripleDifferential` | `Ratio` | — (>0) | 4.6 | Final drive ratio. With the 0.34 m sprocket, 4th gear gives about 72–75 km/h in theory; the 2A6 top speed was not measured (see [Tuning](10-tuning.md)) |
| `TripleDifferential` | `SteeringRatio` | — | 6 | Maximum ratio when turning on the move. **Without it, the earlier 2A7 build did not turn.** Observed on the 2A6 at full steering in 1st gear: the inner track turned at 1/6 of the outer one (99.08 against 16.51 rpm) |
| `TripleDifferential` | `SteeringThreshold` | 0.01 | inherited | Minimum input to turn |
| `TripleDifferential` | `NeutralSteeringRatio` | — | 12 | Pivot turn ratio. Observed on the 2A6: in a pivot turn, the drivetrain-side *Feedback* rpm was the mean of the two tracks' absolute rpm × 12 |

The `Output` attribute of the blocks only links the engine → clutch → gearbox chain. Do not set `Output` on the gearbox; vanilla does not set it either.

### Wheels and suspension

The simulation has one template per wheel type: `RoadWheel`, `Sprocket`, `Idler` and `Roller` (the return roller). Each `TrackPart` of a track takes the template of its `Type`. The `Roller` template works: the 2A6 loads with 4 return rollers per side and reports 13 wheels per track. Write it like the others:

```
    Roller Roller "{EC9CBBD192126CCB}" {
     Mass 20
     Radius 0.152
     BrakeTorque 0
    }
```

| Block | Attribute | Default | Leopard (road wheel / sprocket / idler / roller) | Effect |
| --- | --- | --- | --- | --- |
| all four | `Mass` | 12 kg | 110 / 250 / 150 / 20 | Wheel mass. Must be greater than zero (`TrackWheel's mass is incorrect!`) |
| all four | `Radius` | 0.6 m | 0.418 / 0.34 / 0.335 / 0.152 | Distance from the wheel centre to the **ground face of the track**, not to the rim. Road wheel: the wheel centre sits `Radius` above the contact point, so it sets the ride height. Sprocket: sets the track speed (speed = sprocket ω × `Radius`). Every part's radius is also a circle that the native link chain wraps. Must be greater than zero |
| all four | `BrakeTorque` | 4000 N·m | 6000 / 30000 / 0 / 0 | Brake torque per wheel. Must be zero or more |

How the 2A6 radii were chosen (measured on the model): each sim radius is the visual rim radius plus about one link thickness. Road wheel rim 0.345 + 0.073 = 0.418; idler 0.264 + 0.071 = 0.335; return roller 0.0847 + 0.067 = 0.152. At the sprocket, the belt's inner face is 0.262 m from the centre; with the 0.076 m shoe that gives 0.338, rounded to 0.34.

#### Suspension (inside `RoadWheel` only)

| Attribute | Default | Leopard | Description in the executable | What we measured |
| --- | --- | --- | --- | --- |
| `MaxSteeringAngle` | — | 0 | "Unit: degrees. Maximum wheel rotation angle (positive number). For non steering wheels just leave it at 0." | Always 0 on a tracked vehicle |
| `SpringRate` | 20 | 450 | "Unit: N/mm. Spring force per mm. Tells how much force is needed to compress the spring by one mm." | 450 = 450 kN/m. The spring is **free (zero force) at full droop** (−`MaxTravelDown`). The static compression from full droop is m·g/(n·k): 62300 · 9.81 / (14 · 450 000) = 0.097 m. Confirmed by two runs |
| `CompressionDamper` | 2500 (probably; the string sits next to the name) | 25000 | "Unit: N.s/m. Compression damper force per 1 m/s. Used when suspension is compressing." | Damping ratio about 0.28 per wheel (computed, not measured) |
| `RelaxationDamper` | — | 35000 | "Unit: N.s/m. Relaxation damper force per 1 m/s. Used when suspension is decompressing." | Damping ratio about 0.39 per wheel (computed, not measured) |
| `MaxTravelUp` | 0.15 | 0.30 | "Unit: m. How much can spring compress its natural length. It's the distance that the suspension can be compressed from modeled position." | Bump stop above the modelled wheel position. The engine's own suspension signal stops at exactly +0.30 |
| `MaxTravelDown` | — | 0.10 | "Unit: m. How much can spring stretch its natural length. It's the distance that the suspension can be expanded from modeled position." | Full droop below the modelled position, where the spring force is zero. It sets the ride height (see below). A wheel without ground contact hangs here |
| `RayStartOffsetUp` | — | 0.5 | "Unit: m. Extension of raycast start above top position of suspension. Suspension total length is not affected." | The ray runs along the hull's down axis. Because it starts above the bump stop, it can find ground above `MaxTravelUp`: contact-derived travel reached +0.382 m on rocks and mines, while the engine's signal stopped at +0.30 |

`MaxTravelDown` sets where the tank rests. The spring force is zero at full droop, so the wheel rests this far from its modelled position (+ = up):

```math
x_{\text{rest}} = \frac{m \cdot g}{n \cdot k} - \text{MaxTravelDown}
```

| `MaxTravelDown` | Predicted rest travel | Measured rest travel (2A6, stationary) | Result |
| --- | --- | --- | --- |
| 0.15 | −53.0 mm | −53.0 mm (per-frame mean of a track's 7 wheels, 13 of 14 stationary frames) | Hull 5.3 cm higher than modelled |
| 0.10 | −3.0 mm | −3.0 mm (median, 256 samples on flat ground); −1 to −5 mm front to rear | Hull at its modelled height |

Rule: set `MaxTravelDown` ≈ m·g / (n·k), with n the number of road wheels on **both** sides and k = `SpringRate` × 1000 N/m. Then the tank sits at the height you modelled. Details and the hull pitch in [Suspension and ride height](13-suspension.md).

#### Tracks and TrackParts

| Block | Attribute | Default | Leopard | Effect |
| --- | --- | --- | --- | --- |
| `Tracks` | `Track Left`, `Track Right` | — | both | Exactly two tracks (`Invalid number of tracks! Must be 2!`). The first one is track 0 in the script API and gets `TrackOffset1` |
| `Track` → `Wheels` | `TrackPart` list | — | 13 per track | At least 3 per track. The position in this list is the wheel index in the script API |
| `TrackPart` | `Type` | Road Wheel | see below | `Road Wheel` (default: write no `Type`), `Sprocket`, `Idler`, `Return Roller`. Write the last one in quotes: `Type "Return Roller"` (accepted) |
| `TrackPart` → `Pivot` | `Bone`, `Position`, `Rotation` | — | `Bone "v_wheel_l01"` … | The wheel bone and an optional offset. Its rest position is the modelled wheel centre: the suspension travel is measured from it |

The order of the `TrackPart` list matters (reverse-engineered from the 1.8.0.13 executable, consistent with every test):

- The sprocket and the idler must be at the same index on both tracks (`Wheels - sprocket mismatch between tracks`, `... idler ...`).
- Index 0 and index 1 must be road wheels. The link chain takes its speed from the radius of part 0 and the rotation of part 1. With the idler first (the earlier 2A6 order, when the idler `Radius` was still 0.264), the chain would run at 0.264 / 0.418 = 63% of the road wheel speed.
- The Leopard order, the same on both sides: road wheels `Wheel_X01`…`Wheel_X07` (index 0–6, front to rear), `Idler_X` (7), `Sprocket_X` (8), `Roller_X01`…`Roller_X04` (9–12). [`LEO_RunningGearComponent`](../../scripts/LEO_RunningGearComponent.c) expects this order.

### Aerodynamics

| Attribute | Default | Leopard | Effect |
| --- | --- | --- | --- |
| `ReferenceArea` | 3 (vanilla) | 9.5 | Frontal area in m² |
| `DragCoefficient` | 0.5 | 0.9 | Drag coefficient |

### RigidBody (hull)

| Attribute | Leopard | Description in the executable | Effect |
| --- | --- | --- | --- |
| `Mass` | 62300 | — | Total mass in kg |
| `CenterOfMass` | `0 1 0.46` | "Offset of center of mass. Relative to one defined in model (if applicable)." | An **offset**, added to the centre of mass defined in the model (a `COM_` point). The 2A6 hull has `COM_hull` at (0, 1.168, −0.381), so its centre of mass is at (0, 2.168, 0.079): 2.17 m above the ground (measured through `WheelGetPosition`). The z offset 0.46 levels the hull: pitch −0.049°, against +0.954° nose-up with `0 1 0`. The y offset was not retuned; 2.17 m is probably too high (open) |
| `LinearSleepingThreashold` / `AngularSleepingThreashold` | 0.4 / 0.5 | "Below this linear velocity the body goes to sleep" / "Below this angular velocity the body goes to sleep" | A stopped tank goes to sleep. Telemetry then repeats identical samples; that is the sleep, not a broken logger. The spelling is the engine's |

### SCR\_TrackedControllerComponent

It sits inside `BaseVehicleNodeComponent`. The defaults come from the executable. The *Vanilla* column shows what `TrackedVehicle_Base.et` changes. The Leopard changes only `ThrottleReverseTarget` (1).

| Attribute | Default | Vanilla | Effect |
| --- | --- | --- | --- |
| `Type` | 0 | — | The code never reads it |
| `TransmissionRND` | false | — | Three-position gearbox (R, N, D). It neither unlocks nor blocks reverse |
| `SteeringForwardSpeed` / `SteeringCenterSpeed` | — | `19 1.1 100 0.02` / `19 1.1 60 1.8` | Steering speed when turning and when centering. Format not decoded |
| `ThrottleTurbo` | 0.2 | — | Share of the throttle reserved for turbo. W alone gives 80% (`engineThrust` 0.80, also on the 2A6) |
| `ThrottleTurboTime` | 0.15 s | — | Time for the turbo to kick in |
| `ThrottleReverseTarget` | 0.2 | — | Throttle used in reverse. **Use 1** (see [Bugs](09-bugs.md)) |
| `ClutchUncoupleTime` / `ClutchCoupleTime` | 0.25 / 0.35 s | — | Time to release and to engage the clutch |
| `ClutchCoupleForwardRpm` / `ClutchCoupleReverseRpm` | 0 | — | Engagement RPM when pulling away. It only matters at takeoff |
| `ClutchCoupleUphillFactor` | 1 | — | Engagement adjustment on a climb |
| `BrakingCurve` | — | `0.3 0.7 0.6 1.4 1 2.1` | \[fraction, seconds\] pairs: 30% at 0.7 s, 60% at 1.4 s, 100% at 2.1 s |
| `BrakeTurboTime` | — | — | Brake time with turbo |
| `RpmSmoothing` | 0.7 | 0.95 | RPM filter for gear shifts |
| `SlopeSmoothing` | 0.75 | — | Slope filter |
| `Latency` | 0 | 0.1 | Shift delay |
| `UpShiftFactor` / `UpShiftFactorDownhill` | — | — | RPM factor for upshifts (uphill / downhill) |
| `DownShiftFactor` / `DownShiftFactorDownhill` | 0.4 / — | — | RPM factor for downshifts |
| `PeakTorqueUpshiftingHysteresis` / `...Downshifting...` | -0.14 / -0.2 | — | Hysteresis around maximum torque |
| `SteeringFactorUpshift` / `SteeringFactorDownshift` | 0.1 / 0.05 | 0.1 / 0.05 | Shift correction when turning |
| `TurboShiftFactor` | 1.1 | — | Higher shift points with turbo |
| `PeakPowerShiftingModeTurbo` / `...Normal` | true / — | — | Shifts at maximum power |
| `PeakPowerUpshiftingHysteresis` / `...Downshifting...` | 0 / -0.1 | — | Hysteresis around maximum power |
| `PeakPowerUpShiftFactorUphill` and related | 0.7 | — | Factors for the maximum power mode |
| `MaxStartupTime`, `MaxStartupAttempts`, `EngineStartupChance`, `ShutdownTime` | — | 0.6 / 10 / 100 / 600 | Engine startup (shared by all vehicles) |

### VehicleTrackedSimulation component: the link chain (outside the Simulation)

These seven attributes build the engine's own link chain, a 2D rope of link entities around the wheels (how it moves: [How it works inside](08-internals.md#the-link-chain-rope-step)). They sit on the `VehicleTrackedSimulation` component itself, next to the `Simulation Tracked` object, not inside it. None has a description in the executable.

| Attribute | Default | Leopard | Effect |
| --- | --- | --- | --- |
| `TrackSegments` | 0 | 73 | Read only by *Generate track positions*, as the number of points to make. At runtime the number of links per track is the number of `TrackPositions` entries (reverse-engineered). Keep both equal |
| `TrackPositions` | — | 73 `z y` pairs | One list of (z, y) points in hull model space (z forward, y up), shared by both tracks. Write it as one flat list `{ z y z y ... }`: the 2A6's 146 numbers load as 73 points. Index 0 is on the top run, and the index must increase **forward** along the top run (the engine's own winding). Place the points on the outer path around the sim-radius circles |
| `TrackLength` | — | 0.2110 | Rest distance between neighbouring points, so the chain is N × `TrackLength` long. It must be a little longer than the path around the wheels. 2A6 path at rest: 15.284 m. With 73 × 0.2101 (the real link pitch) only 5 cm were left, and the rope came off the sprocket after a flip. With 73 × 0.2110 there are 12 cm, and the slack sags (no ground collision). See [Track links](15-track-links.md) |
| `TrackThickness` | — | 0.076 | Stored but never read in 1.8.0.13 (reverse-engineered). The 2A6 records its shoe thickness here; it has no effect |
| `TrackOffset1` | — | `-1.3281 0 0` | Offset of the first track's chain (`Tracks[0]`, Left on the Leopard). x is the lateral position of the belt centre. Both chains are the same 2D loop, shifted sideways |
| `TrackOffset2` | — | `1.3281 0 0` | The same for the second track (Right) |
| `TrackSegment` | none | `{B50AEDC702735F50}Assets/Vehicles/Tracked/Leopard2A6/Leopard2A6_track_link.xob` | The link model. The engine spawns one child entity per point per track (146 on the 2A6), without physics. Author the link as it sits on the bottom run: origin at the centre of the shoe pair, ground face at y = 0, +Y up, +Z towards the front. No link asset ships with the game: make your own ([Track links](15-track-links.md)) |

The chain is created only when `TrackPositions` has at least 2 entries and `TrackLength` is greater than 0. Leave these attributes out for the first drive test: the simulation drives without a chain. Without a chain, do not open the *Show vehicle debug* diag: it crashed Workbench (see [Bugs](09-bugs.md)). With a chain present it ran without a crash.

The native chain has gravity but no collision with the ground, and no script API can reset it. The Leopard keeps it running but hidden, and draws its own links by script ([Track links](15-track-links.md)).

#### The *Generate track positions* action

The executable registers *Generate track positions* as editor action 1 of `VehicleTrackedSimulation` (action 2 is *Compute local inertia*). What it does (reverse-engineered, not run):

1. It reads the live wheel circles of track 0, so it needs a placed, initialised vehicle with `TrackSegments` and `TrackLength` already set.
2. It places `TrackSegments` points on an ellipse around the model origin.
3. It runs 1000 rope steps of 1/60 s with gravity and the wheel circles.
4. It writes the points into `TrackPositions`, index 0 at the top, increasing forward.

The action was not found in the Workbench UI during this work. It may be in the component's context menu in the World Editor: the executable's help text for the physics component's `MasslessInertia` attribute says it "Can be computed via context menu on component." This is a guess and was not verified. The 2A6 list was made offline with [`tools/trackpositions.py`](../../tools/trackpositions.py) (MIT). The script samples every `TrackPart` circle at its sim radius, takes the convex hull, orders it so the top run moves forward, and resamples it at equal arc length. Edit the circle list at the top of the script for your tank, then run `python trackpositions.py 73 0.2101` (number of links, link pitch).

### Script API: the wheel getters

`VehicleTrackedSimulation` exposes the state of every wheel. Wheel indices are (track, part): track 0 is the first `Track` in `Tracks`; part is the position in its `Wheels` list. The 2A6 animates its whole running gear from these getters ([Road wheels, idler, sprocket and return rollers](14-running-gear.md)).

| Method | Returns | What we know |
| --- | --- | --- |
| `TrackedControllerComponent.GetSimulation()` | `VehicleTrackedSimulation` | The recommended way to get the simulation |
| `IsValid()` | `bool` | From `VehicleBaseSimulation`: "Returns true if this component was properly initialized." |
| `TrackCount()` | `int` | 2 once the simulation is loaded, 0 before. It is a constant, not read from the prefab (reverse-engineered). It became valid 0.8–1.3 s after the vehicle was created in Play, before the engine was started (measured) |
| `WheelCount()` | `int` | Parts **per track**: 13 on the 2A6 (measured) |
| `WheelHasContact(track, wheel)` | `bool` | Ground contact of the wheel's ray. False when airborne or flipped |
| `WheelGetContactPosition(track, wheel)` | `vector`, world space | Returns a stored value without checking contact (reverse-engineered): read it only when `WheelHasContact` is true. Convert with `owner.CoordToLocal()`. Local y + `WheelGetRadius()` gives the wheel centre height; it matched the engine's own suspension signal within 1.25 mm on average (measured) |
| `WheelGetContactNormal` / `WheelGetContactMaterial` / `WheelGetContactEntity` | `vector` / `GameMaterial` / `IEntity` | Exist; not used in the tests |
| `WheelGetContactLiquidState` / `...LiquidMaterial` / `...LiquidPosition` | liquid contact | Exist; not tested |
| `WheelGetRadius(track, wheel)` | `float` | The template `Radius` |
| `WheelGetRPM(track, wheel)` | `float` | Internal angular velocity (rad/s) × 60/2π (reverse-engineered). Its sign is the sign of the forward speed (87 of 87 samples with all wheels in contact above 2 km/h, measured). For a road wheel it is the belt speed / its `Radius`: road wheel rpm = sprocket rpm × 0.34/0.418 (measured). Whether the idler, sprocket and return rollers report belt speed / their own `Radius` was not tested. A wheel without contact can report a very different value (−230.6 rpm in reverse, against 0–20 on the ground) |
| `WheelGetPosition(track, wheel, displacement)` | `vector` | The wheel centre in the internal physics frame, whose origin is the body's centre of mass. `displacement` 0 = full droop. On the 2A6 it returned y = −1.899 = 0.4186 − 0.15 − 2.1678 for every wheel (measured with `MaxTravelDown` 0.15). Add the centre of mass to get hull coordinates. The unit of `displacement` was not tested |
| `WheelGetRollingDrag` / `WheelSetRollingDrag` | `float` | Exist; not tested |

Rules for every script that reads them:

- Guard every frame. Until the simulation is loaded the getters return 0, false or a zero vector:

```c
VehicleTrackedSimulation simulation = controller.GetSimulation();
if (!simulation || !simulation.IsValid() || simulation.TrackCount() < 2)
	return;
```

- Keep `track < TrackCount()` and `wheel < WheelCount()` yourself. Only `WheelGetPosition` checks the wheel index, and no getter checks the track index; with a bad index the others read outside their arrays (reverse-engineered).
- Never call `GetInputs()`: it crashes Workbench. `GetThrottle()` always returns 0, `SetThrottle()` does nothing and `IsHandbrakeOn()` always returns false on the tracked simulation. `SetBreak(x, true)` stores 6.0, which is why `GetBrake()` can read 6.0.

### Available signals

| Signal | Exists? | Value |
| --- | --- | --- |
| `engineRPM` | yes | Engine RPM |
| `engineThrust` | yes | Applied throttle (0.8 with W, 1.0 with turbo) |
| `engineOn` | yes | Engine running |
| `brake` | yes | Brake input. It reads 6 when the controller holds the tank still |
| `gear`, `throttle`, `clutch`, `handBrake` | **no** | — |

About the 6: the *Brake* field of the debug panel shows 6.0 at a standstill, in pivot turns and at the end of a held brake (0.7 → 1.0 → 6.0), observed on the 2A6. `SetBreak(x, true)` stores 6.0 (reverse-engineered); whether every 6.0 comes from that handbrake path is open.

The executable stores more signal names in the same block of strings as the tracked simulation's attributes: `speed`, `steeringInput`, `suspensionMove`, `drivetrainRPM` (used by the vanilla LAV-25), `engineLoad` and `handbrake` (lower-case b). They were found by a string scan and **not tested** on the tracked simulation.

No per-wheel signal was found. `CarProcAnimComponent` does receive wheel inputs from the tracked simulation (the `.pap` input "rotation", editor range −360..360, and "suspension", in metres along the bone Y from −`MaxTravelDown` to +`MaxTravelUp`), but in 1.8.0.13 they land on the wrong wheels. Read the wheel state with the getters above instead ([Road wheels, idler, sprocket and return rollers](14-running-gear.md)).

---

[← Converting an existing vehicle](06-converting.md) · [Contents](../../README.md) · [How it works inside →](08-internals.md)
