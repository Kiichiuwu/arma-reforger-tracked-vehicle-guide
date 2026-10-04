> [🇧🇷 Português](../pt-BR/04-from-scratch.md) · 🇬🇧 English

[← 3D model requirements](03-model-requirements.md) · [Contents](../../README.md) · [Video examples →](05-video-examples.md)

# Step by step: a tracked vehicle from scratch

This walkthrough takes a new model to a tank that drives, turns and reverses. Everything was tested on the Leopard 2A7, Workbench 1.8.0.13.

### 1. Create the mod project

1. In Workbench, create a new project that depends **only** on the base game (`ArmaReforger`, GUID `58D0FB3206B6F859`).
2. Create the folders `Assets/Vehicles/Tracked/<Nome>/` and `Prefabs/Vehicles/Tracked/<Nome>/`.

### 2. Import the model

1. Copy the `.fbx` into `Assets/...` and use *Register and import* → *as Model*.
2. Set *Merge Meshes* (off), *Export Skinning* and *Export Scene Hierarchy* (on). Reimport.
3. Replace the physics material of each `UCX_*` collider with an `armor_XXmm.gamemat`.
4. Open the `.xob` and check that all the wheel bones appear in the bone list.

### 3. Create the hull prefab

The prefab inherits from `TrackedVehicle_Base.et`, which already includes the controller, the networking and an empty simulation. Create a `.et` text file and register it in the Resource Browser (right-click → *Register*).

```
Vehicle : "{0608D8FA71FD3433}Prefabs/Vehicles/Core/TrackedVehicle_Base.et" {
 components {
  MeshObject "{51DAA09FEFBFC0E7}" {
   Object "{GUID_DO_XOB}Assets/Vehicles/Tracked/Nome/Nome_hull.xob"
  }
  RigidBody "{51DAA09FECF52BBF}" {
   Mass 64500
   CenterOfMass 0 1 -0.3
  }
  VehicleTrackedSimulation "{6160DE6434513C4D}" {
   Simulation Tracked "{6160DE65F2933990}" {
   }
  }
  BaseVehicleNodeComponent "{5D6501223785D0E7}" {
   components {
    SCR_TrackedControllerComponent "{60F8D3F71B2F1D79}" {
     ThrottleReverseTarget 1
    }
   }
  }
 }
}
```

**ID rule:** the text in braces after the class name is the instance ID. To **override** an inherited component, repeat the vanilla ID, as above. A new ID creates a second component.

**Do not repeat the `.ct` inheritance.** `TrackedVehicle_Base.et` already declares `VehicleTrackedSimulation_Base.ct`. In the child, write only `VehicleTrackedSimulation "{6160DE6434513C4D}" {`.

### 4. Fill in the simulation

The vanilla `VehicleTrackedSimulation_Base.ct` is incomplete. It only connects engine → clutch → gearbox and sets friction and aerodynamics. Four things are missing, and without them the log shows `Tracked - failed to load!`:

| Missing | What to write | Error if missing |
| --- | --- | --- |
| Friction curves | `FrictionCurveLongitudinal` and `FrictionCurveLateral` | `FrictionCurveLateral - configuration is missing!` |
| Wheel templates | `RoadWheel` (with `Suspension`), `Sprocket`, `Idler` | `TrackWheel's mass / radius / brake torque is incorrect!` |
| Tracks | `Tracks { Track Left {...} Track Right {...} }` | `Invalid number of tracks! Must be 2!` |
| Valid engine | `Inertia`, `Steepness`, `Friction` greater than zero | `Engine's inertia is incorrect! (must be greater than zero)` |

Paste this inside `Simulation Tracked "{6160DE65F2933990}" { }`. The values are the Leopard's, already tested:

```
SolverSubsteps 4
Engine Engine Engine {
 Inertia 3
 Steepness 50
 Friction 470
 MaxPower 1100
 MaxTorque 4700
 RpmMaxPower 2600
 RpmMaxTorque 1600
 RpmIdle 700
 RpmRedline 2700
 RpmMax 3000
 HasGovernor 1
 Output "Clutch"
}
Clutch Clutch Clutch {
 MaxClutchTorque 9000
 Output "Gearbox"
}
Gearbox Gearbox Gearbox {
 Forward {
  6.0 3.3 1.8 1.0
 }
 Reverse 2.3
 Efficiency 0.9
}
Differential TripleDifferential "{663757C64F4BFDDD}" {
 Ratio 4.9
 SteeringRatio 6
 NeutralSteeringRatio 12
}
FrictionCurveLongitudinal CurveCubicSplineFloat "{8C1E5A3B7D924F60}" {
 SplineType Linear
 EndCondition OpenFlat
 Knots {
  0
  1
 }
 ParamMin 0
 ParamMax 1
 ValueMin 0
 ValueMax 1
 UnrestrictedFlags 0
 Values {
  1
  1
 }
}
FrictionCurveLateral CurveCubicSplineFloat "{2F7B9D41C6A3E858}" {
 SplineType Linear
 EndCondition OpenFlat
 Knots {
  0
  1
 }
 ParamMin 0
 ParamMax 1
 ValueMin 0
 ValueMax 1
 UnrestrictedFlags 0
 Values {
  1
  1
 }
}
RoadWheel RoadWheel "{379D05FA6C0BDB8F}" {
 Mass 110
 Radius 0.438
 BrakeTorque 6000
 Suspension Suspension "{DBF219524735553D}" {
  MaxSteeringAngle 0
  SpringRate 450
  CompressionDamper 25000
  RelaxationDamper 35000
  MaxTravelUp 0.30
  MaxTravelDown 0.15
  RayStartOffsetUp 0.5
 }
}
Sprocket Sprocket "{68A5D8FC81D86ECE}" {
 Mass 250
 Radius 0.355
 BrakeTorque 30000
}
Idler Idler "{C8F9E465BBF2B04F}" {
 Mass 150
 Radius 0.265
 BrakeTorque 0
}
RegenerativeSteering 1
NeutralSteering 1
PivotSteering 1
PivotSteeringThreshold 0.5
BrakeSteering 0
RetarderBaseTorque 0
RetarderThrottleSpeed 20
Aerodynamics Aerodynamics "{6160DE6521A7B70A}" {
 ReferenceArea 9.5
 DragCoefficient 0.9
}
```

The `RoadWheel`, `Sprocket` and `Idler` templates are **one of each**, shared by all wheels of that type. There is no per-wheel radius.

### 5. Declare the tracks

Still inside `Simulation Tracked`, list the parts of each track **from front to back**. The first track is the left one (index 0); the second is the right one (index 1).

```
Tracks {
 Track Left {
  Wheels {
   TrackPart Idler_L {
    Type Idler
    Pivot Pivot "{C7D9DF8DAAE72E34}" {
     Bone "v_idler_l"
    }
   }
   TrackPart Wheel_L01 {
    Pivot Pivot "{3542EB258D51A1DB}" {
     Bone "v_wheel_l01"
    }
   }
   TrackPart Wheel_L02 {
    Pivot Pivot "{74F1DDA5267BB96C}" {
     Bone "v_wheel_l02"
    }
   }
   TrackPart Sprocket_L {
    Type Sprocket
    Pivot Pivot "{F67E7A2BDE42902D}" {
     Bone "v_sprocket_l"
    }
   }
  }
 }
 Track Right {
  Wheels {
   TrackPart Idler_R {
    Type Idler
    Pivot Pivot "{78491E57BF71F53D}" {
     Bone "v_idler_r"
    }
   }
   TrackPart Wheel_R01 {
    Pivot Pivot "{A57984BE1E4ABF5B}" {
     Bone "v_wheel_r01"
    }
   }
   TrackPart Wheel_R02 {
    Pivot Pivot "{6BFC478567B41E45}" {
     Bone "v_wheel_r02"
    }
   }
   TrackPart Sprocket_R {
    Type Sprocket
    Pivot Pivot "{56A9D7F8C0E3E63F}" {
     Bone "v_sprocket_r"
    }
   }
  }
 }
}
```

The example has 2 road wheels per side to fit on the page; add the others in the same format. Each `Pivot` needs its own ID (any 16 hexadecimal digits, with no repeats).

- **Road wheels take no `Type`.** The enum default is `Road Wheel`, so you avoid writing a name with a space.
- **`Type Idler`** and **`Type Sprocket`** go without quotes.
- **Return roller:** `Type` with the value `Return Roller` and a `Roller Roller "{id}" { ... }` template. Not tested.

### 6. Colliders and mass

- The `RigidBody` needs colliders in the `.xob`; without them the log shows `Tracked - no physics component!`.
- `Mass` is in kg. The Leopard uses 64500.
- `CenterOfMass` should be low and slightly behind the center. A high center makes the tank roll over in turns.

### 7. Driver seat

To test, someone has to drive. The `PilotCompartmentSlot` already comes from `TrackedVehicle_Base.et`; set up the driver's entry as for any other vehicle in the game.

### 8. Test

1. Create a test world as a sub-scene of a vanilla map (for example `GM_Arland`) and place the prefab.
2. Press *Play*, get in as the driver and start the engine.
3. Check each function:

| Command | Expected result |
| --- | --- |
| W | Accelerates at 80% throttle (`ThrottleTurbo` 0.2 reserves the other 20%) |
| W + Shift (turbo) | 100% throttle |
| S while moving | Brakes |
| S at a standstill | Engages reverse by itself (there is no reverse key). See the [bugs section](09-bugs.md): native reverse does not move |
| A / D while moving | Turns, with the outer track faster |
| A / D at a standstill | Turns in place (pivot / neutral steering) |

4. Open the *Log Console* and look for lines with `Tracked`, `Wheels`, `Engine` or `TrackWheel`. The [Troubleshooting](12-troubleshooting.md) section explains each error.
5. To make reverse actually work, add the component from the [Ready-made scripts](11-scripts.md) section.
6. Compare with the expected result in [Video examples](05-video-examples.md).

---

[← 3D model requirements](03-model-requirements.md) · [Contents](../../README.md) · [Video examples →](05-video-examples.md)
