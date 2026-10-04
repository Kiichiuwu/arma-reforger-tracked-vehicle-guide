> [🇧🇷 Português](../pt-BR/06-converting.md) · 🇬🇧 English

[← Video examples](05-video-examples.md) · [Contents](../../README.md) · [Attribute reference →](07-attribute-reference.md)

# Converting an existing vehicle

Most mod tanks use the wheeled simulation (`VehicleWheeledSimulation`) with many axles. Converting to the native tracked system means swapping the base and translating the blocks.

### 1. Change the inheritance

Back up the prefab. Then change the first line so it inherits from `TrackedVehicle_Base.et`:

```
Vehicle : "{0608D8FA71FD3433}Prefabs/Vehicles/Core/TrackedVehicle_Base.et" {
```

If the prefab inherited from a vanilla vehicle (for example `LAV25_base.et`), it loses everything that came from there. Copy the components you still want into your prefab (lights, inventory, actions, slots).

### 2. Remove the car components

The two bases use different IDs for the controller node. If you only change the inheritance, the prefab ends up with two nodes and two controllers.

| Remove (wheels) | Stays in place (tracked, inherited) |
| --- | --- |
| `VehicleWheeledSimulation "{731B26FCA2F19855}"` | `VehicleTrackedSimulation "{6160DE6434513C4D}"` |
| `BaseVehicleNodeComponent "{20FB66C5B2237133}"` | `BaseVehicleNodeComponent "{5D6501223785D0E7}"` |
| `SCR_CarControllerComponent` (inside the node) | `SCR_TrackedControllerComponent "{60F8D3F71B2F1D79}"` |
| `NwkCarMovementComponent "{5D6CA5AFEC980F35}"` | `NwkTrackedMovementComponent "{5D6CA5AFEC980F35}"` |

Network movement uses the **same ID** in both bases, but the class is different. Delete the car block so it does not override the tracked one.

If your old node had a HUD or other custom components, move them into the `{5D6501223785D0E7}` node.

### 3. Translate the attributes

| Wheeled simulation | Native tracked | Note |
| --- | --- | --- |
| `Engine`, `Clutch`, `Gearbox` | The same blocks | Same names. But see the [bugs section](09-bugs.md): the gearbox output and the clutch work differently |
| `Axle` → `Wheel { Radius, BrakeTorque }` | `RoadWheel { Radius, BrakeTorque, Mass }` | A single template for all road wheels |
| `Axle` → `Suspension { ... }` | `RoadWheel` → `Suspension { ... }` | Same class and same attributes |
| `MaxSteeringAngle 25` | `MaxSteeringAngle 0` | A tracked vehicle turns by speed difference |
| `WheelPosition Wheel_L01 { PivotID "v_wheel_l01" }` | `TrackPart Wheel_L01 { Pivot Pivot "{id}" { Bone "v_wheel_l01" } }` | The old bones work unchanged |
| Per-axle differential | `Differential TripleDifferential` | `Ratio`, `SteeringRatio`, `NeutralSteeringRatio` |
| Speed from the wheel radius | Speed from the **drive sprocket** radius | Recalculate `Ratio` ([tuning section](10-tuning.md)) |

### 4. Pick the idler and the drive sprocket

Mods with fake wheels often have a row `v_wheel_l01`…`l09` with no distinction. Mark the right wheel at each end:

- Leopard, T-72 and Abrams have the drive sprocket at the **rear** and the idler at the front.
- M113 and several light armored vehicles have the drive sprocket at the **front**.
- The list order is still front to rear. Only the `Type` changes.

### 5. Clean up leftovers from the wheeled version

| Leftover | What to do |
| --- | --- |
| `SCR_WheelHitZone` with `m_iWheelId` | `TrackedVehicle_Base` itself brings four (IDs 0 to 3), carried over from the car. The tracked wheel numbering was not verified |
| `SCR_VehicleDustPerWheel` | Does not exist in the tracked base. You can keep it, but test it |
| Per-wheel sound points (`VehicleWheelSound`) | Adjust the `Offset` values to the position of the new wheels |
| `ChimeraAIPathfindingComponent`, `ChimeraAIVehicleControlComponent` | `ChimeraAIVehicleControlComponent` does not exist in the tracked base either; `ChimeraAIPathfindingComponent` already comes from `Vehicle_Base.et`. AI driving a native tank was not tested |
| `SCR_VehicleBuoyancyComponent` | Missing from the tracked base |

### 6. Protection against other mods

RHS-StatusQuo overrides the vanilla `TrackedVehicle_Base.et` and **disables** (`Enabled 0`) three components: `VehicleTrackedSimulation`, `SCR_TrackedControllerComponent` and `NwkTrackedMovementComponent`. With that mod loaded, your tank inherits the disabled components.

Precaution (not tested): write `Enabled 1` on these three components, inside your prefab. The child value wins over the parent value.

### 7. Wheel animation

In our test, the native tracked system did not spin the model's wheels by itself. The visual link system (`TrackSegments`) depends on an asset that does not ship with the game. Treat the animation as separate work, after the vehicle already drives.

---

[← Video examples](05-video-examples.md) · [Contents](../../README.md) · [Attribute reference →](07-attribute-reference.md)
