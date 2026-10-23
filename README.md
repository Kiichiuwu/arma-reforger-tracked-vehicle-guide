# Arma Reforger — Native Tracked Vehicle Guide

> 🇬🇧 English · [🇧🇷 Português](README.pt-BR.md)

![Leopard 2A6 driving on the runway with the native vehicle debug overlay: wheel circles, rope chain and per-track stats drawn over the scripted wheels and links](docs/images/hero-debug-side.gif)

*Leopard 2A6 on the native tracked simulation, left side, accelerating in gear 2 from about 11 to 21 km/h (test video A, 2026-10-06, 3:06–3:11). The native vehicle debug overlay is on: white circles are the simulation's wheels at their sim radius, the red and green line is the native rope chain, and the boxes are its per-track stats. The wheels and links you see are moved by script. Model "Leopard 2 A6" by buh-late, CC BY 4.0, modified. More in [Video examples](docs/en/05-video-examples.md).*

A complete, tested guide to the **native tracked vehicle** feature of Arma Reforger
(`TrackedVehicle_Base` and `VehicleTrackedSimulation`): what it is, how it works inside,
and how to put it on your own tank, step by step.

Everything here was discovered and tested while building a Leopard 2 mod with the base game
only. The worked example is now a **Leopard 2A6**. The first drivetrain and reverse tests ran
on an earlier Leopard 2A7 build; the guide says so wherever a number still comes from it.

This edition adds **Track physics, suspension and more**: the native suspension and how to set
the ride height, the road wheels, idler, sprocket and return rollers, the native track link chain
and why it fails, a scripted running gear and link chain that work, and the native vehicle debug
overlay. Each fact is marked as measured, reverse-engineered, observed in video or not tested.

**Tested on:** Arma Reforger / Enfusion Workbench **1.8.0.13** and **1.9.0.10 Experimental** (October 2026).

## Workshop mod and model download

| | |
| --- | --- |
| **Workshop mod** | [Leopard 2A6 - Tracked Base](https://reforger.armaplatform.com/workshop/7E4ABBD41FEDAC42), A base prefab to build your own tanks on, in three drive variants: vanilla simulation (patched) and Kiichi Tracked System for 1.8 and 1.9 ([chapter 19](docs/en/19-kiichi-tracked-system.md)) |
| **Mod GUID** | `7E4ABBD41FEDAC42`. Add it to `Dependencies` in your `addon.gproj` to inherit the Leopard prefabs and scripts in your own mod |
| **Mod license** | [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/): use, modify and reupload it freely, just keep the credits |

**3D model:** [⬇ Download the modified model (FBX + textures, 32 MB)](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/releases/download/leopard2a6-model-v1/Leopard2A6_Enfusion_model_v1.zip) · [Original model on Sketchfab](https://sketchfab.com/3d-models/leopard-2-a6-7cb23d5322df4b409a880de635826067)

The modified model is the one the mod uses: split into hull, turret, periscope and track link, with the running gear bones and suspension rig, `UCX_` colliders, the `COM_hull` point, LODs and textures repacked for Enfusion. Both are CC BY 4.0 by buh-late; see [the release](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/releases/tag/leopard2a6-model-v1) for details.

## Why this exists

The base game ships a tracked simulation, but no vanilla vehicle uses it and its default
config does not even load. Published tank mods fake tracks with the wheeled simulation.
This guide documents the real thing, including its bugs and the workarounds, so the next
modder does not have to reverse-engineer it again.

## What works

| Feature | Status |
| --- | --- |
| Driving forward, automatic gears | ✅ Works. Top speed not re-measured on the 2A6: theory gives 72–75 km/h in 4th gear with final drive 4.6. The earlier 2A7 build measured 74.3 km/h |
| Steering while moving | ✅ Works |
| Pivot turn in place | ✅ Works. During a pivot the track running backwards usually turns faster, so the hull creeps backwards at up to about 4 km/h (observed in video); cause open |
| Reverse | ⚠️ Native reverse is broken in 1.8.0.13 (the engine sticks at 3766 rpm on the 2A6). Worked around with a scripted push ([why](docs/en/09-bugs.md)). ✅ Real reverse in Kiichi Tracked System |
| Suspension | ✅ Works natively. Set `MaxTravelDown ≈ m·g/(n·k)` so the hull sits at its modelled height ([chapter 13](docs/en/13-suspension.md)) |
| Return rollers | ✅ Work (`Type "Return Roller"`) |
| Road wheel, idler, sprocket and roller animation | ✅ By script (`LEO_RunningGearComponent`). `CarProcAnimComponent` ❌ gives tracked wheels another wheel's signals in 1.8.0.13 ([chapter 14](docs/en/14-running-gear.md)) |
| Track links | Native rope chain ⚠️: it sags (no ground collision), comes off the sprocket after a flip or a blast, and has no reset API. Scripted chain ✅ (`LEO_TrackChain`): it cannot derail by construction, but it has not yet been tested through a flip or a mine blast ([chapter 15](docs/en/15-track-links.md)) |
| Native vehicle debug overlay | ✅ Ran through a 5-minute test with a link chain present. Without a chain it crashed Workbench on 2026-10-02, most likely because the chain was missing ([chapter 16](docs/en/16-debugging.md)) |
| Multiplayer | Vanilla: ✅ pivot and reverse on dedicated servers with `LEO_TrackedDriveComponent` (the server drives the native simulation; the engine drops the handbrake from the network move). The native client prediction of tracked vehicles is unfinished; v6 never uses it, but a person driving on a client has not checked the result yet. Kiichi Tracked System: ✅ server authoritative, measured with the driver on a client ([chapter 18](docs/en/18-multiplayer.md), [chapter 19](docs/en/19-kiichi-tracked-system.md)) |
| 1.9.0.10 Experimental | ❌ Native simulation: inverted suspension force, parked hulls thrown away. ✅ Kiichi Tracked System works ([chapter 20](docs/en/20-experimental-1.9.md)) |

## What is new in this edition

- **Kiichi Tracked System (2026-10-23).** A tracked simulation written entirely in script
  (`LEO_TrackSimComponent`): suspension, engine, gearbox, per-belt traction, clutch-brake steering and real
  reverse on a plain rigid body, server authoritative. Same code on 1.8 and 1.9
  ([chapter 19](docs/en/19-kiichi-tracked-system.md)).
- **Vanilla, server-driven (v6).** `LEO_TrackedDriveComponent` replaces `LEO_TrackedReverseComponent`: the
  server keeps the vehicle and drives the native simulation from the driver's RPC inputs
  ([chapter 18](docs/en/18-multiplayer.md)).
- **1.9.0.10 Experimental.** What breaks in the native simulation there ([chapter 20](docs/en/20-experimental-1.9.md)).
- **Three prefabs** in the Workshop mod, side by side: vanilla, Kiichi Tracked System 1.8 and 1.9.

- **Multiplayer (2026-10-17).** A new chapter, [Multiplayer](docs/en/18-multiplayer.md): how the
  tracked vehicle is replicated, why a dedicated server never pivots (the handbrake is dropped from
  the network move), the v5 reverse script that fixes it, the driver-on-a-client limitation that it
  cannot fix, and the Workbench Peer Tool.
- **Grip.** `FrictionCurveLongitudinal/Lateral` are never read in 1.8.0.13; the V2 friction attributes
  are, and their vanilla position curve leaves the rear road wheel with no lateral grip. Retuned, with
  a lower centre of mass ([chapter 10](docs/en/10-tuning.md)).
- **New example.** A Leopard 2A6 built from the model by buh-late (CC BY 4.0). The first
  edition (Leopard 2A7, model by king_st0ne) stays in the git history under the tag
  [`v1-leopard2a7`](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/tree/v1-leopard2a7).
- **Four new chapters.** [Suspension and ride height](docs/en/13-suspension.md),
  [Road wheels, idler, sprocket and return rollers](docs/en/14-running-gear.md),
  [Track links](docs/en/15-track-links.md) and
  [Vehicle debug diag and telemetry](docs/en/16-debugging.md). The appendix moved to
  [`17-appendix.md`](docs/en/17-appendix.md).
- **Suspension, measured.** `SpringRate` is in kN/m, the spring is free at full droop, and the
  static sag is `m·g/(n·k)`. That gives the `MaxTravelDown` rule for the ride height.
- **Running gear.** Return rollers work. The first two parts of each track must be road wheels, so
  list the road wheels first. The script API reads every wheel by (track, part).
  `CarProcAnimComponent` mis-maps tracked wheels, so a script drives the bones instead.
- **Track links.** The native link chain, reverse-engineered: a 2D rope with gravity, wheel
  contact and sprocket drive, but no ground collision. This edition adds an offline
  `TrackPositions` generator and a scripted chain that replaces the rope visually.
- **Debugging.** The native vehicle debug overlay (wheel circles, rope, engine panel) and the
  `LEOSUSP` suspension telemetry.
- **Corrections and fixes.** `CenterOfMass` is an offset from the model's own centre of mass, not
  a point near the hull floor. The debug overlay crash most likely came from a missing link chain:
  with a chain it ran through a 5-minute test without a crash. The reverse script now also stops
  the engine when the driver gets out.
- **New media.** GIFs and stills from the 2A6 test videos with the debug overlay on, plus clips
  from before the fixes ([Video examples](docs/en/05-video-examples.md)).

## Contents

| # | Chapter |
| --- | --- |
| 0 | [About this guide](docs/en/00-about.md) |
| 1 | [Overview](docs/en/01-overview.md) |
| 2 | [Architecture](docs/en/02-architecture.md) |
| 3 | [3D model requirements](docs/en/03-model-requirements.md) |
| 4 | [Step by step from scratch](docs/en/04-from-scratch.md) |
| 5 | [Video examples](docs/en/05-video-examples.md) |
| 6 | [Converting an existing vehicle](docs/en/06-converting.md) |
| 7 | [Attribute reference](docs/en/07-attribute-reference.md) |
| 8 | [How it works inside](docs/en/08-internals.md) |
| 9 | [Bugs and limitations (1.8.0.13)](docs/en/09-bugs.md) |
| 10 | [Tuning with real-world data](docs/en/10-tuning.md) |
| 11 | [Ready-made scripts](docs/en/11-scripts.md) |
| 12 | [Troubleshooting](docs/en/12-troubleshooting.md) |
| 13 | [Suspension and ride height](docs/en/13-suspension.md) |
| 14 | [Road wheels, idler, sprocket and return rollers](docs/en/14-running-gear.md) |
| 15 | [Track links: the native rope chain and the scripted chain](docs/en/15-track-links.md) |
| 16 | [Vehicle debug diag and telemetry](docs/en/16-debugging.md) |
| 17 | [Appendix](docs/en/17-appendix.md) |
| 18 | [Multiplayer](docs/en/18-multiplayer.md) |
| 19 | [Kiichi Tracked System](docs/en/19-kiichi-tracked-system.md) *(new)* |
| 20 | [1.9.0.10 Experimental](docs/en/20-experimental-1.9.md) *(new)* |

## Quick start

1. Make your hull prefab inherit `{0608D8FA71FD3433}Prefabs/Vehicles/Core/TrackedVehicle_Base.et`.
2. Fill in the `Simulation Tracked` block: friction curves, wheel templates (`RoadWheel`, `Idler`,
   `Sprocket`, `Roller`) and exactly two tracks. In each track, list the road wheels first (front to
   rear), then the idler, the sprocket and the return rollers, in the same order on both sides
   ([chapter 4](docs/en/04-from-scratch.md); a complete working block is in
   [examples/leopard2a6](examples/leopard2a6/VehicleTrackedSimulation_Leopard2A6.et.txt)).
3. Size the suspension so the hull sits at its modelled height: `MaxTravelDown ≈ m·g/(n·k)`,
   with `n` road wheels and `k` = `SpringRate` × 1000 N/m. Leopard: 62300 × 9.81 / (14 × 450000)
   = 0.097 m, so `MaxTravelDown 0.10` ([chapter 13](docs/en/13-suspension.md)).
4. Add [`LEO_TrackedDriveComponent`](scripts/LEO_TrackedDriveComponent.c) (with
   [`LEO_PlayerController.c`](scripts/LEO_PlayerController.c)) so reverse drives and multiplayer works,
   and set `ThrottleReverseTarget 1` on the controller. Or skip steps 2–4 and use
   [Kiichi Tracked System](docs/en/19-kiichi-tracked-system.md) instead of the native simulation.
5. For moving wheels and links: rig the wheel bones, leave `CarProcAnimComponent` empty, and add
   [`LEO_RunningGearComponent`](scripts/LEO_RunningGearComponent.c) (it needs
   [`LEO_TrackChain.c`](scripts/LEO_TrackChain.c)) and
   [`LEO_TrackBeltComponent`](scripts/LEO_TrackBeltComponent.c)
   ([chapter 14](docs/en/14-running-gear.md)).
6. Give the simulation a link chain too: `TrackSegment` (your link model), `TrackPositions` (from
   [`tools/trackpositions.py`](tools/trackpositions.py)), `TrackLength` and `TrackOffset1/2`. The
   debug overlay most likely needs it (it crashed without one), and the running gear script has only
   been tested with a chain present ([chapter 15](docs/en/15-track-links.md)).
7. After every prefab edit, close the world, reload the scripts with no world open, and open the
   world again. A vehicle in a world that is already open keeps its old simulation values
   ([chapter 13](docs/en/13-suspension.md#prefab-edits-and-the-open-world)).

## Repository layout

```
docs/en/              English chapters (00-20)
docs/pt-BR/           Portuguese chapters (same file names)
docs/images/          GIFs and stills from the Leopard 2A6 test videos
scripts/              Enforce Script components (see the table below)
tools/                trackpositions.py: offline generator for TrackPositions
examples/leopard2a6/  The Leopard 2A6 prefab blocks: simulation with link chain, hull components, the three prefabs
```

| File | What it does |
| --- | --- |
| [`scripts/LEO_TrackSimComponent.c`](scripts/LEO_TrackSimComponent.c) | Kiichi Tracked System: the whole tracked simulation in script, server authoritative |
| [`scripts/LEO_TrackedDriveComponent.c`](scripts/LEO_TrackedDriveComponent.c) | Vanilla simulation driven by the server: RPC inputs, real handbrake, own gear shifting, scripted reverse |
| [`scripts/LEO_PlayerController.c`](scripts/LEO_PlayerController.c) | Carries the driver's inputs to the server (needed by both drive components) |
| [`scripts/LEO_TelemetryComponent.c`](scripts/LEO_TelemetryComponent.c) | `LEOTEL` drivetrain lines and `LEOSUSP` suspension lines in `console.log` |
| [`scripts/LEO_RunningGearComponent.c`](scripts/LEO_RunningGearComponent.c) | Road wheel travel and spin, idler, sprocket and roller spin; owns the scripted link chains and hides the native rope's links |
| [`scripts/LEO_TrackChain.c`](scripts/LEO_TrackChain.c) | One track's scripted link chain (a class used by `LEO_RunningGearComponent`) |
| [`scripts/LEO_TrackBeltComponent.c`](scripts/LEO_TrackBeltComponent.c) | Hides the hull's static track belt meshes once link entities exist |
| [`tools/trackpositions.py`](tools/trackpositions.py) | Builds `TrackPositions` from the wheel circles (convex hull, resampled by arc length) |

## Credits

- **3D model:** ["Leopard 2 A6"](https://sketchfab.com/3d-models/leopard-2-a6-7cb23d5322df4b409a880de635826067)
  by [buh-late](https://sketchfab.com/buh-late), licensed under
  [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/), published 2024-01-17. Modified for
  Enfusion (split into parts, re-parented, bones and suspension rig, colliders, LODs, repacked
  textures). The author does not endorse this project. See [CREDITS.md](CREDITS.md).
- **Previous example model** (first edition, tag `v1-leopard2a7` only): "Leopard 2 A7" by
  king_st0ne, licensed under [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/).
- **Guide, research and scripts:** Kiichi.

## License

| What | License |
| --- | --- |
| Documentation and images (`docs/`, READMEs, `CREDITS.md`) | [CC BY-SA 4.0](LICENSE-docs) |
| Code (`scripts/`, `tools/`, prefab blocks and code snippets in `examples/`) | [MIT](LICENSE) |
| Leopard 2A6 3D model, shown in the images and used by the example mod | [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/), credit buh-late, modified |
| Workshop mod and the modified model download | [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/), credit buh-late (model) and Kiichi |
| Leopard 2A7 3D model shown in the first edition's images (tag `v1-leopard2a7` only) | [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/), credit king_st0ne |

Arma Reforger and Enfusion are trademarks of Bohemia Interactive a.s. This project is not
affiliated with or endorsed by Bohemia Interactive.

## Contributing

Issues and pull requests are welcome, especially results from newer game versions and
multiplayer tests. Please keep the English and Portuguese chapters in sync.
