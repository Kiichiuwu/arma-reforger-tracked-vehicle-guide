> [🇧🇷 Português](../pt-BR/17-appendix.md) · 🇬🇧 English

[← Vehicle debug diag and telemetry](16-debugging.md) · [Contents](../../README.md) · [Multiplayer →](18-multiplayer.md)

# Appendix

### A. Full Leopard 2A6 block

This is the `VehicleTrackedSimulation` block of the mod's `Leopard2A6_base.et`, without comments. It goes inside `components { }` of the hull prefab. The same text is in [`examples/leopard2a6/VehicleTrackedSimulation_Leopard2A6.et.txt`](../../examples/leopard2a6/VehicleTrackedSimulation_Leopard2A6.et.txt).

```
  VehicleTrackedSimulation "{6160DE6434513C4D}" {
   Simulation Tracked "{6160DE65F2933990}" {
    SolverSubsteps 4
    Engine Engine Engine {
     Inertia 3
     Steepness 50
     Friction 470
     MaxPower 1100
     MaxTorque 4700
     RpmMaxPower 2600
     RpmMaxTorque 1600
     BrakingTorqueRatio 0.15
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
     Ratio 4.6
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
     Radius 0.418
     BrakeTorque 6000
     Suspension Suspension "{DBF219524735553D}" {
      MaxSteeringAngle 0
      SpringRate 450
      CompressionDamper 25000
      RelaxationDamper 35000
      MaxTravelUp 0.30
      MaxTravelDown 0.10
      RayStartOffsetUp 0.5
     }
    }
    Sprocket Sprocket "{68A5D8FC81D86ECE}" {
     Mass 250
     Radius 0.34
     BrakeTorque 30000
    }
    Idler Idler "{C8F9E465BBF2B04F}" {
     Mass 150
     Radius 0.335
     BrakeTorque 0
    }
    Roller Roller "{EC9CBBD192126CCB}" {
     Mass 20
     Radius 0.152
     BrakeTorque 0
    }
    RegenerativeSteering 1
    NeutralSteering 1
    PivotSteering 1
    PivotSteeringThreshold 0.5
    BrakeSteering 0
    RetarderBaseTorque 0
    RetarderThrottleSpeed 20
    Tracks {
     Track Left {
      Wheels {
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
       TrackPart Wheel_L03 {
        Pivot Pivot "{FF507AFB45B12875}" {
         Bone "v_wheel_l03"
        }
       }
       TrackPart Wheel_L04 {
        Pivot Pivot "{4B7AD08C0BDD71B4}" {
         Bone "v_wheel_l04"
        }
       }
       TrackPart Wheel_L05 {
        Pivot Pivot "{ED4C344B4513B0A9}" {
         Bone "v_wheel_l05"
        }
       }
       TrackPart Wheel_L06 {
        Pivot Pivot "{D7F1EB919261FBB6}" {
         Bone "v_wheel_l06"
        }
       }
       TrackPart Wheel_L07 {
        Pivot Pivot "{498B3EE52377E3C9}" {
         Bone "v_wheel_l07"
        }
       }
       TrackPart Idler_L {
        Type Idler
        Pivot Pivot "{C7D9DF8DAAE72E34}" {
         Bone "v_idler_l"
        }
       }
       TrackPart Sprocket_L {
        Type Sprocket
        Pivot Pivot "{F67E7A2BDE42902D}" {
         Bone "v_sprocket_l"
        }
       }
       TrackPart Roller_L01 {
        Type "Return Roller"
        Pivot Pivot "{495CE3DBFC7DD5D5}" {
         Bone "v_roller_l01"
        }
       }
       TrackPart Roller_L02 {
        Type "Return Roller"
        Pivot Pivot "{068B4C4818A9ABAB}" {
         Bone "v_roller_l02"
        }
       }
       TrackPart Roller_L03 {
        Type "Return Roller"
        Pivot Pivot "{857B6B1E4C45E6F7}" {
         Bone "v_roller_l03"
        }
       }
       TrackPart Roller_L04 {
        Type "Return Roller"
        Pivot Pivot "{92F7DC37B619974C}" {
         Bone "v_roller_l04"
        }
       }
      }
     }
     Track Right {
      Wheels {
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
       TrackPart Wheel_R03 {
        Pivot Pivot "{C45AA21B5FB1DC10}" {
         Bone "v_wheel_r03"
        }
       }
       TrackPart Wheel_R04 {
        Pivot Pivot "{1B31E132532A4DC9}" {
         Bone "v_wheel_r04"
        }
       }
       TrackPart Wheel_R05 {
        Pivot Pivot "{73214A0BD83CBA54}" {
         Bone "v_wheel_r05"
        }
       }
       TrackPart Wheel_R06 {
        Pivot Pivot "{1135D63A6B8A7A7F}" {
         Bone "v_wheel_r06"
        }
       }
       TrackPart Wheel_R07 {
        Pivot Pivot "{078A016009810BDF}" {
         Bone "v_wheel_r07"
        }
       }
       TrackPart Idler_R {
        Type Idler
        Pivot Pivot "{78491E57BF71F53D}" {
         Bone "v_idler_r"
        }
       }
       TrackPart Sprocket_R {
        Type Sprocket
        Pivot Pivot "{56A9D7F8C0E3E63F}" {
         Bone "v_sprocket_r"
        }
       }
       TrackPart Roller_R01 {
        Type "Return Roller"
        Pivot Pivot "{EB15271F764DB1D9}" {
         Bone "v_roller_r01"
        }
       }
       TrackPart Roller_R02 {
        Type "Return Roller"
        Pivot Pivot "{0DE3390B35CF1172}" {
         Bone "v_roller_r02"
        }
       }
       TrackPart Roller_R03 {
        Type "Return Roller"
        Pivot Pivot "{61C42E2BEBDF2121}" {
         Bone "v_roller_r03"
        }
       }
       TrackPart Roller_R04 {
        Type "Return Roller"
        Pivot Pivot "{CCE7F4BD417F618E}" {
         Bone "v_roller_r04"
        }
       }
      }
     }
    }
    Aerodynamics Aerodynamics "{6160DE6521A7B70A}" {
     ReferenceArea 9.5
     DragCoefficient 0.9
    }
   }
   TrackSegments 73
   TrackPositions {
    -2.0577 1.2299 -1.8483 1.2299 -1.6390 1.2299 -1.4296 1.2299 -1.2202 1.2299 -1.0108 1.2299 -0.8015 1.2299 -0.5921 1.2299 -0.3827 1.2299 -0.1734 1.2299 0.0360 1.2299 0.2454 1.2299 0.4548 1.2299 0.6641 1.2299 0.8735 1.2299 1.0829 1.2299 1.2922 1.2299 1.5016 1.2299 1.7110 1.2299 1.9204 1.2299 2.1297 1.2299 2.3388 1.2188 2.5479 1.2074 2.7569 1.1960 2.9660 1.1846 3.1749 1.1720 3.3601 1.0817 3.4574 0.9002 3.4301 0.6960 3.2918 0.5419 3.1294 0.4097 2.9669 0.2776 2.8045 0.1455 2.6313 0.0298 2.4256 0.0006 2.2163 0.0006 2.0069 0.0006 1.7975 0.0006 1.5881 0.0006 1.3788 0.0006 1.1694 0.0006 0.9600 0.0006 0.7507 0.0006 0.5413 0.0006 0.3319 0.0006 0.1225 0.0006 -0.0868 0.0006 -0.2962 0.0006 -0.5056 0.0006 -0.7149 0.0006 -0.9243 0.0006 -1.1337 0.0006 -1.3431 0.0006 -1.5524 0.0006 -1.7618 0.0006 -1.9712 0.0006 -2.1805 0.0006 -2.3899 0.0006 -2.5976 0.0181 -2.7826 0.1146 -2.9616 0.2232 -3.1406 0.3317 -3.3197 0.4403 -3.4980 0.5500 -3.6193 0.7165 -3.6222 0.9226 -3.5055 1.0925 -3.3122 1.1638 -3.1031 1.1748 -2.8940 1.1859 -2.6849 1.1969 -2.4759 1.2080 -2.2668 1.2191
   }
   TrackLength 0.2110
   TrackThickness 0.076
   TrackOffset1 -1.3281 0 0
   TrackOffset2 1.3281 0 0
   TrackSegment "{B50AEDC702735F50}Assets/Vehicles/Tracked/Leopard2A6/Leopard2A6_track_link.xob"
  }
```

Notes on the block:

- The seven link chain attributes (`TrackSegments` to `TrackSegment`) belong to `VehicleTrackedSimulation` itself. They come **after** the closing brace of `Simulation Tracked`, not inside it.
- `TrackPositions` is one flat list of 146 numbers: 73 (z, y) pairs in hull model space. [`tools/trackpositions.py`](../../tools/trackpositions.py) reproduces it exactly ([Ready-made scripts](11-scripts.md)).
- `TrackSegment` points to the Leopard mod's own link model. Replace it with yours ([chapter 15](15-track-links.md)).
- The `TrackPart` order is the same on both tracks: road wheels first (index 0–6), then the idler (7), the sprocket (8) and the return rollers (9–12). `WheelCount()` reports 13 per track. The native chain takes its speed from parts 0 and 1, so both must be road wheels (reverse-engineered, [chapter 15](15-track-links.md)). `LEO_RunningGearComponent` expects this same order ([Ready-made scripts](11-scripts.md)).
- `Type "Return Roller"` is written as quoted text; the engine accepts it, and the `Roller` template defines the return rollers.
- The previous edition of this guide used a Leopard 2A7 block (idler first, `Ratio` 4.9, no return rollers, no link chain). It stays in the git history under the tag `v1-leopard2a7`.

The chain attributes, as the 1.8.0.13 executable registers them (reverse-engineered):

| Attribute | Type | Leopard 2A6 | What the engine does with it |
| --- | --- | --- | --- |
| `TrackSegments` | int | 73 | Read only by the editor action *Generate track positions*, as the number of points to generate. At runtime the link count is the number of `TrackPositions` entries |
| `TrackPositions` | Vector2 array | 73 pairs | The chain's rest points (z, y), shared by both tracks. Index 0 is on the top run and the index increases forward along it. The chain is built only with 2 or more entries |
| `TrackLength` | float | 0.2110 | Rest distance between neighbouring points. The chain length is N × `TrackLength`. The chain is built only if it is above 0 |
| `TrackThickness` | float | 0.076 | Stored, never read |
| `TrackOffset1` | Vector3 | −1.3281 0 0 | Offset of the first `Track` (left); x is the belt's lateral position |
| `TrackOffset2` | Vector3 | 1.3281 0 0 | Offset of the second `Track` (right) |
| `TrackSegment` | ResourceName | Leopard link `.xob` | Model of each link entity (2 × N entities, no physics) |

### B. Component overrides on the Leopard 2A6 hull

The rest of the hull prefab's tracked-vehicle setup. The first part is in [`examples/leopard2a6/HullComponents_Leopard2A6.et.txt`](../../examples/leopard2a6/HullComponents_Leopard2A6.et.txt):

```
  RigidBody "{51DAA09FECF52BBF}" {
   Mass 62300
   CenterOfMass 0 1 0.46
   LinearSleepingThreashold 0.4
   AngularSleepingThreashold 0.5
  }
  CarProcAnimComponent "{5D6501223785D009}" {
  }
  LEO_RunningGearComponent "{5292A83C2D6FC873}" {
  }
  LEO_TrackedDriveComponent "{7A1D4C9E2B6F3086}" {
  }
  LEO_TelemetryComponent "{3C5E7A9B1D2F4608}" {
  }
  LEO_TrackBeltComponent "{8E3A61C04F2D9B57}" {
  }
```

In the controllers node, the Leopard only changes `ThrottleReverseTarget`:

```
  BaseVehicleNodeComponent "{5D6501223785D0E7}" {
   components {
    SCR_TrackedControllerComponent "{60F8D3F71B2F1D79}" {
     ThrottleReverseTarget 1
    }
   }
  }
```

- `CarProcAnimComponent` stays empty; the `LEO_` components keep their default attributes, which are the Leopard values ([Ready-made scripts](11-scripts.md)).
- `CenterOfMass` is an **offset** from the centre of mass defined in the model, not an absolute position. The executable describes it as "Offset of center of mass. Relative to one defined in model (if applicable)." The 2A6 hull model has a `COM_hull` empty at (0.000176, 1.16777, −0.38093), so `0 1 0.46` puts the physics centre of mass at (0.0002, 2.1678, 0.0791), 2.17 m above the ground (measured through `WheelGetPosition`). The offset z 0.46 levels the hull (pitch −0.049° at rest). The Y offset has not been retuned yet (open, [chapter 13](13-suspension.md)).
- The sleeping thresholds let a stopped tank go to sleep. While it sleeps, telemetry lines repeat bit for bit.

### C. Leopard 2A6 running gear geometry

Bone rest positions in hull model space, in metres (Enfusion axes: x to the right, y up, z forward; ground at y = 0). Left side shown; the right side has the opposite x. Measured from the installed hull `.fbx`.

| Part | Bones | x (left) | y | z (front to rear) |
| --- | --- | --- | --- | --- |
| Road wheels | `v_wheel_l01` … `v_wheel_l07` | −1.3288 | 0.41858 | 2.47784, 1.66161, 0.88941, 0.16036, −0.72156, −1.59587, −2.47779 |
| Idler | `v_idler_l` | −1.3298 | 0.84028 | 3.12783 |
| Sprocket | `v_sprocket_l` | −1.3307 | 0.82407 | −3.29679 |
| Return rollers 01 and 03 | `v_roller_l01`, `v_roller_l03` | −1.4193 | 1.07793 | 2.13063, −0.40004 |
| Return rollers 02 and 04 | `v_roller_l02`, `v_roller_l04` | −1.1593 | 1.07793 | 1.34050, −2.05767 |

The suspension bones `v_suspension_l0<k>` and the rotators `v_wheel_l0<k>_rotator` sit at the same point as their wheel.

Simulation radius against visual radius. Each simulation radius is about one link thickness larger than the rim, because the simulation measures to the track's ground face:

| Part | Simulation `Radius` | Visual rim radius | Mass | `BrakeTorque` |
| --- | --- | --- | --- | --- |
| Road wheel | 0.418 m | 0.345 m | 110 kg | 6000 |
| Idler | 0.335 m | 0.264 m | 150 kg | 0 |
| Sprocket | 0.34 m | about 0.29 m (notes vary up to 0.32; no script uses it) | 250 kg | 30000 |
| Return roller | 0.152 m | 0.0847 m | 20 kg | 0 |

Track and links:

| Value | Leopard 2A6 | How known |
| --- | --- | --- |
| Links in the model's own belt | 71 per side | Measured on the mesh |
| Link pitch in the model | 0.2032–0.2127 m, mean 0.2101 m | Measured on the mesh |
| Shoe thickness | 0.076 m | Measured on the mesh |
| Belt width and centre | 0.6355 m, centred at x ∓1.3281 | Measured on the mesh |
| Link model `Leopard2A6_track_link.xob` | 118 vertices (LOD0), 47 (LOD1); origin at the centre of the shoe pair, ground face at y = 0; no colliders | Measured |
| Belt path around the simulation circles at rest | 15.284 m | Computed from the bones and radii |
| Chain length | 73 × 0.2110 = 15.403 m (11.9 cm over the rest path) | Computed |

These values come from the mod built on the model ["Leopard 2 A6"](https://sketchfab.com/3d-models/leopard-2-a6-7cb23d5322df4b409a880de635826067) by [buh-late](https://sketchfab.com/buh-late), licensed [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/), published 2024-01-17. It was modified for Enfusion (split into parts, re-parented, bones and suspension rig, colliders, LODs, re-baked textures). The author does not endorse this project. See [CREDITS.md](../../CREDITS.md).

### D. Vanilla resource GUIDs

| Resource | GUID |
| --- | --- |
| Base game (`ArmaReforger.gproj`) | `58D0FB3206B6F859` |
| `Prefabs/Vehicles/Core/TrackedVehicle_Base.et` | `{0608D8FA71FD3433}` |
| `Prefabs/Vehicles/Core/Components/VehicleTrackedSimulation_Base.ct` | `{BBF0C4D0AF8761DC}` |
| `Prefabs/Vehicles/Core/Vehicle_Base.et` (parent of `TrackedVehicle_Base.et`) | `{4085446E2B406849}` |
| `anims/proc/wheel_rotation.pap` | `{CCE4D4889B1BB408}` |
| `anims/proc/wheel_suspension.pap` | `{FEFBB47B33FB500F}` |
| `armor_100mm.gamemat` | `{5824DB4DA1A28E22}` |
| `armor_80mm.gamemat` | `{B6F2475AE2D7E8FD}` |
| `armor_40mm.gamemat` | `{8E8A10341B3BF250}` |
| `armor_20mm.gamemat` | `{33CE4B76B338E44F}` |
| M113 track link (not shipped with the game) | `{32FF46BA61EE0DCA}` |

The `ID "BBCBA43A9778AE21"` inside `TrackedVehicle_Base.et` is the entity ID, **not** the file GUID.

The two `.pap` files are the vanilla wheel procedural animations of the wheeled vehicles:

| File | Bone slots | Signal |
| --- | --- | --- |
| `wheel_rotation.pap` | `wheel` | Rotation about X from `rotation` (editor range −360..360, degrees) |
| `wheel_suspension.pap` | `wheel`, `suspension`, `rotator` | `wheel`: rotation about X from `rotation`. `suspension`: translation along Y from `suspension`. `rotator`: steering. The editor preview range of the suspension input is −1..1; on the tracked simulation the signal is in metres, from −`MaxTravelDown` to +`MaxTravelUp` (measured) |

On a tracked vehicle in 1.8.0.13, `CarProcAnimComponent` binds these to the wrong wheels. Do not use them for the running gear ([chapter 14](14-running-gear.md)).

The M113 link is referenced only by an engine test routine and is not in any data pak. Use your own link model.

### E. Instance IDs of the tracked base

Use these IDs to override the inherited components:

| Component | ID |
| --- | --- |
| `VehicleTrackedSimulation` | `{6160DE6434513C4D}` |
| `Simulation Tracked` | `{6160DE65F2933990}` |
| `Differential TripleDifferential` | `{663757C64F4BFDDD}` |
| `Aerodynamics` | `{6160DE6521A7B70A}` |
| `BaseVehicleNodeComponent` | `{5D6501223785D0E7}` |
| `SCR_TrackedControllerComponent` | `{60F8D3F71B2F1D79}` |
| `NwkTrackedMovementComponent` | `{5D6CA5AFEC980F35}` |
| `CarProcAnimComponent` (empty in the base) | `{5D6501223785D009}` |
| `RigidBody` | `{51DAA09FECF52BBF}` |
| `MeshObject` | `{51DAA09FEFBFC0E7}` |
| `SCR_WheeledDamageManagerComponent` | `{141326E9FD94FE40}` |

The wheel templates (`RoadWheel`, `Sprocket`, `Idler`, `Roller`), the friction curves and the `Pivot` IDs in block A are the Leopard's own. The vanilla base defines none of them.

### F. Script API of the tracked simulation

`TrackedControllerComponent.GetSimulation()` returns the `VehicleTrackedSimulation`. Its methods in 1.8.0.13, from the generated `VehicleTrackedSimulation.c` and `VehicleBaseSimulation.c`, with what this work found. [Chapter 14](14-running-gear.md) explains the wheel getters in use.

Wheels and tracks (indices are track, then `TrackPart` index in the prefab order):

| Method | Behaviour | How known |
| --- | --- | --- |
| `TrackCount()` | 0 until the simulation is loaded, then always 2 (it does not read the prefab) | Reverse-engineered |
| `WheelCount()` | `TrackPart`s of **one** track: 13 on the Leopard | Measured (`LEOGEAR`) |
| `WheelGetPosition(track, wheel, displacement = 0)` | Wheel centre relative to the physics centre of mass. Displacement 0 = full droop. The only getter that checks its wheel index | Measured; reverse-engineered |
| `WheelHasContact(track, wheel)` | True while the suspension ray hits something | Measured |
| `WheelGetContactPosition(track, wheel)` | Contact point in world space. A stored value, also returned without contact | Measured; reverse-engineered |
| `WheelGetContactNormal`, `WheelGetContactMaterial`, `WheelGetContactEntity` | Normal, material and entity at the contact | API documentation; not used |
| `WheelGetContactLiquidState`, `WheelGetContactLiquidMaterial`, `WheelGetContactLiquidPosition` | Contact with water and other liquids | API documentation; not used |
| `WheelGetRPM(track, wheel)` | Wheel angular velocity × 60/2π. Sign of the forward speed. A road wheel reads belt speed / its `Radius`; a wheel without contact can read very different values | Measured; reverse-engineered |
| `WheelGetRadius(track, wheel)` | The template's `Radius` | Measured |
| `WheelGetRollingDrag`, `WheelSetRollingDrag` | Rolling drag percentage | API documentation; not tested |

The getters checked (`TrackCount`, `WheelCount`, `WheelGetPosition`, `WheelHasContact`, `WheelGetContactPosition`, `WheelGetContactNormal`, `WheelGetRadius`, `WheelGetRPM`, `WheelGetRollingDrag`) first test that the simulation is loaded, and return 0, false or a zero vector if it is not. No binding checks the track index, and only `WheelGetPosition` checks the wheel index: an index out of range reads foreign memory (reverse-engineered).

Drivetrain and state:

| Method | Notes |
| --- | --- |
| `GetGear()`, `SetGear(int)` | 0 = R, 1 = N, 2.. = forward gears. `SetGear` is range-checked |
| `GetClutch()`, `SetClutch(float)` | 0 = disengaged, 1 = engaged |
| `GetBrake()`, `SetBreak(float, bool hb)` | `SetBreak(x, true)` stores 6.0, so `GetBrake()` reads 6.0 for the handbrake |
| `GetThrottle()`, `SetThrottle(float)`, `IsHandbrakeOn()` | Stubs on the tracked simulation: always 0, no effect, always false |
| `GetInputs(out, out)` | **Crashes**: the binding writes through a null pointer. Never call it |
| `SetInputs(steer, throttle)` | Not tested |
| `EngineStart()`, `EngineStop()`, `EngineIsOn()` | Engine state |
| `EngineGetRPM()`, `EngineGetRPMFeedback()`, `EngineGetLoad()` | Feedback is the rpm fed back from the gearbox side (sprocket rpm × gear × `Ratio`, seen in the diag) |
| `EngineGetRPMIdle()`, `EngineGetRPMPeakTorque()`, `EngineGetRPMPeakPower()`, `EngineGetRPMMax()` | Prefab values |
| `EngineGetPeakTorque()`, `EngineGetPeakTorqueState()`, `EngineSetPeakTorqueState(float)`, and the same for `PeakPower` | Initial and current peak torque (N·m) and power (kW) |
| `GearboxGetEfficiency()`, `GearboxGetEfficiencyState()`, `GearboxSetEfficiencyState(float)`, `GearboxGearsCount()`, `GearboxForwardGearsCount()` | Gearbox values |
| `ForceEnableSimulation()` | For cinematics only, according to the API documentation |

From the base class `VehicleBaseSimulation`: `IsValid()`, `GetWorldTransform()` (internal physics space), `GetVelocity()`, `GetVelocityLocal()`, `GetAngularVelocity()`, `GetAngularVelocityLocal()`, `GetAcceleration()`, `GetAccelerationLocal()`, `GetSpeedKmh()`, `GetGForce()`, `GetGForceMagnitude()`, `HasAnyGroundContact()`, `SetSurfaceOverride()`.

The script API has **no** access to the link chain: no getter, no setter, no reset (`VehicleTrackedSimulationClass` is empty).

### G. Simulation signals for procedural animation

The tracked simulation offers these signals to `CarProcAnimComponent` (reverse-engineered from the signal-name table of the executable):

| Per | Signals |
| --- | --- |
| Wheel | `steering`, `suspension`, `rotation`, `damper`, `longslip`, `latslip` |
| Track | `trackSpeed` (km/h), `trackLongSlip`, `trackLatSlip` |

`rotation` is in degrees (minus the wheel's visual angle) and stays 0 unless the link chain exists. `suspension` is in metres along the bone's Y. The simulation never moves bones itself. In 1.8.0.13 the binding of these signals to tracked wheels is wrong ([chapter 14](14-running-gear.md)).

### H. Vehicle diag menu entries

In the Diag exe, open the diag menu with left Win + Alt in *Play*. The entries below are the ones visible in test video A (2026-10-06). [Chapter 16](16-debugging.md) explains how to use them and how to read the overlay.

The vehicles menu holds the submenus *Helicopter*, *Tracked*, *Portals*, *Vehicle Networking and Reconciliation* and *Compartments*, then these entries:

| Entry | Seen in the 2026-10-06 test |
| --- | --- |
| *Show stats* | Not tried |
| *Show vehicle debug* | Master switch of the overlay. With it on, the *Debug* panel (Steering, Clutch, Brake, Throttle, Gearbox Gear, Speed, Slope, G-Force, Y/P/R) was shown. With a link chain it did not crash |
| *Player vehicle only* | On during the test |
| *Show CoM* | Not tried |
| *Show inertia* | Shows the inertia tensor (327602.7, 395692.4, 94730.0 on the 2A6) |
| *Show forces* | Red force lines; at 62 t they leave the screen |
| *Show engine* | On during the test, with the *Engine* panel shown (Running, Load, Torque, Power, RPM, Feedback, graph) |
| *Show contacts* | Toggled without a crash |
| *Show raycast* | Toggled without a crash |
| *Show suspension* | On during the test; suspension markers were drawn at each wheel |
| *Show swaybar*, *Show wheels*, *Show bones*, *Show slope* | *Show wheels* toggled without a crash; the others not tried |
| *Test raycast* | Toggled without a crash |
| *Toggle signals* | Not tried |
| *Reset vehicle* (lctrl+lalt+r), *Teleport up* (lctrl+lalt+t) | Shortcuts shown in the menu; not tried |
| *Wheel drag set*, *Wheel drag value* | Not tried |
| *Disable freelook*, *Enable debugger trace*, *Force vehicle active*, *Show hidden properties* | *Enable debugger trace* and *Show hidden properties* toggled without a crash |
| *Wheel engine brake*, *MT simulation* | On during the test |
| *Show Controller Diags* | Adds an *Inputs* panel. Toggled without a crash with the chain present |
| *Show Compartment Events* | Not tried |
| *Show dust materials*, *Fuel consumption*, *Visualize push force*, *Collisions debug prints* | Not related to the tracked simulation; not tried |

The *Tracked* submenu holds the link chain entries. *Draw tracks*, *Track dynamic*, *Track gravity* and *Reset tracks* exist only in Diag builds (string scan of the executables):

| Entry | Value shown | Seen in the 2026-10-06 test |
| --- | --- | --- |
| *Steering mode* | wheeled | Selected; the value stayed *wheeled* |
| *TrackSim* | true | Switched off for a few seconds: nothing visible changed |
| *Draw tracks* | true | Off removes the wheel circles, the rope polyline and the per-track rope stats |
| *Track dynamic* | true | Not tried |
| *Track gravity* | true | Switched off: no visible effect from the rear camera |
| *Reset tracks* | disabled | Not tried |

### I. Addresses in the executable

For anyone who wants to check the reverse engineering. File `ArmaReforgerWorkbenchSteamDiag.exe` 1.8.0.13, base `0x140000000`.

Drivetrain, controller and network:

| Address | Function |
| --- | --- |
| `0x140ea7050` | Tracked simulation step |
| `0x140e9feb0` | Per-side ratio (divides the load) |
| `0x140ea7ac0` / `0x140ea7aec` | Multiplies and divides the reaction by the ratio |
| `0x140e85830` | Engine sync (applies the reaction to the rpm) |
| `0x140e85060` | Engine torque curve |
| `0x140e86070` | Gearbox table `[−\|Reverse\|, 0, Forward...]` |
| `0x140fe8320` | Direction selection state machine |
| `0x140fe7cf0` / `0x140fe8880` | Gear request and shift state machine |
| `0x140fe85c0` | Clutch at launch |
| `0x140fe76c0` / `0x140fee110` | Input reading and input gate |
| `0x140fe4940` | Command replay on the server (`NwkTrackedMovementComponent`) |

Loading, wheels and script bindings:

| Address | Function |
| --- | --- |
| `0x140ea4b10` | Tracked loader: exactly 2 tracks, at least 3 parts each, idler and sprocket at the same index on both sides; `Type` switch (Road Wheel 0, Sprocket 1, Idler 2, Return Roller 3) |
| `0x140ea4b00` | Validity test called first by the getter bindings checked (simulation + `0x1480` == 2) |
| `0x140fc49e0` | `TrackCount()` binding (returns the constant 2 when valid) |
| `0x140fc5490` | `WheelCount()` binding |
| `0x140fc5bf0` | `WheelGetPosition()` binding, the only one with a wheel index check |
| `0x140fc5d10` | `WheelGetRPM()` binding; the value is the angular velocity × 9.549296 |
| `0x140fc10a0` | Faulty `GetInputs()` binding (crash 1) |
| `0x140fc1270` / `0x140fc1370` | Procedural animation signal provider (wheel name lookup with a plain string compare) and value filler |
| `0x142fa44e0` | Signal-name table (`steering`, `suspension`, `rotation`, ... `trackSpeed`) |

Link chain (rope):

| Address | Function |
| --- | --- |
| `0x140fc18d0` | Registers the chain attributes after `Simulation` |
| `0x140fbc1f0` | Component constructor: builds the two ropes and spawns 2 × N link entities when there are 2 or more `TrackPositions` and `TrackLength` is above 0 |
| `0x140fc0520` | *Generate track positions* (editor action 1; dispatcher `0x140fc5390`). Seeds an ellipse, runs 1000 rope steps of 1/60 s, writes index 0 on top with the index increasing forward along the top run |
| `0x140fc4090` | Per-step chain update; returns early unless both ropes exist |
| `0x140fc4430` | Angular speed from the angle change of `TrackPart` 1; drives the visual wheel angles and the chain |
| `0x140fc13d0` | `trackSpeed` = radius of `TrackPart` 0 × ω × 3.6 |
| `0x140fbfed0` | Collects one circle per `TrackPart` (simulation radius, current position) |
| `0x140a478c0` | Rope step: fixed substep 0.0083 s, 25 constraint iterations each |
| `0x140a43a60` | Verlet integration of each point; rotates the point gripped by the sprocket |
| `0x140a40e80` | Distance constraint relaxation |
| `0x140a41760` | Collision with the wheel circles, and the snap of points under wheels that have ground contact |
| `0x140fbe7f0` | Places each link entity at the midpoint of two chain points (`EOnPostFrame`) |
| `0x140fbdf30` | Diag draw. Passes the rope pointers at component + `0x1638` / `0x1640` to `0x140a41ce0` without a null check: crash 2, only when no chain exists |

Network and friction (added 2026-10-17, see [chapter 18](18-multiplayer.md)):

| Address | Function |
| --- | --- |
| `0x14106c700` | `NwkTrackedMovementComponent` constructor: reconciliation allowances (distance |v|/300 clamped to 0.001–1.5 m, angle 0.003, linear velocity 0.3, angular velocity 0.25) |
| `0x1410743d0` | Per-step network dispatcher: driver's client produces (`0x141075190`) and is approved (`0x1410745f0`) or corrected (`0x141074770`); authority processes (`0x141074940`) and consumes (`0x14106e4d0`) |
| `0x140fe5d10` | Move recorder: steer, throttle, brake, handbrake (from the stub `IsHandbrakeOn` `0x140fc23c0`), gear, clutch, shift state |
| `0x140fe7c70` / `0x140fe4940` | Server replay of a move into the controller and the simulation; `SetBreak` binding `0x140fc3c10` clamps the brake to [0, 1] |
| `0x14104d8f0` | Server comparison of its state with the client's report |
| `0x141076250` / `0x141075cc0` | Client correction and native re-simulation of the pending moves |
| `0x141054b50` | `NwkCarMovementComponent` move producer, for comparison (36 calls against 31 in the tracked one) |
| `0x1410740b0` / `0x141053790` | Tracked / car simulation-enable virtual (bit 0x400 of +0xf4; enabling only sets the bit) |
| `0x140a448a0` / `0x140a410a0` | Road wheel contact force / fixed Magic Formula tyre; parameter table `0x142e57a00`; old friction path disabled by the byte `0x143b38fd1` |
| `0x1409dba90` / `0x140ba8310` | Camera handler update (scripted cameras only on keyframes) / character animation keyframe driver |

---

[← Vehicle debug diag and telemetry](16-debugging.md) · [Contents](../../README.md) · [Multiplayer →](18-multiplayer.md)
