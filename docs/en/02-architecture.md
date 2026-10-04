> [🇧🇷 Português](../pt-BR/02-architecture.md) · 🇬🇧 English

[← Overview](01-overview.md) · [Contents](../../README.md) · [3D model requirements →](03-model-requirements.md)

# Architecture

```mermaid
flowchart TD
  IN["Driver inputs<br/>TrackedThrottle, TrackedBrake, Steering L/R<br/>Turbo, HandBrake, EngineStart/Stop"]
  NWK["NwkTrackedMovementComponent<br/>sends the commands to the server,<br/>which replays them (multiplayer)"]
  CTL["SCR_TrackedControllerComponent<br/>W = 80% throttle; turbo = 100%<br/>S at a standstill engages reverse by itself<br/>picks gear, clutch and shifts"]
  REV["LEO_TrackedReverseComponent (script)<br/>workaround for the broken reverse:<br/>S at a standstill pushes the hull backward"]
  subgraph SIM["VehicleTrackedSimulation › Simulation Tracked"]
    ENG["Engine<br/>torque curve, zero at RpmMax"] --> CLU["Clutch<br/>factor from 0 to 1"]
    CLU --> GBX["Gearbox<br/>Forward + one reverse<br/>reverse always negative (bug)"]
    GBX --> DIF["TripleDifferential<br/>Ratio and steering ratios"]
    DIF --> TRK["Tracks<br/>Left (0) and Right (1)"]
    TRK --> WHL["RoadWheel · Sprocket · Idler<br/>ray from the bone to the ground + suspension"]
  end
  RB["RigidBody<br/>the hull receives the wheel forces"]
  IN --> CTL
  CTL --> ENG
  IN --> NWK
  IN --> REV
  WHL --> RB
  REV --> RB
  classDef bug stroke:#d33,stroke-width:3px
  classDef fix stroke:#36c,stroke-width:3px
  class GBX bug
  class REV fix
```

*Red border: the native reverse bug. Blue border: the script workaround.*

The driver never talks to the simulation directly. The controller reads the keys and decides throttle, brake, gear and clutch. The simulation computes the engine, drivetrain, tracks and wheels, and applies the forces to the hull.

Where each part lives in the prefab:

| Component | Where it lives | Role |
| --- | --- | --- |
| `VehicleTrackedSimulation` | Hull `components` | Track physics: engine, drivetrain, tracks, wheels |
| `SCR_TrackedControllerComponent` | Inside `BaseVehicleNodeComponent` | Turns the keys into pedals, gear and clutch |
| `NwkTrackedMovementComponent` | Hull `components` | Replicates movement in multiplayer |
| `RigidBody` | Hull `components` | Mass, center of mass and collision |
| `SCR_WheeledDamageManagerComponent` | Hull `components` | Damage: hull, engine, gearbox, wheels |
| `SignalsSourceAccess` | Inside `VehicleTrackedSimulation` (comes from the `.ct`) | Links the simulation to the `engineRPM`, `engineThrust`, etc. signals |

All of them come ready-made in `TrackedVehicle_Base.et`. What is left is to fill in the simulation, as the [step-by-step guide](04-from-scratch.md) shows.

---

[← Overview](01-overview.md) · [Contents](../../README.md) · [3D model requirements →](03-model-requirements.md)
