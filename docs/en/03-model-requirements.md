> [🇧🇷 Português](../pt-BR/03-model-requirements.md) · 🇬🇧 English

[← Architecture](02-architecture.md) · [Contents](../../README.md) · [Step by step from scratch →](04-from-scratch.md)

# 3D model requirements

The simulation only needs **one bone per wheel**, with the name you reference in the prefab. The bone is the ground contact point: the engine casts a ray down from it and uses the wheel radius (`Radius`).

**Required bones per track** (the names are up to you; these are the Leopard's):

| Part | Leopard bone | Position | Note |
| --- | --- | --- | --- |
| Idler | `v_idler_l` / `v_idler_r` | Center of the front wheel | Must exist on both tracks |
| Road wheels | `v_wheel_l01`…`l07` / `v_wheel_r01`…`r07` | Center of each wheel | At least 3 wheels per track, counting all parts |
| Drive sprocket | `v_sprocket_l` / `v_sprocket_r` | Center of the rear wheel | Its radius sets the speed |
| Return roller | (optional) | — | Type `Return Roller`; the Leopard does not use it |

Rules the engine checks on load:

- **Exactly 2 tracks.** Error: `Invalid number of tracks! Must be 2!`
- **At least 3 wheels per track.** Error: `Every track should have at least 3 wheels!`
- **The same count of each type on both sides.** Error: `Wheels - ... mismatch between tracks`

**Orientation and scale** (Blender → Enfusion): model in meters, with the vehicle front toward **+Y**, Z up and the ground at z = 0. The default Blender FBX exporter (forward -Z, up Y) maps Blender +Y to Enfusion +Z (front). The +X axis stays the right side. So the suffix `_l` = -X side and `_r` = +X side.

**Bones in Blender:** one armature with a root bone that is **not** named `Scene_Root` (the `.xob` already creates a root node with that name; on the Leopard the root bone is `v_body`). Each moving part has 100% weight on its own bone, with no mixed weights.

**Colliders:** `UCX_*` meshes (convex), with the custom property `usage = "Vehicle"`. The physics material of each collider is set in the `.xob.meta`. The vanilla armor materials are `Common/Materials/Game/Armor/armor_{1..100}mm.gamemat`. The default that the importer assigns (`{536BF67B2052B869}material/metal.gamemat`) does not exist and causes an error.

**Import in Workbench** (Resource Browser → right-click → *Register and import* → *as Model*). Then, in the *Import Settings* panel of the `.xob`:

| Option | Value | Why |
| --- | --- | --- |
| Merge Meshes | off | Otherwise the hull, wheels and tracks become a single mesh |
| Export Skinning | on | Without it, the bones do not go into the `.xob` |
| Export Scene Hierarchy | on | Keeps the hierarchy of bones and points |

In the `.meta`, this shows up as `MergeMeshes 0`, `ExportSkinning 1` and `ExportSceneHierarchy 1`.

A turret goes in a separate `.xob`, attached to the hull by a slot bone (`v_turret_slot`, at the center of the ring). The turret has its own bones: `v_gun_01` at the trunnion, crew points, sights, etc.

---

[← Architecture](02-architecture.md) · [Contents](../../README.md) · [Step by step from scratch →](04-from-scratch.md)
