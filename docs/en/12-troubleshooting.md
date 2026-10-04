> [🇧🇷 Português](../pt-BR/12-troubleshooting.md) · 🇬🇧 English

[← Ready-made scripts](11-scripts.md) · [Contents](../../README.md) · [Appendix →](13-appendix.md)

# Troubleshooting

Find the message or symptom in the right table. Messages appear in the Workbench *Log Console* and in the session `console.log`.

### Errors when loading the vehicle

| Message | Cause | Fix |
| --- | --- | --- |
| `Tracked - failed to load!` | One of the errors below | Read the line just above it in the log |
| `FrictionCurveLateral - configuration is missing!` | The vanilla `.ct` has no curves | Add `FrictionCurveLongitudinal` and `FrictionCurveLateral` |
| `Invalid number of tracks! Must be 2!` | `Tracks` is missing or has more than two | Exactly `Track Left` and `Track Right` |
| `Wheels - configuration is missing! Every track should have at least 3 wheels!` | Fewer than 3 parts on a track | At least 3 `TrackPart` per track |
| `Wheels - ... mismatch between tracks` | Different count of some part type between the sides | Same number of idlers, road wheels and drive sprockets on both sides |
| `TrackWheel's mass / radius / brake torque is incorrect!` | Wheel template missing or with a value out of range | Define `RoadWheel`, `Sprocket` and `Idler` with positive mass and radius |
| `Engine's inertia is incorrect! (must be greater than zero)` | `Inertia` 0 | `Inertia` greater than zero. The same applies to `Steepness` and `Friction` |
| `Tracked - no physics component!` | No colliders or no `RigidBody` | `UCX_*` colliders in the `.xob` |
| `Aerodynamics - configuration is missing!` | `Aerodynamics` block deleted | Keep the inherited one or write a new one |
| Warning: `CarControllerComponent` missing | `AICarMovementComponent` from the vanilla base | Harmless |
| Sound warning asking for the `v_axle_01` bone | Sound component inherited from the car | Harmless; or create the bone |

### Driving problems

| Symptom | Cause | Fix |
| --- | --- | --- |
| W + A/D only slows down and does not turn | `SteeringRatio` has no value | `SteeringRatio 6` |
| A/D at a standstill does not turn in place | `NeutralSteeringRatio` and `PivotSteering` are missing | `NeutralSteeringRatio 12`, `PivotSteering 1`, `NeutralSteering 1` |
| Speed caps at \~51–54 km/h | Default retarder | `RetarderBaseTorque 0`, `RetarderThrottleSpeed 20` |
| Top speed far off from the calculation | Calculation used the road wheel radius | Redo it with the drive sprocket radius |
| Very slow acceleration | High `Friction`, high `Inertia` or a long first gear | `Friction` \~10% of the torque; shorten 1st gear |
| W never reaches 100% | `ThrottleTurbo` 0.2 | Normal; use turbo |
| S at a standstill: the engine revs above `RpmMax` and nothing happens | Native reverse bug | Reverse script |
| S at a standstill: the tank moves forward | `ThrottleReverseTarget` below 1 | `ThrottleReverseTarget 1` |
| Downhill it holds reverse speed, but on flat ground it does not move | Reverse bug: only gravity moves it | Reverse script |
| Hull sinks or floats | `RoadWheel.Radius` does not match the bone height, or the spring is weak | Radius = bone height above the ground; recalculate `SpringRate` |
| Stopped tank slowly slides down a slope | Not solved | Open |

### Scripts and crashes

| Symptom | Cause | Fix |
| --- | --- | --- |
| Workbench closes when the engine starts | A script called `GetInputs()` | Remove the call |
| Workbench closes when a vehicle diag opens | *Show vehicle debug* or *Show Controller Diags* | Do not use these diags |
| Workbench closes on recompile | Recompiled with *Play* running | Stop *Play* first |
| `Pointer type Physics can only be used with local variables` | `Physics` as a class member | Use a local variable in each function |
| `LEOREV no pilot compartment found` | Compartments registered after `OnPostInit` | Get the pilot slot on demand (already done in v4) |
| `LEOREV native gearbox still in forward gear` | The controller has not shifted to reverse yet | Normal for up to 0.5 s |
| `GetThrottle()` always 0 | Empty function on the tracked simulation | Read the `engineThrust` signal |

### Model and import

| Symptom | Cause | Fix |
| --- | --- | --- |
| *Missing Addon* window (`Game addon '58D0FB3206B6F859' not found`) | Workbench opened without the addon folders | Open it with `-addonsDir` |
| Bones do not show in the `.xob` | Imported without a skeleton | *Export Skinning* and *Export Scene Hierarchy* on, *Merge Meshes* off |
| Duplicate `Scene_Root` | The Blender root bone has that name | Rename it to `v_body` |
| Material error on the colliders | The default `material/metal.gamemat` does not exist | Use `armor_XXmm.gamemat` |
| Turret rotates around the rear | `v_turret_slot` is off the ring center | Bone at the ring center; turret `.xob` origin at the same point |
| Gunner shows as *obstructed* | Turret has no `DoorInfoList` | Doors with teleport in the turret compartment |

---

[← Ready-made scripts](11-scripts.md) · [Contents](../../README.md) · [Appendix →](13-appendix.md)
