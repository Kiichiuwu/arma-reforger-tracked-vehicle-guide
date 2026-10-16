> [🇧🇷 Português](../pt-BR/06-converting.md) · 🇬🇧 English

[← Video examples](05-video-examples.md) · [Contents](../../README.md) · [Attribute reference →](07-attribute-reference.md)

# Converting an existing vehicle

Most mod tanks use the wheeled simulation (`VehicleWheeledSimulation`) with many axles. Converting to the native tracked system means swapping the base, translating the blocks, and redoing the parts that the wheeled version solved differently: the ride height, the wheel animation and the tracks.

Convert in this order: first make the vehicle drive and sit at the right height (steps 1–6), then add the animation (step 7). The values quoted are the Leopard 2A6's ([Step by step from scratch](04-from-scratch.md) has the full blocks).

### 1. Change the inheritance

Back up the prefab. Then change the first line so it inherits from `TrackedVehicle_Base.et`:

```
Vehicle : "{0608D8FA71FD3433}Prefabs/Vehicles/Core/TrackedVehicle_Base.et" {
```

If the prefab inherited from a vanilla vehicle (for example `LAV25_base.et`), it loses everything that came from there. Copy the components you still want into your prefab.

### 2. Remove the car components

The two bases use different IDs for some components. If you only change the inheritance, every block your prefab declares with a wheeled ID stays as an extra component: two nodes, two controllers, two procedural animation components.

| Remove (wheels) | Stays in place (tracked, inherited) |
| --- | --- |
| `VehicleWheeledSimulation "{731B26FCA2F19855}"` | `VehicleTrackedSimulation "{6160DE6434513C4D}"` |
| `BaseVehicleNodeComponent "{20FB66C5B2237133}"` | `BaseVehicleNodeComponent "{5D6501223785D0E7}"` |
| `SCR_CarControllerComponent` (inside the node) | `SCR_TrackedControllerComponent "{60F8D3F71B2F1D79}"` |
| `NwkCarMovementComponent "{5D6CA5AFEC980F35}"` | `NwkTrackedMovementComponent "{5D6CA5AFEC980F35}"` |
| `CarProcAnimComponent "{A201E6A6F82F2CDA}"` (the wheeled base's ID) with its wheel `ProcAnimParams` | `CarProcAnimComponent "{5D6501223785D009}"`, empty. Leave it empty (step 7) |

Network movement uses the **same ID** in both bases, but the class is different. Delete the car block so it does not override the tracked one.

If your old node had other custom components, move them into the `{5D6501223785D0E7}` node.

### 3. Translate the attributes

| Wheeled simulation | Native tracked | Note |
| --- | --- | --- |
| `Engine`, `Clutch`, `Gearbox` | The same blocks | Same names. But see the [bugs section](09-bugs.md): the gearbox output and the clutch work differently |
| `Axle` → `Wheel { Radius, BrakeTorque }` | `RoadWheel { Radius, BrakeTorque, Mass }` | A single template for all road wheels. `Radius` = distance from the wheel centre to the ground face of the track ([chapter 3](03-model-requirements.md#what-a-part-bone-means-to-the-simulation)) |
| `Axle` → `Suspension { ... }` | `RoadWheel` → `Suspension { ... }` | Same class and attribute names, but re-derive the values (below) |
| `MaxSteeringAngle 25` | `MaxSteeringAngle 0` | A tracked vehicle turns by speed difference |
| `WheelPosition Wheel_L01 { PivotID "v_wheel_l01" }` | `TrackPart Wheel_L01 { Pivot Pivot "{id}" { Bone "v_wheel_l01" } }` | The old bones work unchanged for driving |
| — | `Idler`, `Sprocket` and, with return rollers, `Roller` templates, plus a `Type` on those `TrackPart` entries | New; see step 4 |
| Per-axle differential | `Differential TripleDifferential` | `Ratio`, `SteeringRatio`, `NeutralSteeringRatio` |
| Speed from the wheel radius | Speed from the **drive sprocket** radius | Recalculate `Ratio` ([tuning section](10-tuning.md)) |

**Re-derive the suspension.** On the tracked simulation `SpringRate` is in N/mm, that is kN/m, and the spring is free (zero force) at full droop (executable description and measurements on the 2A6). `MaxTravelDown` therefore sets the ride height. Set it to the static compression:

```math
\text{MaxTravelDown} \approx \frac{M \cdot g}{n \cdot k}
```

with *n* the number of road wheels on both tracks and *k* = `SpringRate` × 1000 N/m. 2A6: 62300 · 9.81 / (14 · 450 000) = 0.097 m, so `MaxTravelDown` 0.10. With 0.15 the 2A6 hull rode 5 cm high (measured). Copied wheeled values can leave the hull high or low by the difference. Whether the wheeled simulation uses the same spring model was not checked. Details: [Suspension and ride height](13-suspension.md).

**Check the centre of mass too.** `CenterOfMass` on the `RigidBody` is an offset from the model's own centre of mass point, if the model has one, not an absolute position ([chapter 13](13-suspension.md#centre-of-mass)).

### 4. Pick the idler and the drive sprocket, and order the parts

Mods with fake wheels often have a row `v_wheel_l01`…`l09` with no distinction. Mark the right wheel at each end:

- Leopard, T-72 and Abrams have the drive sprocket at the **rear** and the idler at the front.
- M113 and several light armoured vehicles have the drive sprocket at the **front**.

Then list each track in this order:

1. The road wheels, from front to rear, with no `Type` (list positions 0…n−1).
2. The idler (`Type Idler`).
3. The drive sprocket (`Type Sprocket`).
4. The return rollers, if any (`Type "Return Roller"`).

Use the same sequence on both sides. Example for a row `v_wheel_l01` (idler, front) … `v_wheel_l09` (sprocket, rear): list `v_wheel_l02`…`l08` first, then `v_wheel_l01` as the idler and `v_wheel_l09` as the sprocket. For driving, the bone names can stay as they are; the running gear script of step 7 expects other names.

Why the order matters:

- The native link chain runs at the angular velocity of part 1 times the radius of part 0, so both must be road wheels (reverse-engineered from the 1.8.0.13 executable).
- The script API and the scripts in this guide address parts as (track, list position). `WheelCount()` returns the parts of one track: 13 on the 2A6.
- The loader wants the idler and sprocket at the same position on both tracks, and the simulation keeps a single part count for both (reverse-engineered).
- The driving physics itself worked with both orders (the earlier 2A7 build listed the idler first).

**Front sprocket.** The part order above stays the same. Two things are not tested with a front sprocket:

- **The scripted chain.** `LEO_TrackChain` builds the belt clockwise as seen from the right, starting at the sprocket: in `LEO_RunningGearComponent.CreateChains` the first circle is the sprocket (rear), then the return rollers from rear to front, then the idler (front), then the road wheels from front to rear. With a front sprocket, swap the sprocket and idler circles there (both the rest position and the radius index). The road wheel and roller code needs no change.
- **The native rope.** Its sprocket grip lets go of a point once it is behind or below the sprocket centre (reverse-engineered). How that works with a front sprocket was not checked; the rope was only run on the Leopard's rear sprocket.

### 5. Clean up leftovers from the wheeled version

| Leftover | What to do |
| --- | --- |
| `SCR_WheelHitZone` with `m_iWheelId` | `TrackedVehicle_Base` itself brings four (IDs 0 to 3), carried over from the car. The simulation numbers wheels per track, as (track, list position), with `WheelCount()` parts per track (13 on the 2A6). How a flat `m_iWheelId` maps onto that is not verified, and reordering the `TrackPart` list changes the positions |
| `CarProcAnimComponent` parameters for the wheels | Remove them (step 2 and step 7) |
| Your own track animation (a material UV scroll or separate track entities driven by script) | It was written for the wheeled simulation and was not tested on the tracked one. Remove it if you use the link chains of step 7 |
| `ChimeraAIPathfindingComponent`, `ChimeraAIVehicleControlComponent` | `ChimeraAIVehicleControlComponent` does not exist in the tracked base either; `ChimeraAIPathfindingComponent` already comes from `Vehicle_Base.et`. AI driving a native tank was not tested |

### 6. Protection against other mods

RHS-StatusQuo overrides the vanilla `TrackedVehicle_Base.et` and **disables** (`Enabled 0`) three components: `VehicleTrackedSimulation`, `SCR_TrackedControllerComponent` and `NwkTrackedMovementComponent`. With that mod loaded, your tank inherits the disabled components.

Precaution (not tested): write `Enabled 1` on these three components, inside your prefab. The child value wins over the parent value.

### 7. Suspension, wheel and track animation

Do this after the vehicle drives. The tracked simulation computes the travel and spin of every part, but in 1.8.0.13 it does not animate the model for you.

**Leave `CarProcAnimComponent` empty.** On a tracked vehicle it gives most wheel bones another wheel's signals. On the 2A6, with `wheel_suspension.pap` on the road wheels, only the front left wheel got its own signals (measured with the `LEOSUSP` telemetry). A second run with the parameters interleaved left and right showed the same kind of mix-up, but that run had not reloaded the world, so the parameter order is not fully excluded (open). Do not carry the wheeled version's wheel `ProcAnimParams` over.

![Right side of the stopped Leopard 2A6: the front road wheel sits on the track links while road wheels 2 to 7 float above them](../images/before-wheels-floating.jpg)

*Diagnostic video 2 (2026-10-05, 22:56 clip), 0:09, before the fix: the wheel bones driven by `CarProcAnimComponent`, engine idling. The front road wheel (right of the picture) sits on the links. Road wheels 2–7 float above them, because they were fed another part's signals (telemetry: 51–54 mm above the simulated wheels). Model "Leopard 2 A6" by buh-late, CC BY 4.0, modified ([credits](../../CREDITS.md)).*

**Drive the bones by script.** `LEO_RunningGearComponent` reads every part from the simulation by (track, part) and sets the suspension and spin bones each frame ([Road wheels, idler, sprocket and return rollers](14-running-gear.md)). It needs:

- a suspension bone above each road wheel bone, both with an identity rest orientation ([chapter 3](03-model-requirements.md#suspension-rig-for-the-road-wheels)). A wheeled tank often already has a three-bone rig from `wheel_suspension.pap` (wheel, suspension, steering). Check that the suspension bone is a parent of the wheel bone;
- the bone names it looks up: `v_suspension_<s>0<k>`, `v_wheel_<s>0<k>`, `v_idler_<s>`, `v_sprocket_<s>`, `v_roller_<s>0<k>` with *s* = `l` or `r`. If your bones use another pattern (vanilla wheeled vehicles use names like `v_wheel_L01`), rename them or change the name strings in the script;
- the part order from step 4;
- its attributes set for your tank: road wheel and roller counts, visual radii, rest height and travels ([step 11 of the walkthrough](04-from-scratch.md#11-animate-the-running-gear)).

**Tracks.** The wheeled version shows a static belt or its own track animation. On the tracked base you have two options ([Track links](15-track-links.md)):

| Option | What you need | Behaviour |
| --- | --- | --- |
| Native rope chain | A link model of your own (`TrackSegment`) and the chain attributes. Vanilla ships no link model: the executable only names a development M113 link that is not in the game data | Moves by itself, but sags where it has slack and came off the sprocket after a flip and after a mine blast on the 2A6. No ground collision, no script API to reset it |
| Scripted chain (2A6) | The same link model and attributes, plus `LEO_RunningGearComponent` (`m_bScriptedChain 1`) and `LEO_TrackBeltComponent` | Follows the wheels; cannot come off by construction. Not yet tested through a flip or a mine blast |

Keep the old static belts in the model as named meshes: `LEO_TrackBeltComponent` hides them once link entities exist and keeps them if no chain appears. [Step 12 of the walkthrough](04-from-scratch.md#12-add-the-track-links) lists the attributes; mind the front-sprocket note in step 4 above.

**Multiplayer.** The animation scripts run locally and do nothing on a dedicated server. They were not tested on other players' machines.

---

[← Video examples](05-video-examples.md) · [Contents](../../README.md) · [Attribute reference →](07-attribute-reference.md)
