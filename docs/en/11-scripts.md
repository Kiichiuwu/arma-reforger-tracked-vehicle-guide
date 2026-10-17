> [🇧🇷 Português](../pt-BR/11-scripts.md) · 🇬🇧 English

[← Tuning with real-world data](10-tuning.md) · [Contents](../../README.md) · [Troubleshooting →](12-troubleshooting.md)

# Ready-made scripts

The Leopard 2A6 uses five script files and one offline tool for its tracked vehicle. They use only the vanilla API of Workbench 1.8.0.13. The code is in [`scripts/`](../../scripts) and [`tools/`](../../tools), under the MIT license.

| File | Type | What it does | When you need it |
| --- | --- | --- | --- |
| [`LEO_TrackedReverseComponent.c`](../../scripts/LEO_TrackedReverseComponent.c) | Component | Pushes the hull backwards while the driver holds reverse. Stops the engine when the driver gets out | Always: the native reverse cannot drive ([why](09-bugs.md)) |
| [`LEO_RunningGearComponent.c`](../../scripts/LEO_RunningGearComponent.c) | Component | Moves the road wheel bones with the suspension, spins every wheel, runs the scripted link chain and hides the native rope's links | When the wheels and links must move ([chapter 14](14-running-gear.md)) |
| [`LEO_TrackChain.c`](../../scripts/LEO_TrackChain.c) | Plain class | One track's scripted link chain: the belt path and the link entities | Always together with `LEO_RunningGearComponent` ([chapter 15](15-track-links.md)) |
| [`LEO_TrackBeltComponent.c`](../../scripts/LEO_TrackBeltComponent.c) | Component | Hides the hull's static belt meshes once link entities exist | When the model keeps a static belt as a fallback |
| [`LEO_TelemetryComponent.c`](../../scripts/LEO_TelemetryComponent.c) | Component | Writes `LEOTEL` and `LEOSUSP` lines to the log | For tuning only. Remove it for a release |
| [`tools/trackpositions.py`](../../tools/trackpositions.py) | Python 3 script | Computes the `TrackPositions` list offline | When you configure the native link chain |

The full code is in the files. This chapter explains what to install, which attributes to set and how the parts work together. [Chapter 14](14-running-gear.md) and [chapter 15](15-track-links.md) explain the maths behind the running gear and the chain.

### How the components work together

```mermaid
flowchart LR
    SIM["VehicleTrackedSimulation<br/>(physics + native rope)"]
    CTRL["TrackedControllerComponent<br/>GetSimulation()"]
    GEAR["LEO_RunningGearComponent"]
    CHAIN["LEO_TrackChain x 2<br/>73 link entities each"]
    NATIVE["Native rope links<br/>146 entities"]
    BONES["Wheel bones<br/>SetBoneMatrix"]
    BELT["LEO_TrackBeltComponent"]
    STATIC["Static belts<br/>track_l, track_r"]
    REV["LEO_TrackedReverseComponent"]
    BODY["RigidBody"]
    TEL["LEO_TelemetryComponent"]
    LOG["console.log"]
    SIM --> CTRL
    SIM --> NATIVE
    CTRL --> GEAR
    CTRL --> REV
    CTRL --> TEL
    GEAR --> BONES
    GEAR --> CHAIN
    GEAR -.->|hides| NATIVE
    CHAIN -.->|counted by| BELT
    NATIVE -.->|counted by| BELT
    BELT -->|hides| STATIC
    REV -->|ApplyForce, ApplyTorque| BODY
    REV -.->|StopEngine when the driver leaves| CTRL
    TEL --> LOG
```

What happens in *Play*, in order:

1. **`OnPostInit`.** `LEO_RunningGearComponent` reads the rest position of every running-gear bone (the bind pose) and checks that the bones exist. The other components find the controller, the pilot slot and the signals.
2. **The simulation becomes valid** 0.78–1.26 s after the vehicle entity is created, 8–13 s before the engine was started in the logged sessions (measured). On that first frame, `LEO_RunningGearComponent` logs `LEOGEAR simulation: 2 tracks, WheelCount 13` and spawns 2 × 73 link entities. The engine's rope has already spawned its own 146 links.
3. **Same frame.** `LEO_TrackBeltComponent` counts 292 link children and hides the static belts (`LEOTRACK link chain found (292 links)`).
4. **Every frame.** `LEO_RunningGearComponent` reads each wheel from the simulation, writes the bone matrices, rebuilds both belt paths and places the links. It hides the native links 4 times a second for the first 10 s, then every 2 s.
5. **Every physics step.** `LEO_TrackedReverseComponent` checks the reverse conditions and pushes the hull. Every frame, it also checks whether the driver seat has become empty.
6. **Every 0.5 s.** `LEO_TelemetryComponent` writes its lines.

The components that read the simulation get it through `TrackedControllerComponent.GetSimulation()`. None of them writes to it, except the optional `SetGear` of `m_bHoldNeutral`. `LEO_TrackBeltComponent` does not use the simulation at all.

### How to install

1. Copy the five `.c` files into `scripts/Game/Vehicle/` of your mod.
2. Recompile the scripts with *Play* **stopped**. In the Diag exe, reload them with **no world open**; otherwise you get a `Resources are leaking` assert ([Troubleshooting](12-troubleshooting.md)).
3. Add the components to the `components { }` block of the hull prefab, with the empty `CarProcAnimComponent` override:

```
  CarProcAnimComponent "{5D6501223785D009}" {
  }
  LEO_RunningGearComponent "{5292A83C2D6FC873}" {
  }
  LEO_TrackedReverseComponent "{7A1D4C9E2B6F3085}" {
  }
  LEO_TelemetryComponent "{3C5E7A9B1D2F4608}" {
  }
  LEO_TrackBeltComponent "{8E3A61C04F2D9B57}" {
  }
```

4. Set the attributes for your vehicle (tables below). The defaults are the Leopard 2A6 values; that is why the Leopard's override blocks are empty.
5. Close the world and open it again. A vehicle in a world that is already open keeps the simulation built from the old prefab (measured).

Notes on the snippet:

- `{5D6501223785D009}` is the `CarProcAnimComponent` that `TrackedVehicle_Base.et` already has, empty. Keep it **without** `Parameters`. On a tracked vehicle it gives most wheel bones the signals of another wheel ([chapter 14](14-running-gear.md)). If your prefab added `ProcAnimParams` to it, delete them.
- The four `LEO_` IDs are the Leopard's. Any unique 16-digit hexadecimal ID works; Workbench creates one when you add a component in the editor.
- The complete Leopard block, with `RigidBody`, is in [`examples/leopard2a6/HullComponents_Leopard2A6.et.txt`](../../examples/leopard2a6/HullComponents_Leopard2A6.et.txt). `ThrottleReverseTarget 1` goes on the controller ([Appendix](17-appendix.md)).
- `LEO_TrackChain.c` holds no component. `LEO_RunningGearComponent` creates one chain per track.
- If you use the scripts in another mod, rename the `LEO_` prefix so the classes do not collide with the Leopard mod. The `category: "Leopard2A7"` in `ComponentEditorProps` only groups the components in the editor's list.

### LEO\_TrackedReverseComponent (scripted reverse, v5)

The native reverse pins the engine above `RpmMax`, so no torque reaches the tracks ([Bugs and limitations](09-bugs.md)). This component applies a backward force to the hull while all of these hold:

- the local driver holds `TrackedBrake` and not `TrackedThrottle`;
- the engine is running;
- the hull is stopped or already moving backwards (forward speed below `m_fForwardCutoffKmh`);
- the native gear is reverse or neutral (index 0 or 1);
- the native brake signal (`GetBrake()`) is below 0.05.

The force is `m_fLaunchForceG` times the weight at a standstill. It fades linearly to zero at `m_fMaxReverseSpeedKmh`. The steering keys add a yaw torque, limited to `m_fMaxYawRate`.

The component also stops the engine when the driver seat becomes empty (`m_bStopEngineOnExit`). Without a driver, the controller pulls the handbrake; at a standstill it then selects reverse, and the reverse bug pinned the engine at about 4500 rpm (observed on the 2A6, 2026-10-04). The check runs in `EOnFrame`, because the physics body can go to sleep once nobody drives. The log shows `LEOREV driver left: engine stopped` when it acts.

| Attribute | Default | Purpose |
| --- | --- | --- |
| `m_fLaunchForceG` | 0.3 | Force at a standstill, as a fraction of the weight |
| `m_fMaxReverseSpeedKmh` | 31 | Top reverse speed in km/h. The force drops to zero there |
| `m_fForwardCutoffKmh` | 1.5 | Above this forward speed in km/h, the script does not push |
| `m_fSteerTorque` | 900000 N·m | Turning torque in reverse at full steering |
| `m_fMaxYawRate` | 0.45 rad/s | Maximum turn rate in reverse |
| `m_bHoldNeutral` | 0 | Forces the native gearbox from R to N while reversing. The controller shifts back to R, so it is off |
| `m_bDebug` | 1 | Writes `LEOREV` lines to the log while the reverse key is held |
| `m_bStopEngineOnExit` | 1 | Stops the engine when the driver seat becomes empty. Without it, the parked tank selects reverse and the engine stays pinned above `RpmMax` |

Keep `m_fMaxReverseSpeedKmh` below the native track-speed cap in reverse:

```math
v_{\text{max,rev}}\,[\text{km/h}] = \frac{\text{RpmMax}}{|\text{Reverse}| \cdot \text{Ratio}} \cdot \frac{2\pi}{60} \cdot r_{\text{sprocket}} \cdot 3.6
```

On the 2A6: 3000 / (2.3 · 4.6) · 0.10472 · 0.34 · 3.6 ≈ 36.3 km/h. The script stops pushing at 31 km/h.

`m_fSteerTorque` 900 000 N·m is the value on the 62.3 t 2A6. The earlier 64.5 t 2A7 build used the same value. Scale it with the mass.

**Measured on the 2A6** (LEOTEL, 2026-10-05 23:19, full reverse input): 0 to −20.9 km/h in about 3 s, and −27.8 km/h after about 7.6 s. The native engine still settled at 3767 rpm in R during the push. The formula in [chapter 9](09-bugs.md) predicts 3766.1 rpm for 62 300 kg, `Reverse` 2.3, `Ratio` 4.6 and `Inertia` 3.

**Multiplayer (v5).** The driver's client sends `{reverse, steer, handbrake}` to the server by RPC; the server applies the same push, restores the handbrake that the network move drops (`m_bMirrorHandbrake`), relaxes the reconciliation allowances while pivoting or reversing (`m_bRelaxAllowance`) and stops the engine on the authority only. `m_bClientPrediction 0` switches the client's re-simulation off (an experiment switch). Verified in the Workbench Peer Tool; not yet under real latency. The driver's camera and input problem on clients is an engine limitation that this component cannot fix. See [Multiplayer](18-multiplayer.md).

The `LEOREV` messages are listed in [Troubleshooting](12-troubleshooting.md).

### LEO\_TelemetryComponent (telemetry for tuning)

It writes log lines that you can grep from the session `console.log` (`Documents\My Games\ArmaReforgerWorkbench\logs\logs_<date>\console.log`). [Chapter 16](16-debugging.md) shows how to read them next to the native vehicle debug.

| Line | When | Content |
| --- | --- | --- |
| `LEOTEL init` | Once, at start | Whether the controller and the signals manager were found |
| `LEOTEL pose` | Every `m_fInterval`, engine on or off | Hull `pitch` and `roll` in degrees (pitch > 0 = nose up) |
| `LEOTEL v=` | Every `m_fInterval` with the engine on | `v`: forward speed in km/h (negative = backwards). `rpm`, `thrust`: the `engineRPM` and `engineThrust` signals. `gear`: 0 = R, 1 = N, 2.. = forward gears. `clutch`. `simBrk`: 6.00 = handbrake. `simRpm`: `EngineGetRPM()`. `wheelRpm`: `EngineGetRPMFeedback()`, the rpm fed back from the gearbox side |
| `LEOSUSP track=l` / `track=r` | Every 2 s with the engine on | `worst_mm`. `diff_mm` per road wheel: bone minus simulation wheel centre (`air` without ground contact). `travel_mm` per road wheel: simulation travel from the modelled height, + = up. `spin`: angle of each road wheel and of the sprocket about the axle, in degrees |

| Attribute | Default | Purpose |
| --- | --- | --- |
| `m_fInterval` | 0.5 s | Time between lines |
| `m_bReadSimulation` | 1 | Adds gear, clutch, brake and engine rpm from the simulation |
| `m_fWheelRestHeight` | 0.4186 m | Modelled road wheel centre height (bone `v_wheel_l01` at rest), for `travel_mm` |

What healthy values look like on the 2A6 (measured, sessions of 2026-10-05 and 2026-10-06):

- `diff_mm` has a median of 0 with `LEO_RunningGearComponent`, and 90–95% of samples are within 3 mm. Larger values appear only while a wheel eases back after it lost contact.
- The 7 `spin` values of a track are equal.
- At rest on flat ground, `travel_mm` reads −1 to −5 mm from front to rear (`MaxTravelDown` 0.10, `CenterOfMass 0 1 0.46`).
- At rest on flat ground, `LEOTEL pose` reads a pitch of about −0.05°.
- Several bit-identical lines in a row mean the body is asleep (`LinearSleepingThreashold` / `AngularSleepingThreashold`), not that the logger is broken.

`LEOSUSP` assumes 7 road wheels named `v_wheel_<s>0<k>` and a bone `v_sprocket_<s>`. Edit the loop for another layout. It reads the wheel getters only with the engine on; by then the simulation has been loaded for several seconds.

The header comment of the file says that calling the getters on `owner.FindComponent(VehicleTrackedSimulation)` crashed Workbench. Later reverse engineering traced that crash (crash 1) to `GetInputs()`, so the `FindComponent` route is probably safe. It was not tested again; `GetSimulation()` through the controller stays the recommended path.

### LEO\_RunningGearComponent (scripted running gear)

It replaces `CarProcAnimComponent` for the running gear, and the native rope for the visible links. Every frame, it reads the simulation per (track, part) and writes local bone matrices with `Animation.SetBoneMatrix`, the same call that vanilla `SCR_DestructionTireComponent` uses. It also owns one `LEO_TrackChain` per track. The user confirmed the result in game on 2026-10-06.

![Scripted links wrapping the rear sprocket of the Leopard 2A6](../images/sprocket-wrap.jpg)

*Test video A (2026-10-06), 209.5 s: right side on grass at about 8–10 km/h. The scripted links wrap the rear sprocket, run under all seven road wheels and over the return rollers. Leopard 2A6 model by buh-late, CC BY 4.0, modified.*

**Requirements**

1. **`TrackPart` order**, the same on both tracks: road wheels at indices 0 to n−1, the idler at n, the sprocket at n+1, the return rollers after it. The idler must be at the front and the sprocket at the rear (the belt order of `LEO_TrackChain`). Track 0 (the first `Track`, `Track Left` on the Leopard) uses the `_l` bones; track 1 uses the `_r` bones.
2. **The `TrackPart` with index k** (0-based) must be the wheel whose bones are numbered k + 1. The script builds the names as `"v_wheel_" + side + "0" + k`, so it supports at most 9 road wheels and 9 rollers per side.
3. **The bones in the table below**, per side `<s>` = `l` / `r` and number `<k>` = 1..n. Each sits at the centre of its part with an **identity rest orientation**, because the script builds its matrices in hull axes.
4. **`CarProcAnimComponent` empty.**

| Bone | Parent | Moved by the script |
| --- | --- | --- |
| `v_suspension_<s>0<k>` | `v_body` | Translation along Y (the suspension travel) |
| `v_wheel_<s>0<k>_rotator` | `v_suspension_<s>0<k>` | Not moved. It is left over from the `wheel_suspension.pap` rig |
| `v_wheel_<s>0<k>` | `v_wheel_<s>0<k>_rotator` | Rotation about X. It is also the `TrackPart` pivot bone |
| `v_idler_<s>`, `v_sprocket_<s>`, `v_roller_<s>0<k>` | — | Rotation about X |

[Chapter 14](14-running-gear.md) shows how the Leopard's bones were added in Blender without moving a vertex, and how the suspension arms were re-weighted to swing with the wheel.

**Attributes.** The defaults are the Leopard 2A6 values.

| Attribute | Default | Purpose |
| --- | --- | --- |
| `m_iRoadWheels` | 7 | Road wheels per track (`TrackPart` 0 to n−1) |
| `m_iRollers` | 4 | Return rollers per track (the `TrackPart`s after the sprocket) |
| `m_fRoadWheelRadius` | 0.345 m | Visual road wheel rim radius, where the wheel rolls on the track's inner face |
| `m_fRollerRadius` | 0.0847 m | Visual return roller rim radius |
| `m_fRestHeight` | 0.4186 m | Modelled road wheel centre height. Used only if a wheel bone cannot be read |
| `m_fMaxTravelDown` | 0.10 m | Must equal `MaxTravelDown` of the `Suspension` block |
| `m_fMaxTravelUp` | 0.30 m | Must equal `MaxTravelUp` of the `Suspension` block |
| `m_fDropRate` | 8 /s | How fast a wheel drops to full droop when it loses ground contact, and comes back when it lands |
| `m_bScriptedChain` | 1 | Moves the links by script and hides the native rope's links |
| `m_sLinkModel` | `{B50AEDC702735F50}Assets/Vehicles/Tracked/Leopard2A6/Leopard2A6_track_link.xob` | Link model of the scripted chain |
| `m_iLinks` | 73 | Links per track |
| `m_sNativeLinkName` | `Leopard2A6_track_link` | Part of the native link model's file name, used to find and hide the rope's links among the hull's children |
| `m_fMaxDistance` | 300 m | Beyond this distance from the camera, bones and links are not updated |

**What it computes every frame**

Suspension travel of each road wheel, from the simulation's contact point (hull space) and the wheel's simulation radius:

```math
\text{travel} = \operatorname{clamp}\left(y_{\text{contact}} + r_{\text{sim}} - y_{\text{rest}},\ -\text{MaxTravelDown},\ +\text{MaxTravelUp}\right)
```

With contact, the bone follows the simulation exactly. Without contact, it eases to full droop with the factor 1 − e^(−rate·Δt), which does not depend on the frame rate. After landing, it eases back until it is within 5 mm, then follows exactly again. The clamp matters: in the logged sessions the contact-derived travel reached +0.382 m, above `MaxTravelUp` 0.30, because the suspension ray starts above the top of the travel (measured).

Track distance per frame, from road wheel 0 (the speed of the track's ground face):

```math
d = \text{RPM}_0 \cdot \frac{2\pi}{60} \cdot r_{\text{sim},0} \cdot \Delta t
```

- Road wheels and return rollers **roll** on the track's inner face: angle += d / visual rim radius (0.345 m and 0.0847 m).
- The idler and the sprocket are **wrapped** by the track and turn with the links: angle += d / their simulation radius (0.335 m and 0.34 m).
- A positive angle moves the top of the wheel forward (+Z). The bone matrix rows are (1, 0, 0), (0, c, s), (0, −s, c), with c = cos and s = sin of the angle.
- The suspension bone gets an identity matrix with the translation (0, travel, 0).

Then the chain of each track gets the new road wheel heights and moves its links by d.

**Accuracy (measured with `LEOSUSP`).** In the 2026-10-05 23:58 and 2026-10-06 00:12 sessions, the bone-minus-simulation difference had a median of 0 mm, with 95% and 90% of samples within 3 mm. Values up to 124 mm occurred only while a wheel eased back after losing contact. In the first run with the script (23:18), 70% of samples were within 3 mm, with transients up to 83 mm on obstacles.

**Limits and open points**

- It does nothing on a dedicated server (`System.IsConsoleApp()`). The links are local entities and are not replicated. Behaviour on other players' machines (proxies) is **not tested**.
- The track distance comes from road wheel 0. A wheel without contact can report a very different RPM (−230.6 rpm in reverse while the grounded wheels read 0–20, measured). So the wheels and links can spin wrongly while the front road wheel is in the air in reverse. Using a grounded wheel or the sprocket is untested (open).
- The script runs in `EOnFrame`. A one-frame lag against the physics is possible but not shown (open, low confidence).
- The native rope keeps running with its links hidden. Whether `WheelGetRPM` depends on it was not tested, and its CPU cost was not measured.
- If any running-gear bone is missing, or a wheel's rest height is outside 0.5–2 × `m_fRestHeight`, the scripted chain is switched off and a `LEOGEAR` warning is printed.

### LEO\_TrackChain (scripted link chain)

A plain class, created by `LEO_RunningGearComponent`, one per track. Every update it:

1. rebuilds the belt path in the track plane (hull z along, y up) from the circles in belt order: sprocket, return rollers rear to front, idler, road wheels front to rear. Each circle has the part's **simulation** radius, which is the links' ground face. Road wheel circles move with the suspension;
2. joins neighbouring circles by their outer common tangents and wraps each circle between them. Where the wrap is more than half a turn (a road wheel pushed above its neighbours), it uses a straight chord instead;
3. places N links at equal spacing L / N along the path length L, shifted by the track distance d.

Each link is a `GenericEntity` child of the hull, created with `GetGame().SpawnEntity`, attached with `AddChild(link, -1, EAddChildFlags.AUTO_TRANSFORM)`, given the model with `SetObject`, and cleared of the `TRACEABLE` and `ACTIVE` flags. The links are deleted in `OnDelete`. Because the links depend only on the wheel positions, they cannot sag through the ground or come off the sprocket. That follows from the construction; a flip or a mine blast was **not tested** with the scripted chain. [Chapter 15](15-track-links.md) has the geometry, the link frame and the comparison with the native rope.

### LEO\_TrackBeltComponent (static belt fallback)

The Leopard's hull model keeps its original static belts (meshes `track_l` and `track_r`). This component counts the hull's child entities whose model name contains `m_sLinkModelName`, native rope links and scripted links alike. Once there are at least 2, it hides the static belts with `Animation.ShowMesh`, prints `LEOTRACK link chain found (<n> links)` and stops. The logs showed 146 with the native rope only and 292 with both chains. If no link appears within `m_fTimeout`, it keeps the static belts and prints a warning, so a broken chain never leaves the tank without tracks. It runs on every machine, because mesh visibility is local.

| Attribute | Default | Purpose |
| --- | --- | --- |
| `m_sStaticBeltMeshes` | `track_l track_r` | Names of the static belt meshes, separated by spaces |
| `m_sLinkModelName` | `Leopard2A6_track_link` | Part of the link model's file name |
| `m_fTimeout` | 5 s | How long to wait for link entities |

### tools/trackpositions.py (offline TrackPositions)

The native rope needs a `TrackPositions` list: (z, y) points in hull model space around the wheels. Workbench has an editor action *Generate track positions* for it, but it was not found in the Workbench UI during this work. This script computes the list offline.

```
python trackpositions.py [N] [pitch]
```

- Edit the constants at the top for your tank: `WHEELS` (the z of each road wheel), the idler and sprocket entries of `C` (z, y, simulation radius), and the two lines that add the road wheels and the return rollers (their height, radius and z). Use the **simulation** radii: the chain points lie on the links' ground face.
- It samples every circle, takes the convex hull, orders it so that the index increases **forward along the top run** (the engine's own winding), and resamples it at equal arc length L / N.
- It prints a comment with the hull length, N, the arc step, the shortest and longest chord and N × pitch, a check line with the first two points, and the `TrackPositions { ... }` block.
- With the default Leopard values it reproduces the prefab list exactly: hull length 15.2841 m, arc step 0.2094 m for N = 73. The docstring says the list starts at the top point closest to z = 0, but with the Leopard's collinear return rollers it starts above the rearmost roller (z −2.0577).
- It needs Python 3.8 or newer (`math.dist`).

Choose N and `TrackLength` with the slack rules in [chapter 15](15-track-links.md).

### Rules for writing your own scripts

**Getting the simulation**

- Get it with `TrackedControllerComponent.GetSimulation()`.
- Guard every frame until it is loaded. The getters return 0, false or a zero vector until then, and `GetGear()` returns 0 (= R):

```c
VehicleTrackedSimulation simulation = m_Controller.GetSimulation();
// TrackCount() is 0 until the simulation is loaded (reverse-engineered: the getters test an internal state first)
if (!simulation || !simulation.IsValid() || simulation.TrackCount() < 2)
	return;
```

**Wheel getters** (full list in the [Appendix](17-appendix.md), behaviour in [chapter 14](14-running-gear.md))

- Indices are (track, `TrackPart` index in the prefab order). `WheelCount()` is the count **per track** (13 on the Leopard).
- Keep `track < TrackCount()` and `wheel < WheelCount()` yourself. Only `WheelGetPosition` checks its wheel index; the others read foreign memory with an index out of range (reverse-engineered).
- `WheelGetContactPosition` is in world space; convert it with `owner.CoordToLocal()`. It returns a stored value even without contact, so test `WheelHasContact` first.
- `WheelGetRPM` has the sign of the forward speed. A road wheel's RPM is the belt speed divided by its simulation radius (measured). For the idler and the sprocket this was not checked.
- `WheelGetPosition(track, wheel, 0)` returns the full-droop wheel centre relative to the physics centre of mass (the model's COM point plus the `CenterOfMass` offset), not relative to the hull origin. On the 2A6 with `MaxTravelDown` 0.15 it read y = −1.899 for every wheel. Only displacement 0 was tested.

**Moving bones**

- Use `owner.GetAnimation().SetBoneMatrix(owner, bone, mat)` every frame. The matrix is a local delta over the bind pose: identity means rest. Write it again every frame, as `SCR_DestructionTireComponent` does.
- Read the rest positions with `GetBoneMatrix` in `OnPostInit`, before anything moves the bones.

```c
// spin a bone about its X axis (identity rest orientation); positive = top of the wheel moves forward (+Z)
float c = Math.Cos(angle);
float s = Math.Sin(angle);
vector mat[4];
mat[0] = Vector(1, 0, 0);
mat[1] = Vector(0, c, s);
mat[2] = Vector(0, -s, c);
mat[3] = vector.Zero;
anim.SetBoneMatrix(owner, bone, mat);
```

**Visual-only entities and cost**

- Spawn them locally (`GetGame().SpawnEntity`), attach them with `AddChild(..., EAddChildFlags.AUTO_TRANSFORM)`, clear `TRACEABLE` and `ACTIVE`, and delete them in `OnDelete`.
- Skip the work far from the camera (`owner.GetWorld().GetCurrentCamera`) and on a dedicated server (`System.IsConsoleApp()`).

**Drivetrain calls**

- Use `GetGear()`, `GetClutch()`, `GetBrake()`, `EngineGetRPM()` and `EngineGetRPMFeedback()`.
- Gear index 0 is R, 1 is N, 2 and up are the forward gears (`EVehicleGearboxGear.REVERSE` / `NEUTRAL`).
- `GetBrake()` reads 6.0 for the handbrake: `SetBreak(x, true)` stores 6.0. The controller holds it at a standstill and in pivot turns.
- **Never** call `GetInputs()` or `GetFloorNormalAngles()`. Their bindings write through a null pointer (crash 1).
- `GetThrottle()` always returns 0, `SetThrottle()` does nothing and `IsHandbrakeOn()` always returns false on the tracked simulation. Read the `engineThrust` signal instead.
- Read the driver's inputs with `GetGame().GetInputManager().GetActionValue("TrackedBrake")` and similar calls.

**Language and workflow**

- `Physics` can only be a local variable. As a class member, the compiler rejects it.
- Recompile with *Play* stopped. Reload scripts with no world open in the Diag exe.
- After a prefab change, close the world and open it again before you test ([Troubleshooting](12-troubleshooting.md)).

---

[← Tuning with real-world data](10-tuning.md) · [Contents](../../README.md) · [Troubleshooting →](12-troubleshooting.md)
