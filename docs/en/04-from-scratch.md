> [🇧🇷 Português](../pt-BR/04-from-scratch.md) · 🇬🇧 English

[← 3D model requirements](03-model-requirements.md) · [Contents](../../README.md) · [Video examples →](05-video-examples.md)

# Step by step from scratch

This walkthrough takes a new model to a tank that drives, turns and reverses, with a working suspension, moving wheels and track links. Unless a step says otherwise, everything was tested on the Leopard 2A6 in Workbench 1.8.0.13, with the base game only. Where a number was only measured on the earlier Leopard 2A7 build, the text says so.

Steps 1–9 give a tank that drives. Steps 10–13 add the ride height, the running gear animation, the track links and the debug tools. Chapters 13–16 explain each of these in depth.

The complete 2A6 blocks are in [examples/leopard2a6](../../examples/leopard2a6/): the whole `VehicleTrackedSimulation` block and the hull components with their IDs.

### 1. Create the mod project

1. In Workbench, create a new project that depends **only** on the base game (`ArmaReforger`, GUID `58D0FB3206B6F859`).
2. Create the folders `Assets/Vehicles/Tracked/<Name>/` and `Prefabs/Vehicles/Tracked/<Name>/`.

### 2. Import the model

1. Copy the hull `.fbx` into `Assets/...` and use *Register and import* → *as Model*.
2. Set *Merge Meshes* (off), *Export Skinning* and *Export Scene Hierarchy* (on). Reimport.
3. Replace the physics material of each `UCX_*` collider with an `armor_XXmm.gamemat`.
4. Open the `.xob` and check that every part bone appears in the bone list: road wheels, suspension bones, idlers, sprockets and return rollers ([3D model requirements](03-model-requirements.md)).
5. Import the track link model the same way. It has no colliders. You need it in step 12.

### 3. Create the hull prefab

The prefab inherits from `TrackedVehicle_Base.et`, which already includes the controller, the networking and an empty simulation. Create a `.et` text file and register it in the Resource Browser (right-click → *Register*).

```
Vehicle : "{0608D8FA71FD3433}Prefabs/Vehicles/Core/TrackedVehicle_Base.et" {
 components {
  MeshObject "{51DAA09FEFBFC0E7}" {
   Object "{XOB_GUID}Assets/Vehicles/Tracked/<Name>/<Name>_hull.xob"
  }
  RigidBody "{51DAA09FECF52BBF}" {
   Mass 62300
   CenterOfMass 0 1 0.46
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

The `RigidBody` values are the 2A6's. `CenterOfMass` depends on your model: see step 6.

**ID rule:** the text in braces after the class name is the instance ID. To **override** an inherited component, repeat the vanilla ID, as above. A new ID creates a second component.

**Do not repeat the `.ct` inheritance.** `TrackedVehicle_Base.et` already declares `VehicleTrackedSimulation_Base.ct`. In the child, write only `VehicleTrackedSimulation "{6160DE6434513C4D}" {`.

### 4. Fill in the simulation

The vanilla `VehicleTrackedSimulation_Base.ct` is incomplete. It only connects engine → clutch → gearbox and sets friction and aerodynamics. Four things are missing, and without them the log shows `Tracked - failed to load!`:

| Missing | What to write | Error if missing |
| --- | --- | --- |
| Friction curves | `FrictionCurveLongitudinal` and `FrictionCurveLateral` | `FrictionCurveLateral - configuration is missing!` |
| Wheel templates | `RoadWheel` (with `Suspension`), `Sprocket`, `Idler`, and `Roller` if you have return rollers | `TrackWheel's mass / radius / brake torque is incorrect!` |
| Tracks | `Tracks { Track Left {...} Track Right {...} }` | `Invalid number of tracks! Must be 2!` |
| Valid engine | `Inertia`, `Steepness`, `Friction` greater than zero | `Engine's inertia is incorrect! (must be greater than zero)` |

Paste this inside `Simulation Tracked "{6160DE65F2933990}" { }`. These are the current Leopard 2A6 values:

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
Aerodynamics Aerodynamics "{6160DE6521A7B70A}" {
 ReferenceArea 9.5
 DragCoefficient 0.9
}
```

The `RoadWheel`, `Sprocket`, `Idler` and `Roller` templates are **one of each**, shared by all parts of that type. There is no per-wheel radius. Leave out `Roller` if your tank has no return rollers.

What the values mean for your tank:

| Value | Rule | Leopard 2A6 |
| --- | --- | --- |
| `Radius` of every template | Distance from the part's bone to the ground face of the links around it, not the rim ([chapter 3](03-model-requirements.md#what-a-part-bone-means-to-the-simulation)) | Road wheel 0.418, sprocket 0.34, idler 0.335, roller 0.152 |
| `Sprocket` `Radius` and `Ratio` | The sprocket radius sets the speed; choose `Ratio` for your top speed ([Tuning](10-tuning.md)) | `Ratio` 4.6 with 0.34: in theory 72–75 km/h in 4th at 2600–2700 rpm. Not measured on the 2A6; the earlier 2A7 build (`Ratio` 4.9, sprocket 0.355) measured 74.3 km/h |
| `Suspension` | Step 10 | `MaxTravelDown` 0.10 sets the ride height |
| `RetarderBaseTorque`, `RetarderThrottleSpeed` | With the defaults the retarder caps the speed ([Attribute reference](07-attribute-reference.md)) | 0 and 20 |

### 5. Declare the tracks

Still inside `Simulation Tracked`, list the parts of each track in this order: **the road wheels from front to rear first**, then the idler, the drive sprocket and the return rollers. The first track is the left one (index 0); the second is the right one (index 1).

```
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
  }
 }
}
```

The example shows 2 of the 7 road wheels and 1 of the 4 return rollers per side, to fit on the page. The full 2A6 list is in [VehicleTrackedSimulation_Leopard2A6.et.txt](../../examples/leopard2a6/VehicleTrackedSimulation_Leopard2A6.et.txt): `Wheel_X01`…`X07` (parts 0–6), `Idler_X` (7), `Sprocket_X` (8), `Roller_X01`…`X04` (9–12). Each `Pivot` needs its own ID (any 16 hexadecimal digits, with no repeats).

**Why this order:**

- **The native link chain reads parts 0 and 1.** Its speed is the angular velocity of part 1 times the radius of part 0 (reverse-engineered from the 1.8.0.13 executable). Both must be road wheels. With an idler first, the chain would run at idler radius / road wheel radius of the track speed: about 63% on an early 2A6 setup.
- **The scripts use the same indices.** The script API addresses a part as (track, position in this list), and `WheelCount()` returns the parts of one track (13 on the 2A6). `LEO_RunningGearComponent` expects road wheels at 0…n−1, the idler at n, the sprocket at n+1 and the rollers after it.
- **Both sides must match.** The loader wants the idler and sprocket at the same position on both tracks, and the simulation keeps a single part count for both (reverse-engineered). Use the same list on both sides.
- The driving physics worked with both orders: the earlier 2A7 build listed the idler first.

**Types:**

- **Road wheels take no `Type`.** The default is `Road Wheel`, so you avoid writing a name with a space.
- **`Type Idler`** and **`Type Sprocket`** go without quotes.
- **Return rollers: `Type "Return Roller"`**, with quotes. The quoted text value is accepted. Add the `Roller` template from step 4. Tested on the 2A6 with 4 rollers per side; their effect on driving was not measured.

### 6. Colliders, mass and centre of mass

- The `RigidBody` needs colliders in the `.xob`; without them the log shows `Tracked - no physics component!`.
- `Mass` is in kg. The 2A6 uses 62300.
- **`CenterOfMass` is an offset, not a position.** The executable describes it as "Offset of center of mass. Relative to one defined in model (if applicable)." The 2A6 model has a centre of mass point, `COM_hull`, at (0, 1.168, −0.381) in hull space. `0 1 0.46` therefore puts the centre of mass at (0, 2.168, 0.079), which was measured through the wheel API ([chapter 13](13-suspension.md#centre-of-mass)). For a new model with a `COM_` object, start from `0 0 0`.

**Level the hull with the z value.** On the springs, the fore-aft position of the centre of mass against the road wheels sets the static pitch. With *z̄* the mean z of the road wheel bones and *k* = `SpringRate` × 1000 N/m, the pitch (nose up positive, radians) is:

```math
\theta = \frac{M \cdot g \cdot (\bar z - z_{\text{COM}})}{k \sum_i (z_i - \bar z)^2}
```

The sum runs over all road wheels of both tracks. In practice: put the centre of mass z over the mean road wheel z.

| `CenterOfMass` | Centre of mass z | Pitch (model) | Pitch (measured) |
| --- | --- | --- | --- |
| `0 1 0` | −0.381 m | +0.90° | +0.954° (nose up) |
| `0 1 0.46` (2A6) | +0.079 m | −0.047° | −0.049° |
| `0 1 0.437` | +0.056 m = *z̄* | 0° | not tested |

That is about 2.1° per metre of z on the 2A6 (2.06°/m from the model, 2.18°/m from the two measured points). To measure, add `LEO_TelemetryComponent` (step 9): it logs `LEOTEL pose pitch=… roll=…` every 0.5 s, even with the engine off, pitch positive nose up. Park on flat ground, read the pitch, turn the tank 180° and read it again. The 2A6 read +0.954° both ways, which rules out the terrain.

**Height: open.** The 2A6 keeps y = 1 from earlier builds. With its `COM_hull` that puts the centre of mass 2.17 m above the ground, about 0.47 m above the hull roof. Half the track gauge over that height gives about 0.61 g, against about 1.14 g with y = 0 (computed). The 2A6 flipped once at 33–41 km/h; the high centre of mass is a computed risk factor, not a proven cause. A lower value has not been tested yet.

The 2A6 also sets `LinearSleepingThreashold 0.4` and `AngularSleepingThreashold 0.5` on the `RigidBody` (spelled as in the engine). A stationary tank goes to sleep; telemetry then repeats the same values.

### 7. Driver seat

To test, someone has to drive. The `PilotCompartmentSlot` already comes from `TrackedVehicle_Base.et`; set up the driver's entry as for any other vehicle in the game.

### 8. Test

1. Create a test world as a sub-scene of a vanilla map (for example `GM_Arland`) and place the prefab.
2. **After every prefab edit, reload the world.** A vehicle in a world that is already open keeps the simulation values it was loaded with. On the 2A6, a run still behaved like `MaxTravelDown 0.15` after the prefab had been changed to 0.10. Do this:
   1. Save the prefab and close the world in the World Editor.
   2. Reload the scripts with no world open. The Diag build fired a `Resources are leaking! Check log!` assert after every script reload in the 2026-10-05 log (7 of 7), also when no world was open. Workbench carried on after *Ignore*.
   3. Reopen the world: double-click it in the Resource Browser.
3. Press *Play*, get in as the driver and start the engine.
4. Check each function:

| Command | Expected result |
| --- | --- |
| W | Accelerates at 80% throttle (`ThrottleTurbo` 0.2 reserves the other 20%). 2A6: 20 km/h in 4.1 s, 30 km/h in 9.7 s (one logged start) |
| W + Shift (turbo) | 100% throttle |
| S while moving | Brakes |
| S at a standstill | Engages reverse by itself (there is no reverse key). Native reverse does not move: see the [bugs section](09-bugs.md) and step 9 |
| A / D while moving | Turns, with the outer track faster |
| A / D at a standstill | Turns in place (pivot / neutral steering). The 2A6 hull crept backwards at 0.2–3 km/h while pivoting (measured; cause open) |
| Leave the driver seat with the engine on | The controller applies the handbrake and selects reverse; the reverse bug then holds the engine above `RpmMax`. Step 9 fixes this |

5. Open the *Log Console* and look for lines with `Tracked`, `Wheels`, `Engine` or `TrackWheel`. The [Troubleshooting](12-troubleshooting.md) section explains each error.
6. Compare with the expected result in [Video examples](05-video-examples.md).

### 9. Make reverse drive (and install the scripts)

Native reverse cannot drive in 1.8.0.13 ([bugs section](09-bugs.md)). `LEO_TrackedDriveComponent` pushes the hull backwards while the driver holds S, and with `m_bStopEngineOnExit` (default 1) it stops the engine when the driver seat becomes empty. It also drives the whole vehicle from the server in multiplayer ([chapter 18](18-multiplayer.md)), so it needs `LEO_PlayerController.c`. Keep `ThrottleReverseTarget 1` on the controller. (Before 2026-10-20 this was `LEO_TrackedReverseComponent` v5; the measurements in this chapter were taken with it.)

All the scripts of this guide install the same way ([Ready-made scripts](11-scripts.md)):

1. Save the files from [scripts/](../../scripts/) in `scripts/Game/Vehicle/` inside your mod. `LEO_TrackChain.c` is a plain class that `LEO_RunningGearComponent` uses; install it with it.
2. Recompile the scripts with *Play* stopped and no world open.
3. Add the components to the hull's `components { }` block. The IDs below are the 2A6's ([HullComponents_Leopard2A6.et.txt](../../examples/leopard2a6/HullComponents_Leopard2A6.et.txt)):

```
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

| Component | Step | Does |
| --- | --- | --- |
| `LEO_TrackedDriveComponent` | 9 | Reverse workaround, server-side driving; stops the engine when the driver leaves |
| `LEO_TelemetryComponent` | 6, 10, 13 | `LEOTEL` and `LEOSUSP` lines in the log |
| `CarProcAnimComponent` (empty) | 11 | Keeps the inherited component without wheel parameters |
| `LEO_RunningGearComponent` | 11, 12 | Suspension and spin of the part bones, scripted link chain |
| `LEO_TrackBeltComponent` | 12 | Hides the static belt meshes once links exist |

Change the `LEO_` prefix if you use them in another mod, so they do not collide with the Leopard. The attribute defaults are the 2A6's; the next steps say which ones to change.

### 10. Size the suspension and check the ride height

The spring is free (zero force) at full droop, and `SpringRate` is in N/mm, that is kN/m (executable description, confirmed by two runs on the 2A6). At rest each road wheel is compressed from full droop by:

```math
s = \frac{M \cdot g}{n \cdot k}
```

with *n* the number of road wheels on both tracks and *k* in N/m. 2A6: 62300 · 9.81 / (14 · 450 000) = 0.097 m. **Set `MaxTravelDown` ≈ *s*** and the tank sits at its modelled height:

| `MaxTravelDown` | Rest travel predicted | Rest travel measured on the 2A6 |
| --- | --- | --- |
| 0.15 | −53 mm (hull 5 cm high) | −53.0 mm (mean of the 7 wheels, 13 of 14 stationary samples) |
| 0.10 | −3 mm | −3.0 mm (median); −1 to −5 mm per wheel from front to rear |

Negative travel means the wheel hangs below its modelled position, so the hull rides high. `MaxTravelUp` (0.30 on the 2A6) is the room above the modelled position. The dampers are in N·s/m; [chapter 13](13-suspension.md#sizing-the-suspension) shows how to choose them.

Check it in Play: with the engine on, `LEOSUSP` prints every 2 s the `travel_mm` of each road wheel, measured from `m_fWheelRestHeight` of `LEO_TelemetryComponent` (0.4186 on the 2A6; set it to your road wheel centre height). At rest on flat ground the mean of a track should be near 0. Remember the world reload from step 8: the run that still showed −51 to −54 mm after the change had not reloaded the world.

Details: [Suspension and ride height](13-suspension.md).

### 11. Animate the running gear

The simulation computes the travel and spin of every part, but `CarProcAnimComponent` does not deliver them to the right bones on a tracked vehicle in 1.8.0.13. On the 2A6, most wheel bones got another wheel's signals (measured with the `LEOSUSP` telemetry in two runs, and seen in video). An interleaved parameter order did not fix it in the second run, but that run had not reloaded the world, so it was not a clean test. So:

- **Do not add wheel `ProcAnimParams`.** `TrackedVehicle_Base` brings an empty `CarProcAnimComponent "{5D6501223785D009}"`. Leave it empty. The 2A6 writes an empty override because it had parameters before.
- **Add `LEO_RunningGearComponent`** (step 9). Every frame it reads each part from the simulation by (track, part) and sets the bones with `Animation.SetBoneMatrix`: the travel on `v_suspension_<s>0<k>`, the spin on the road wheels, idler, sprocket and rollers.

It needs the bone rig and the names from [chapter 3](03-model-requirements.md#suspension-rig-for-the-road-wheels), the part order from step 5, the idler at the front and the sprocket at the rear. Set these attributes for your tank:

| Attribute | 2A6 | Set to |
| --- | --- | --- |
| `m_iRoadWheels` | 7 | Road wheels per track |
| `m_iRollers` | 4 | Return rollers per track |
| `m_fRoadWheelRadius` | 0.345 | Visual rim radius of a road wheel, where it rolls on the inner face of the track |
| `m_fRollerRadius` | 0.0847 | Visual rim radius of a return roller |
| `m_fRestHeight` | 0.4186 | Road wheel centre height at rest |
| `m_fMaxTravelDown` / `m_fMaxTravelUp` | 0.10 / 0.30 | The same values as the `Suspension` template |
| `m_fDropRate` | 8 | How fast a wheel without ground contact drops to full droop and comes back (1/s) |
| `m_fMaxDistance` | 300 | Beyond this distance from the camera the bones are not updated (m) |

Check it:

- The log shows `LEOGEAR simulation: 2 tracks, WheelCount 13` once the simulation has loaded (the count is per track).
- In `LEOSUSP`, `diff_mm` (bone minus simulation) stays near 0 and the 7 road wheels of a track spin together. 2A6, two sessions with the published script: `worst_mm` median 0 mm, 90–95% of samples within 3 mm; larger values only while a wheel eases back after losing contact. An earlier version, which smoothed every frame, had only 70% within 3 mm.

Confirmed in game on the 2A6. The component does nothing on a dedicated server and was not tested on other players' machines (multiplayer proxies). Details: [Road wheels, idler, sprocket and return rollers](14-running-gear.md).

### 12. Add the track links

Two ways to show moving links. Both use the link model from [chapter 3](03-model-requirements.md#track-link-model) and the sim radii as the path:

|  | Native rope chain | Scripted chain (2A6) |
| --- | --- | --- |
| What moves the links | The engine: a 2D rope with gravity, the wheel circles and the sprocket drive | `LEO_TrackChain`: a path rebuilt every frame around the wheel circles at their current travel |
| Ground collision | None | Not needed: the links follow the wheels |
| Known problems | Sags where it has slack; came off the sprocket after a flip and after a mine blast; no script API to reset it | Cannot come off by construction; not yet tested through a flip or a mine blast |
| Needs | The attributes below | The attributes below, `LEO_RunningGearComponent` with `m_bScriptedChain 1`, and `LEO_TrackBeltComponent` |

The 2A6 keeps the native attributes with the scripted chain. The rope keeps running under its hidden links, and the vehicle debug diag (step 13) needs a chain. Running the scripted chain without the native attributes was not tested.

**Native chain attributes.** Write them inside `VehicleTrackedSimulation "{6160DE6434513C4D}"`, after the closing brace of `Simulation Tracked`. They are siblings of `Simulation`, not part of it:

```
TrackSegments 73
TrackPositions {
 <73 "z y" pairs from tools/trackpositions.py>
}
TrackLength 0.2110
TrackThickness 0.076
TrackOffset1 -1.3281 0 0
TrackOffset2 1.3281 0 0
TrackSegment "{B50AEDC702735F50}Assets/Vehicles/Tracked/Leopard2A6/Leopard2A6_track_link.xob"
```

| Attribute | What to put | Leopard 2A6 |
| --- | --- | --- |
| `TrackSegments` | Links per track. Only the editor's generator reads it; at runtime the number of links is the number of `TrackPositions` entries. Keep them equal | 73 |
| `TrackPositions` | A flat list of `z y` pairs in hull space: chain points around the wheel circles, index 0 on the top run, increasing forward | 73 pairs |
| `TrackLength` | Rest distance between neighbouring points, about the link pitch. Chain length = N × `TrackLength` | 0.2110 (pitch 0.2101 + 0.9 mm) |
| `TrackThickness` | Stored but never read in 1.8.0.13 (reverse-engineered) | 0.076 |
| `TrackOffset1` / `TrackOffset2` | Lateral position of the belt centre of track 0 (left) and track 1 (right) | −1.3281 / 1.3281 |
| `TrackSegment` | The link `.xob` | `Leopard2A6_track_link.xob` |

The engine builds the chain only with at least 2 `TrackPositions` and `TrackLength` > 0 (reverse-engineered).

- **Positions.** The editor action *Generate track positions* was never found in the Workbench UI during the 2A6 work. Use [tools/trackpositions.py](../../tools/trackpositions.py) instead. Edit its circle list (z, y and sim `Radius` of every part of one side), run `python trackpositions.py 73 0.2101` and paste the `TrackPositions { }` block it prints (the `#` lines above it are only information). It spaces N points evenly around the convex outline of the circles.
- **Number of links.** Path length around the sim circles / link pitch, rounded up. 2A6: 15.284 m / 0.2101 m = 72.7 → 73. The model's own belt has 71 links, because its pin line is a shorter loop.
- **Slack.** The rope has no ground collision, so slack hangs down; with too little slack, drooping wheels most likely pull it off (inferred). With 73 × 0.2101 (5.3 cm over the rest path, computed along the arcs) the 2A6 rope came off the sprocket after a flip; with 73 × 0.2110 (11.9 cm over) it hung below the wheels. [Chapter 15](15-track-links.md) has the numbers.

**Scripted chain.** Keep `m_bScriptedChain 1` on `LEO_RunningGearComponent` and add `LEO_TrackBeltComponent` (step 9). Set:

| Attribute | Component | 2A6 | Set to |
| --- | --- | --- | --- |
| `m_bScriptedChain` | `LEO_RunningGearComponent` | 1 | 1: scripted links, and the native links are hidden |
| `m_sLinkModel` | `LEO_RunningGearComponent` | `Leopard2A6_track_link.xob` | Your link `.xob` |
| `m_iLinks` | `LEO_RunningGearComponent` | 73 | Links per track |
| `m_sNativeLinkName` | `LEO_RunningGearComponent` | `Leopard2A6_track_link` | Part of your link model's name |
| `m_sStaticBeltMeshes` | `LEO_TrackBeltComponent` | `track_l track_r` | Names of your static belt meshes |
| `m_sLinkModelName` | `LEO_TrackBeltComponent` | `Leopard2A6_track_link` | Part of your link model's name |
| `m_fTimeout` | `LEO_TrackBeltComponent` | 5 | Seconds to wait for links before keeping the static belts |

Check it: the log shows `LEOTRACK link chain found (292 links): 2 static belt meshes hidden` (146 native + 146 scripted links on the 2A6). The belt path assumes the idler at the front and the sprocket at the rear; for a front sprocket see [Converting an existing vehicle](06-converting.md#4-pick-the-idler-and-the-drive-sprocket-and-order-the-parts).

Details: [Track links: the native rope chain and the scripted chain](15-track-links.md).

### 13. Check with the vehicle debug diag and telemetry

The Diag executable (`ArmaReforgerWorkbenchSteamDiag.exe`) has a native vehicle debug overlay:

1. In *Play*, open the diag menu with **left Win + Alt**.
2. Under *Vehicles*, set *Show vehicle debug* and *Player vehicle only*. They are in the Vehicles list itself, not in its *Tracked* submenu (observed in the 2026-10-06 video). The 2A6 videos also had *Show engine*, *Show suspension*, *Wheel engine brake* and *MT simulation* on.
3. The overlay draws a white circle at every part's sim `Radius`, the native rope as a polyline, a stats box per track, and *Debug* and *Engine* panels.

**Set up the link chain first.** On 2026-10-02, with no chain attributes in the prefab, turning on *Show vehicle debug* / *Show Controller Diags* (the diary names both) crashed Workbench; the overlay reads the chain without checking that it exists (reverse-engineered). With the chain present it ran through the whole 5-minute test video of 2026-10-06 without a crash.

Quick checks:

- The road wheel circles reach the ground face of the links. If not, `Radius` is wrong.
- In each track's stats box, `Speed` / `AngVel` equals the sprocket `Radius` (0.3400 on the 2A6).
- In `console.log` (`Documents\My Games\ArmaReforgerWorkbench\logs\logs_<date>\`): `LEOTEL` (speed, rpm, gear, clutch, brake, pose), `LEOSUSP` (suspension and spin), `LEOGEAR` and `LEOTRACK` (setup messages).

Details: [Vehicle debug diag and telemetry](16-debugging.md).

---

[← 3D model requirements](03-model-requirements.md) · [Contents](../../README.md) · [Video examples →](05-video-examples.md)
