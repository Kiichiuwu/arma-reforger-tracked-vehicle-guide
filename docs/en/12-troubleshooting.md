> [🇧🇷 Português](../pt-BR/12-troubleshooting.md) · 🇬🇧 English

[← Ready-made scripts](11-scripts.md) · [Contents](../../README.md) · [Suspension and ride height →](13-suspension.md)

# Troubleshooting

Find the message or symptom in the right table. Messages appear in the Workbench *Log Console* and in the session `console.log` (`Documents\My Games\ArmaReforgerWorkbench\logs\logs_<date>\`). Lines that start with `LEOREV`, `LEOTEL`, `LEOSUSP`, `LEOGEAR` or `LEOTRACK` come from the [ready-made scripts](11-scripts.md).

The rows come from the Leopard work in Workbench 1.8.0.13: the earlier 2A7 build and the current 2A6. Causes marked inferred, reverse-engineered or open were not confirmed in game.

### Errors when loading the vehicle

| Message | Cause | Fix |
| --- | --- | --- |
| `Tracked - failed to load!` | One of the errors below | Read the line just above it in the log |
| `FrictionCurveLateral - configuration is missing!` | The vanilla `.ct` has no curves | Add `FrictionCurveLongitudinal` and `FrictionCurveLateral` |
| `Invalid number of tracks! Must be 2!` | `Tracks` is missing or has more than two | Exactly `Track Left` and `Track Right` |
| `Wheels - configuration is missing! Every track should have at least 3 wheels!` | Fewer than 3 parts on a track | At least 3 `TrackPart` per track |
| `Wheels - ... mismatch between tracks` | The idler or the sprocket is not at the same `TrackPart` index on both tracks (reverse-engineered) | Use the same `TrackPart` order on both sides |
| `TrackWheel's mass / radius / brake torque is incorrect!` | A wheel template is missing, or a value is out of range: mass and radius must be greater than zero, brake torque zero or more | Define `RoadWheel`, `Sprocket` and `Idler`, plus `Roller` if a track has `Type "Return Roller"` parts |
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
| Top speed far off from the calculation | Calculation used the road wheel radius | Redo it with the drive sprocket radius. On the 2A6, speed ÷ sprocket angular speed gave 0.344 m against `Sprocket.Radius` 0.34 (measured) |
| Very slow acceleration | High `Friction`, high `Inertia` or a long first gear | `Friction` \~10% of the torque; shorten 1st gear |
| W never reaches 100% | `ThrottleTurbo` 0.2 | Normal; use turbo |
| S at a standstill: the engine revs above `RpmMax` and nothing happens | Native reverse bug (on the 2A6 the engine sticks at 3766–3768 rpm) | [`LEO_TrackedDriveComponent`](11-scripts.md) |
| S at a standstill: the tank moves forward | `ThrottleReverseTarget` below 1 | `ThrottleReverseTarget 1` |
| Downhill it holds reverse speed, but on flat ground it does not move | Reverse bug: only gravity moves it | Reverse script |
| The engine stays at about 4500 rpm after the driver gets out | Without a driver the controller pulls the handbrake and, at a standstill, selects reverse; the reverse bug then pins the engine (observed on the 2A6) | `m_bStopEngineOnExit 1` in the reverse script (v4) |
| In a pivot turn the engine drops to `RpmIdle` (700 rpm) | With the clutch engaged the controller holds the brake at 6.0 while pivoting (measured). Why it does so is open | No fix needed: the hull still turns. With the clutch at 0, the engine sits at the governor (2700 rpm) instead (observed in the video) |
| The hull drifts backwards in a pivot turn | The track that runs backwards turns faster than the other one (measured −0.1 to −3.8 km/h of drift). Cause open | Open |
| Full throttle, the engine sits at 700 rpm and the tank barely moves | The tank is blocked or high-centred on a rock, with only 1–5 road wheels per side touching the ground (measured) | Reverse out. Expected behaviour |
| The tank flips easily at speed | Possibly a high centre of mass: `CenterOfMass` is an offset added to the model's own COM point. On the 2A6, `0 1 0.46` puts the COM 2.17 m above the ground, for a static stability factor of about 0.61 g (computed, not proven) | Lower the Y offset (Y 0 would give about 1.14 g). Not tested yet ([chapter 13](13-suspension.md)) |
| Stopped tank slowly slides down a slope | Not solved. One logged case: 0.3–0.4 km/h for 2.6 s on a \~6° slope with the brake at 6.0 | Open |
| On a dedicated server, steering from standstill does nothing; the engine revs and the hull rubberbands | The network move drops the handbrake (`IsHandbrakeOn` is a stub), so the server never pivots (measured) | `LEO_TrackedDriveComponent` (the server applies `SetBreak(x, true)` itself), or Kiichi Tracked System ([chapter 18](18-multiplayer.md), [chapter 19](19-kiichi-tracked-system.md)) |
| Reverse rubberbands on a server | The scripted push ran only on the driver's machine | v5 ([chapter 18](18-multiplayer.md)) |
| As the driver on a client: head frozen, sluggish look at standstill, camera jumps when accelerating and turning, keys stick | Engine limitation of the tracked owner-client path. Not caused by the mod scripts (measured with everything removed) | None. Singleplayer, listen host and the other seats work ([chapter 18](18-multiplayer.md)) |

### Suspension and ride height

| Symptom | Cause | Fix |
| --- | --- | --- |
| The hull rides high at rest; the wheels hang below their modelled position | `MaxTravelDown` is larger than the static sag m·g/(n·k). The spring is free (zero force) at full droop | Set `MaxTravelDown` ≈ m·g/(n·k). 2A6: 62 300 · 9.81 / (14 · 450 000) = 0.097 m. With 0.15 the hull rode 53 mm high, with 0.10 it sits 3 mm low (measured) |
| The hull sits low or bottoms out | `MaxTravelDown` is smaller than the sag, or `SpringRate` is too soft. `SpringRate` is in kN/m (= N/mm) | The same rule. Recompute `SpringRate` in kN/m |
| The hull floats or sinks by a constant amount | `RoadWheel.Radius` is not the distance from the wheel centre to the track's ground face | Radius = wheel centre height above the ground (2A6: 0.418 m for a 0.345 m rim) |
| Nose up or nose down at rest | The centre of mass is not over the middle of the road wheels. `CenterOfMass` is an offset added to the model's own COM point (a `COM_` empty), not an absolute position | Move the z offset: forward lowers the nose. 2A6: z 0 gave 0.954° nose up and z 0.46 gave 0.049° nose down (measured): about 2.1° per metre (2.06°/m computed, 2.18°/m measured) |
| A change to the suspension values has no effect | The world was open when you changed the prefab | See [Workbench workflow](#workbench-workflow) |
| The wheels hang down when the tank is in the air or upside down | No ground contact means full droop | Expected |
| Suspension travel above `MaxTravelUp` in telemetry (up to +0.38 m) | The ray starts `RayStartOffsetUp` above the top of the travel, so the contact can lie above the bump stop. The engine's own signal clamps at `MaxTravelUp` | Expected. Clamp it in your scripts |

[Chapter 13](13-suspension.md) explains the spring model and the measurements.

### Running gear and track links

| Symptom | Cause | Fix |
| --- | --- | --- |
| Only the front left road wheel follows the ground; the other bones move with another wheel or not at all | `CarProcAnimComponent` with `ProcAnimParams` on a tracked vehicle. In 1.8.0.13 it gave every bone except `v_wheel_l01` the signals of another wheel, with both param orders tried (measured) | Leave `CarProcAnimComponent` empty and use [`LEO_RunningGearComponent`](11-scripts.md) |
| Road wheels float above the chain: right wheels 2–7 about 5 cm high at rest ([picture](../images/before-wheels-floating.jpg)) | The same mis-mapping. Those bones most likely read an idler, sprocket or roller signal, which has no suspension, so they stay at the modelled height while the simulated wheels sit lower (inferred) | The same fix |
| Some wheels spin with the other track | The same mis-mapping (left even wheels spun with the right track, measured) | The same fix |
| Scripted running gear: the wheels wobble or turn about the wrong axis | The bone's rest orientation is not the identity. The script rotates about the bone's X axis in hull axes | Rig the bones at the part centre with identity orientation ([chapter 14](14-running-gear.md)) |
| Scripted running gear: the wheels turn too fast or too slow against the links | `m_fRoadWheelRadius` or `m_fRollerRadius` is not the visual rim radius | Measure the rims in the model (2A6: 0.345 m and 0.0847 m) |
| Wheels and links spin wrongly while the front road wheel is in the air in reverse | The script takes the track speed from road wheel 0, whose RPM differs without contact (inferred from logged RPM values) | Open |
| The suspension arms stay still while the wheel moves | The arms are weighted to `v_body` | Re-weight them to `v_suspension_<s>0<k>`, from 0 at the hull pivot to 1 at the hub ([chapter 14](14-running-gear.md)) |
| The native links hang below the road wheels or behind the sprocket ([at rest](../images/before-rope-sag.jpg), [moving](../images/before-rope-sag-sprocket.jpg)) | The native rope has gravity but no ground collision. Its slack (`TrackLength` too long for the path) hangs in the gaps | Use the scripted chain. Less slack hides the sag but derails at full droop ([chapter 15](15-track-links.md)) |
| The native chain comes off the sprocket after a flip or a mine blast and stays off ([flip](../images/before-rope-off-flip.jpg), [mines](../images/before-rope-off-mines.jpg), [bare sprocket](../images/before-bare-sprocket.jpg)) | The wheels lose contact and drop to full droop, which needs more chain than there is. The rope is dragged across the sprocket and settles outside it. There is no script API to reset it | Use the scripted chain. The Diag exe has a *Reset tracks* entry, not tried |
| The native links run slower or faster than the ground | `TrackPart` 0 and 1 are not road wheels: the chain speed uses the radius of part 0 and the angle of part 1 (reverse-engineered; with the idler first it would run at about 63%) | Put the road wheels first |
| The native links sit beside the wheels | The x of `TrackOffset1` / `TrackOffset2` is not the belt centre. `TrackOffset1` is used for the first `Track` | Set x to the belt centre (2A6: −1.3281 and 1.3281) |
| The links are upside down or offset from the wheels | The link model's origin or axes are wrong | Author the link as a bottom-run link: origin at the centre of its ground face, +Y towards the wheels, +Z forward ([chapter 15](15-track-links.md)) |
| No links appear, the static belts stay, and `LEOTRACK no link chain after 5 s: static belts kept` | Native chain: fewer than 2 `TrackPositions` entries, `TrackLength` 0 or no `TrackSegment` model. Scripted chain: a `LEOGEAR` warning above it | Fix the chain attributes or the `LEOGEAR` cause |
| Two belts of links on top of each other | `m_sNativeLinkName` does not match the `TrackSegment` model's file name, so the native links are not hidden | Set `m_sNativeLinkName` to part of that file name |
| The static belt stays visible over the moving links | `LEO_TrackBeltComponent` is missing, or its mesh names do not match (`LEOTRACK static belt mesh not found: <name>`) | Add it and set `m_sStaticBeltMeshes` and `m_sLinkModelName` |
| Wheels and links stop moving far from the camera | `LEO_RunningGearComponent` skips the update beyond `m_fMaxDistance` (300 m) | By design. Raise the value if needed |

### Scripts, diag and crashes

| Symptom | Cause | Fix |
| --- | --- | --- |
| Workbench closes when the engine starts | A script called `GetInputs()` | Remove the call. `GetFloorNormalAngles()` has the same defect |
| Workbench closes when the vehicle debug opens (*Show vehicle debug*, *Show Controller Diags*) | No link chain is configured. The diag draw reads the rope pointers without a null check (crash of 2026-10-02, reverse-engineered) | Configure the native chain (2 or more `TrackPositions`, `TrackLength` above 0). With the chain, both options ran without a crash on 2026-10-06. Save your work first |
| `Pointer type Physics can only be used with local variables` | `Physics` as a class member | Use a local variable in each function |
| `GetThrottle()` always 0 | Empty function on the tracked simulation | Read the `engineThrust` signal |
| `GetBrake()` returns 6.0 | Handbrake: `SetBreak(x, true)` stores 6.0. The controller holds it at a standstill and in pivot turns | Expected |
| The wheel getters return 0, false or a zero vector; `TrackCount()` is 0 | The simulation is not loaded yet (valid about 1 s after the vehicle spawns) | Guard with `IsValid()` and `TrackCount() < 2` ([Ready-made scripts](11-scripts.md)) |
| `WheelGetPosition` returns y ≈ −1.9 for every wheel | The vector is relative to the physics centre of mass, and displacement 0 means full droop | Add the effective COM to get hull coordinates |
| `LEOTEL` and `LEOSUSP` print bit-identical lines | The body is asleep (sleeping thresholds) | Expected |
| `LEOREV no pilot compartment found` | Compartments registered after `OnPostInit` | Get the pilot slot on demand (already done in v4) |
| `LEOREV native gearbox still in forward gear` | The controller has not shifted to reverse yet | Normal for up to 0.5 s |
| `LEOREV native brake applied: 6.00` | The handbrake is on | Release it. The push waits until the brake signal is below 0.05 |
| `LEOREV still moving forward: ... km/h` | The hull moves forward faster than `m_fForwardCutoffKmh`; the native simulation brakes first | Normal |
| `LEOREV no controller / engine off` | The engine is off | Start the engine |
| `LEOGEAR <n> running gear bones missing or unreadable: the scripted chain is off` | A bone name differs, or a wheel bone's rest height is outside 0.5–2 × `m_fRestHeight` | Check the bone names ([Ready-made scripts](11-scripts.md)) and `m_fRestHeight` |
| `LEOGEAR the simulation has <n> wheels, expected ...` | The track has fewer `TrackPart`s than `m_iRoadWheels` + 2 + `m_iRollers` | Match the attributes to the prefab |
| `LEOGEAR link model not found: ... - the scripted chain is off` | `m_sLinkModel` points to a missing `.xob` | Set your link model |
| `LEOGEAR no TrackedControllerComponent or no skeleton: running gear not animated` | The component is on a prefab without a tracked controller or without bones | Put it on the tracked hull |

### Workbench workflow

| Symptom | Cause | Fix |
| --- | --- | --- |
| A prefab change has no effect in *Play* | A vehicle in a world that is already open keeps the simulation built from the old prefab. Measured: the prefab said `MaxTravelDown` 0.10, the vehicle still behaved as 0.15 | Close the world, reload the scripts with no world open, open the world again |
| `Resources are leaking! Check log!` assert (Diag exe, `GameApp.cpp` line 1287) | Scripts reloaded with a world open. The 77 leaked resources are vanilla ones, none from the mod | Close the world first, or click *Ignore* |
| Workbench freezes after you replace the hull `.fbx` | Hot reimport with the world open (it froze once, one CPU core at 100%) | Close the World Editor before you replace models. If it freezes, end the process and start again |
| Workbench closes on recompile | Recompiled with *Play* running | Stop *Play* first |

### Model and import

| Symptom | Cause | Fix |
| --- | --- | --- |
| Bones do not show in the `.xob` | Imported without a skeleton | *Export Skinning* and *Export Scene Hierarchy* on, *Merge Meshes* off |
| Duplicate `Scene_Root` | The Blender root bone has that name | Rename it to `v_body` |
| Material error on the colliders | The default `material/metal.gamemat` does not exist | Use `armor_XXmm.gamemat` |

---

[← Ready-made scripts](11-scripts.md) · [Contents](../../README.md) · [Suspension and ride height →](13-suspension.md)
