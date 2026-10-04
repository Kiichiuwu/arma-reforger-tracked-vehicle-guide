> [🇧🇷 Português](../pt-BR/07-attribute-reference.md) · 🇬🇧 English

[← Converting an existing vehicle](06-converting.md) · [Contents](../../README.md) · [How it works inside →](08-internals.md)

# Attribute reference

The names, defaults and ranges below were read from the attribute registration code of the 1.8.0.13 executable. "—" means the default does not appear in the binary. The *Effect* column says what we observed in game or in the code.

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
| `NeutralSteering` | — | 1 | In a pivot turn, the inner track spins backward |
| `PivotSteering` | — | 1 | In a pivot turn, the inner track locks. **With 0, the tank did not turn in place** |
| `PivotSteeringThreshold` | 0.5 (0–1) | 0.5 | Minimum steering input for a pivot turn |
| `BrakeSteering` | — | 0 | Brakes the inner track in a turn |
| `BrakeSteeringThreshold` | — | inherited | Minimum input for brake steering |
| `RetarderBaseTorque` | 10 | 0 | Brake that grows with speed. **With 10, the Leopard did not go past \~54 km/h** |
| `RetarderThrottleSpeed` | 14.5 m/s | 20 | With 14.5, the speed locked at \~51 km/h |
| `InertiaOverrideEnabled` / `InertiaOverride` | false / 1 1 1 | inherited | Overrides the inertia tensor |
| `RaycastLayer` / `LiquidsLayers` | — | `VehicleCast` / `Liquids` (vanilla) | Layers for the wheel rays and for water |

### Engine, clutch, gearbox and differential

| Block | Attribute | Default | Leopard | Effect |
| --- | --- | --- | --- | --- |
| `Engine` | `Inertia` | — (>0) | 3 | Engine inertia. It must be greater than zero |
| `Engine` | `InertiaCoupled` | — | not used | Extra inertia with the clutch engaged |
| `Engine` | `MaxPower` | 100 kW | 1100 | Maximum power |
| `Engine` | `MaxTorque` | 186 N·m | 4700 | Maximum torque |
| `Engine` | `RpmMaxPower` | 5800 | 2600 | RPM at maximum power |
| `Engine` | `RpmMaxTorque` | 4400 | 1600 | RPM at maximum torque |
| `Engine` | `Steepness` | — (>0) | 50 | Shape of the torque curve. Vanilla engines use 4 to 50 |
| `Engine` | `Friction` | — (>0) | 470 | Internal friction. **1400 ate a third of the torque** |
| `Engine` | `BrakingTorqueRatio` | 0.15 | 0.15 | Engine braking, as a fraction of `MaxTorque` |
| `Engine` | `RpmIdle` / `RpmRedline` / `RpmMax` | 1250 / 6250 / 7000 | 700 / 2700 / 3000 | Idle, cutoff and limit. The order must be ascending |
| `Engine` | `HasGovernor` | — | 1 | RPM limiter |
| `Clutch` | `MaxClutchTorque` | 225 | 9000 | **Ignored** by the tracked simulation: the clutch is just a 0–1 multiplier |
| `Gearbox` | `Forward { ... }` | — | 6.0 3.3 1.8 1.0 | Forward gear ratios |
| `Gearbox` | `Reverse` | — | 2.3 | **One** reverse ratio. The sign is forced to negative (see [bugs](09-bugs.md)) |
| `Gearbox` | `Efficiency` | — | 0.9 | Transmission efficiency |
| `TripleDifferential` | `Ratio` | — (>0) | 4.9 | Final drive ratio |
| `TripleDifferential` | `SteeringRatio` | — | 6 | Maximum ratio when turning on the move. **Without it, the tank did not turn** |
| `TripleDifferential` | `SteeringThreshold` | 0.01 | inherited | Minimum input to turn |
| `TripleDifferential` | `NeutralSteeringRatio` | — | 12 | Pivot turn ratio |

The `Output` attribute of the blocks only links the engine → clutch → gearbox chain. Do not set `Output` on the gearbox; vanilla does not set it either.

### Wheels and suspension

| Block | Attribute | Default | Leopard | Effect |
| --- | --- | --- | --- | --- |
| `RoadWheel` / `Sprocket` / `Idler` / `Roller` | `Mass` | 12 kg | 110 / 250 / 150 / — | Wheel mass |
| all | `Radius` | 0.6 m | 0.438 / 0.355 / 0.265 / — | The drive sprocket radius sets the track speed |
| all | `BrakeTorque` | 4000 N·m | 6000 / 30000 / 0 / — | Brake torque per wheel |
| `Suspension` (`RoadWheel` only) | `MaxSteeringAngle` | — | 0 | Always 0 on a tracked vehicle |
| `Suspension` | `SpringRate` | 20 | 450 | Spring stiffness (likely unit: N/mm) |
| `Suspension` | `CompressionDamper` | 2500 | 25000 | Compression damping |
| `Suspension` | `RelaxationDamper` | — | 35000 | Rebound damping |
| `Suspension` | `MaxTravelUp` / `MaxTravelDown` | 0.15 / — | 0.30 / 0.15 | Upward and downward travel, in meters |
| `Suspension` | `RayStartOffsetUp` | — | 0.5 | How far above the bone the ray starts |
| `TrackPart` | `Type` | Road Wheel | — | `Road Wheel`, `Sprocket`, `Idler`, `Return Roller` |
| `TrackPart` → `Pivot` | `Bone`, `Position`, `Rotation` | — | `Bone "v_..."` | Wheel bone and optional offset |

### Aerodynamics

| Attribute | Default | Leopard | Effect |
| --- | --- | --- | --- |
| `ReferenceArea` | 3 (vanilla) | 9.5 | Frontal area in m² |
| `DragCoefficient` | 0.5 | 0.9 | Drag coefficient |

### SCR\_TrackedControllerComponent

It sits inside `BaseVehicleNodeComponent`. The defaults come from the executable. The *Vanilla* column shows what `TrackedVehicle_Base.et` changes.

| Attribute | Default | Vanilla | Effect |
| --- | --- | --- | --- |
| `Type` | 0 | — | The code never reads it |
| `TransmissionRND` | false | — | Three-position gearbox (R, N, D). It neither unlocks nor blocks reverse |
| `SteeringForwardSpeed` / `SteeringCenterSpeed` | — | `19 1.1 100 0.02` / `19 1.1 60 1.8` | Steering speed when turning and when centering. Format not decoded |
| `ThrottleTurbo` | 0.2 | — | Share of the throttle reserved for turbo. W alone gives 80% |
| `ThrottleTurboTime` | 0.15 s | — | Time for the turbo to kick in |
| `ThrottleReverseTarget` | 0.2 | — | Throttle used in reverse. **Use 1** (see [bugs](09-bugs.md)) |
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

### VehicleTrackedSimulation component (outside the Simulation)

| Attribute | Effect |
| --- | --- |
| `TrackSegments`, `TrackSegment`, `TrackPositions`, `TrackLength`, `TrackThickness`, `TrackOffset1`, `TrackOffset2` | Visual link chain. The default asset (M113 link) does not ship with the game |
| *Generate track positions* button | Generates the chain positions in Workbench |

Leave these empty for the first test. The chain is only created with `TrackLength > 0` and at least 2 `TrackPositions`.

### Available signals

| Signal | Exists? | Value |
| --- | --- | --- |
| `engineRPM` | yes | Engine RPM |
| `engineThrust` | yes | Applied throttle (0.8 with W, 1.0 with turbo) |
| `engineOn` | yes | Engine running |
| `brake` | yes | Brake. It reads 6 when the controller applies the parking brake |
| `gear`, `throttle`, `clutch`, `handBrake` | **no** | — |

---

[← Converting an existing vehicle](06-converting.md) · [Contents](../../README.md) · [How it works inside →](08-internals.md)
