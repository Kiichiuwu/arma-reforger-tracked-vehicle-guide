> [🇧🇷 Português](../pt-BR/03-model-requirements.md) · 🇬🇧 English

[← Architecture](02-architecture.md) · [Contents](../../README.md) · [Step by step from scratch →](04-from-scratch.md)

# 3D model requirements

To drive, the simulation only needs **one bone per running gear part**, with the name you reference in the prefab. To show a moving suspension, spinning wheels and track links, the model needs a little more: a suspension bone per road wheel, a bone per return roller, named static belt meshes, and one track link as a separate model. This chapter lists all of it, with the Leopard 2A6 values.

All positions below are in hull space as Enfusion sees it: x to the right, y up, z forward, ground plane at y = 0. "∓" means negative on the left side, positive on the right side.

### What a part bone means to the simulation

Each `TrackPart` in the prefab points to a bone through its `Pivot`. That bone is the **centre of the part at its modelled (rest) position**. It is not the ground contact point.

For a road wheel the simulation does this:

| Step | What happens | How it is known |
| --- | --- | --- |
| 1 | A ray starts `RayStartOffsetUp` above the top of the suspension travel and runs along the hull's down axis | Start: attribute description in the 1.8.0.13 executable, "Extension of raycast start above top position of suspension". Direction: inferred from step 2 |
| 2 | The wheel centre is placed at the contact point plus `Radius`, along the hull's up axis | Measured on the 2A6: this rule matched the engine's own suspension signal with a mean difference of about 1 mm (0.9–1.7 mm per wheel, 106 frames) |
| 3 | The travel is the distance of that centre from the bone, from −`MaxTravelDown` to +`MaxTravelUp` | Measured on the 2A6 |

So **`Radius` is the distance from the wheel centre to the ground face of the track**, not the rubber rim. It is about the rim radius plus the link thickness. If the track's ground face lies on the ground plane of your model, `Radius` equals the height of the road wheel bone (2A6: bone at 0.4186 m, `Radius` 0.418). [Suspension and ride height](13-suspension.md) explains the spring and the ride height.

The idler, sprocket and return rollers have no suspension; they stay where their bones are. Their `Radius` follows the same idea: the radius of the links' ground face around that part. These circles are also the path of the link chain, native or scripted ([Track links](15-track-links.md)).

![Right side of the stopped Leopard 2A6 with the native debug overlay: white circles around every wheel bone reach the ground face of the track links](../images/debug-overlay-both-tracks.jpg)

*Test video A (2026-10-06), 0:30, tank stopped on the runway. The vehicle debug overlay draws each `TrackPart` as a white circle of its sim `Radius` around its bone. The road wheel circles reach the ground face of the links, below the visible rims; the small circles on the top run are the return rollers. The lines with ticks are the native rope chains of both tracks. Model "Leopard 2 A6" by buh-late, CC BY 4.0, modified ([credits](../../CREDITS.md)).*

### Part bones and radii (Leopard 2A6)

| Part | Bones (left / right) | Bone position (m) | Sim `Radius` (m) | Visual radius (m) |
| --- | --- | --- | --- | --- |
| Road wheels | `v_wheel_l01`…`l07` / `v_wheel_r01`…`r07` | x ∓1.3288, y 0.4186, z 2.478 / 1.662 / 0.889 / 0.160 / −0.722 / −1.596 / −2.478 (front to rear) | 0.418 | rim 0.345 |
| Idler (front) | `v_idler_l` / `v_idler_r` | x ∓1.3298, y 0.840, z 3.128 | 0.335 | 0.264 |
| Drive sprocket (rear) | `v_sprocket_l` / `v_sprocket_r` | x ∓1.3307, y 0.824, z −3.297 | 0.34 | about 0.29–0.32 at the teeth (not measured precisely; no script uses it) |
| Return rollers | `v_roller_l01`…`l04` / `v_roller_r01`…`r04` | y 1.078, z 2.131 / 1.341 / −0.400 / −2.058; x ∓1.4193 (rollers 01 and 03), ∓1.1593 (02 and 04) | 0.152 | 0.0847 |

How the sim radii were found (measured on the model): the inner face of the links lies 0.256 m from the idler centre and 0.262 m from the sprocket centre, and a link is 0.076 m thick. 0.256 + 0.076 ≈ 0.335 and 0.262 + 0.076 ≈ 0.34. Over the return rollers the outer face of the top run lies 0.144–0.159 m above the roller centres, hence 0.152.

Notes:

- **The sprocket `Radius` sets the vehicle speed.** On the 2A6, speed = sprocket angular velocity × 0.34 m (measured: 0.344 m median over 162 first-gear samples; the debug overlay's chain speed divided by its angular velocity gives 0.3400). See [Tuning with real-world data](10-tuning.md).
- **Return rollers are optional.** The 2A6 has 4 per side, declared with `Type "Return Roller"` and a `Roller` template. Tested: they load, they count in `WheelCount()` (13 parts per track on the 2A6) and their circles carry the top run of the chain. Their effect on driving was not measured.
- **The wheel bones are what the scripts read.** `LEO_RunningGearComponent` takes each part's rest position from its bone and builds the scripted chain around those circles ([chapter 14](14-running-gear.md)).

### Rules the engine checks on load

| Rule | Error in the log |
| --- | --- |
| Exactly 2 tracks | `Invalid number of tracks! Must be 2!` |
| At least 3 `TrackPart` entries per track | `Wheels - configuration is missing! Every track should have at least 3 wheels!` |
| The idler and the sprocket at the same list position on both sides | `Wheels - ... mismatch between tracks` |
| `Mass` > 0, `Radius` > 0 and `BrakeTorque` ≥ 0 in every wheel template | `TrackWheel's mass is incorrect! (must be greater than zero)`, and the same for the radius and the brake torque |

The messages are strings in the 1.8.0.13 executable (reverse-engineered). The simulation keeps a single part count for both tracks, so use the same list on both sides (reverse-engineered). The list order of the parts matters too ([step 5 of the walkthrough](04-from-scratch.md#5-declare-the-tracks)).

### Orientation and scale

Model in metres (Blender → Enfusion), with the vehicle front toward **+Y**, Z up and the ground at z = 0. The default Blender FBX exporter (forward −Z, up Y) maps Blender +Y to Enfusion +Z (front) and Blender +Z to Enfusion +Y (up). The +X axis stays the right side. So the suffix `_l` = −X side and `_r` = +X side.

### Bones in Blender

**One armature, with a root bone that is not named `Scene_Root`.** The `.xob` importer already creates a root node with that name, and a bone with the same name collides with it. On a small 2A6 sub-model, this bone together with a missing collider gave `Provided entity is not valid for procedural animation`; both were fixed at the same time. The 2A6 hull's root bone is `v_body`; every part bone hangs from it.

**Turret root bone.** If the turret is a separate model and its prefab inherits a vanilla LAV25 turret, the inherited `VehicleAnimationComponent` (`LAV25.agr` / `LAV25_vehicle.asi`) animates a bone named `v_body`. On the 2A6, a turret root bone with that name ended up 0.5455 m forward of its place in game. Give the turret model's root bone another name (2A6: `v_turret_root`). The hull keeps `v_body`.

**Rest orientation.** In Blender, point every part bone forward (+Y) with roll 0. The 2A6 bones then have an identity rest orientation in Enfusion: a bone's local axes are the hull's axes (X right, Y up, Z forward). The scripts rely on this. They move a suspension bone along its own Y and spin a wheel about its own X.

**Weights.** No mixed weights on parts that turn:

| Mesh | Weight |
| --- | --- |
| Each road wheel, idler, sprocket and return roller | 100% on its own bone |
| Suspension arms | Blended between `v_body` and the wheel's suspension bone (see below) |
| Hull, static track belts and everything else that does not move | 100% `v_body` |

### Suspension rig for the road wheels

The simulation moves each road wheel up and down. The suspension arm must follow that travel but not the wheel's spin, so travel and spin need separate bones. The 2A6 gives each road wheel this chain of bones:

```text
v_body
└─ v_suspension_l01        moved along Y by the suspension travel
   └─ v_wheel_l01_rotator  not moved (steering slot of the vanilla three-bone layout)
      └─ v_wheel_l01       spun about X; the TrackPart Pivot points here
```

- All three bones copy the wheel bone's head, tail and roll, so they sit at the wheel centre and the rest pose does not change.
- The prefab's `Pivot` stays on `v_wheel_<s>0<k>`. The suspension bones are only for the animation.
- The wheel mesh keeps 100% weight on `v_wheel_<s>0<k>` and inherits the travel from its parents.
- **Suspension arms swing with the wheel.** Each arm is weighted linearly from 0 at the hull pivot (the arm end farthest from the hub) to 1 at the hub, between `v_body` and `v_suspension_<s>0<k>`. No vertex moves.
- The idler, the sprocket and each return roller keep one spin bone each.
- The `_rotator` bone comes from the three-bone layout of vanilla `wheel_suspension.pap` (wheel, suspension, rotator). `LEO_RunningGearComponent` never touches it. A rig without it was not tested.

The 2A6 rig was added to the finished model by a Blender script that only added bones (each wheel bone was re-parented under its new chain) and changed the arm weights; it checked that no vertex moved.

### Bone names the scripts expect

`LEO_RunningGearComponent` ([chapter 14](14-running-gear.md)) looks the bones up by name, with *s* = `l` or `r` and *k* = 1…9:

| Bone | Used for |
| --- | --- |
| `v_suspension_<s>0<k>` | Road wheel travel (translation along Y) |
| `v_wheel_<s>0<k>` | Road wheel spin; its rest height is the zero of the travel |
| `v_idler_<s>`, `v_sprocket_<s>` | Spin |
| `v_roller_<s>0<k>` | Spin |

- The `0` is literal. More than 9 road wheels or rollers per side need a change in the script.
- A missing bone, or a road wheel bone whose height is outside 0.5–2 × `m_fRestHeight` (default 0.4186), counts as missing. The script then logs `LEOGEAR ... running gear bones missing or unreadable` and switches the scripted chain off. Set `m_fRestHeight` to your road wheel centre height.
- `LEO_TelemetryComponent` reads `v_wheel_<s>01`…`07` and `v_sprocket_<s>` for its `LEOSUSP` line. The 7 is fixed in that script.

If your bones use another pattern (for example `v_wheel_L01`), rename them in the model or change the name strings in the scripts.

### Centre of mass point

The 2A6 hull model contains an empty object `COM_hull`, placed at the solid centroid of the main hull collider `UCX_hull_01`: hull (0.0002, 1.168, −0.381). The engine uses it. The `RigidBody` `CenterOfMass` attribute is an **offset** from this point, not a position. The executable describes it as "Offset of center of mass. Relative to one defined in model (if applicable)." Measured on the 2A6: the physics centre of mass is `COM_hull` plus the offset, within 0.01 mm.

Open: which point the engine uses when the model has no such object. [Suspension and ride height](13-suspension.md#centre-of-mass) shows how to place and level the centre of mass.

### Track link model

The native link chain and the scripted chain both spawn one entity per link with a mesh you provide ([Track links](15-track-links.md)). Make one link as its own model:

| Requirement | Why | Leopard 2A6 |
| --- | --- | --- |
| A separate `.fbx` → `.xob` with a single link | One entity per link uses this mesh | `Leopard2A6_track_link.xob` |
| Authored as a link of the **bottom run**: origin at the centre of the link (centre of the shoe pair, middle of its length), ground face at y = 0, +Y up toward the wheels, +Z forward along the track, X across | The native chain places a bottom-run link with an identity rotation (reverse-engineered). The scripted chain puts the origin on its path, at the ground face | Origin at the centre of the shoe pair, ground face at 0 |
| In Blender | Same rule with the default exporter | Ground face at z = 0, +Z up, +Y forward |
| No colliders | Links have no physics: the native ones are plain entities, the scripted ones are not traceable | `.xob.meta` without colliders |
| The belt's material | — | `Leopard2A6_track.emat` |
| LODs if you want them | — | LOD0 118 vertices / 180 faces, LOD1 47 / 60 |
| Measure the pitch and the thickness | The pitch sets `TrackLength` and the number of links; the thickness is about the difference between rim and sim radii | Pitch 0.2101 m (mean of the model's 71 links), thickness 0.076 m |
| A file name you can recognise | The scripts find link entities by part of the model name | `Leopard2A6_track_link` |

**How the 2A6 link was made.** The model's belt has 71 identical links per side, each made of 6 mesh islands (2 shoes, 2 end connectors, a centre guide, a horn). A Blender script, run on a copy of the file, cut out one bottom-run link under road wheel 3, moved its origin to the centre of the shoe pair with the ground face at 0, took the matching LOD1 islands and exported both LODs. Every link of the original belt maps the same full UV atlas, so a texture scroll cannot fake belt motion on this model (measured).

**Keep the static belts as named meshes.** Leave the model's original belts in the hull, weighted 100% to `v_body`, with names a script can find (2A6: `track_l` and `track_r`, LOD0–3). `LEO_TrackBeltComponent` hides them by name once at least 2 link entities exist. If no links appear within 5 s it keeps them and warns, so a broken chain never leaves the tank without tracks.

**Open: link direction in the scripted chain.** The scripted chain turns each link 180° about its own Y axis compared with the native chain, so front and back are swapped. The 2A6 link is not symmetric front to back (it extends from −0.097 to +0.186 m along its length), so the scripted belt may show it reversed. This was not checked in a close-up ([chapter 15](15-track-links.md)).

### Colliders

`UCX_*` meshes (convex), with the custom property `usage = "Vehicle"`. The physics material of each collider is set in the `.xob.meta`. The vanilla armour materials are `Common/Materials/Game/Armor/armor_{1..100}mm.gamemat`. The default that the importer assigns (`{536BF67B2052B869}material/metal.gamemat`) does not exist and causes an error.

The 2A6 hull's physics colliders (`LayerPreset "Vehicle"`) are `UCX_hull_01`, `UCX_skirt_l` and `UCX_skirt_r`. The road wheels have no colliders: the simulation finds the ground with its rays.

### Import in Workbench

Resource Browser → right-click → *Register and import* → *as Model*. Then, in the *Import Settings* panel of the `.xob`:

| Option | Value | Why |
| --- | --- | --- |
| Merge Meshes | off | Otherwise the hull, wheels and tracks become a single mesh |
| Export Skinning | on | Without it, the bones do not go into the `.xob` |
| Export Scene Hierarchy | on | Keeps the bone hierarchy |

In the `.meta`, this shows up as `MergeMeshes 0`, `ExportSkinning 1` and `ExportSceneHierarchy 1`.

After the import, open the `.xob` and check the bone list. The 2A6 hull got 28 more bones from the suspension rig (14 `v_suspension_*` and 14 `_rotator` bones).

**Close the world before you replace the hull `.fbx`.** Workbench reimports a `.fbx` by itself when the file changes. With the world open in the World Editor, this hot reload of the 2A6 hull froze Workbench once (2026-10-05); with no world open it reimported in 6.5 s.

---

[← Architecture](02-architecture.md) · [Contents](../../README.md) · [Step by step from scratch →](04-from-scratch.md)
