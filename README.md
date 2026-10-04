# Arma Reforger — Native Tracked Vehicle Guide

> 🇬🇧 English · [🇧🇷 Português](README.pt-BR.md)

![Leopard 2A7 pivot turn with the native tracked simulation](docs/images/pivot-in-place.gif)

*Pivot turn in place with the native tracked simulation (Leopard 2A7 test, untextured model). More in [Video examples](docs/en/05-video-examples.md).*

A complete, tested guide to the **native tracked vehicle** feature of Arma Reforger
(`TrackedVehicle_Base` and `VehicleTrackedSimulation`): what it is, how it works inside,
and how to put it on your own tank, step by step.

Everything here was discovered and tested while building a **Leopard 2A7** mod with the
base game only. That mod is the worked example throughout the guide, and it will be added
to this repository as a complete example later.

**Tested on:** Arma Reforger / Enfusion Workbench **1.8.0.13** (October 2026).

## Why this exists

The base game ships a tracked simulation, but no vanilla vehicle uses it and its default
config does not even load. Published tank mods fake tracks with the wheeled simulation.
This guide documents the real thing, including its bugs and the workarounds, so the next
modder does not have to reverse-engineer it again.

## What works

| Feature | Status |
| --- | --- |
| Driving forward, automatic gears, top speed | ✅ Works (Leopard: ~74 km/h measured) |
| Steering while moving | ✅ Works |
| Pivot turn in place | ✅ Works |
| Reverse | ⚠️ Native reverse is broken in 1.8.0.13 — fixed with a small script ([why](docs/en/09-bugs.md)) |
| Track / road wheel animation | ❌ Not provided by the native system out of the box |
| Multiplayer | ⚠️ Not tested yet; the scripted reverse currently runs only on the driver's machine |

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
| 13 | [Appendix](docs/en/13-appendix.md) |

## Quick start

1. Make your hull prefab inherit `{0608D8FA71FD3433}Prefabs/Vehicles/Core/TrackedVehicle_Base.et`.
2. Fill in the `Simulation Tracked` block: friction curves, wheel templates and exactly two tracks
   ([chapter 4](docs/en/04-from-scratch.md); a complete working block is in
   [examples/leopard2a7](examples/leopard2a7/VehicleTrackedSimulation_Leopard2A7.et.txt)).
3. Add [`LEO_TrackedReverseComponent`](scripts/LEO_TrackedReverseComponent.c) so reverse actually drives,
   and set `ThrottleReverseTarget 1` on the controller.

## Repository layout

```
docs/en/        English chapters
docs/pt-BR/     Portuguese chapters (same file names)
docs/images/    Stills and GIFs from the test video
scripts/        Enforce Script components (scripted reverse, telemetry)
examples/       The Leopard 2A7 example (prefab block now, full mod later)
```

## Credits

- **3D model:** ["Leopard 2 A7"](https://sketchfab.com/3d-models/leopard-2-a7-b29d1cb2e65e4fa8a6e99f88e11b013b)
  by [king_st0ne](https://sketchfab.com/kingstonlee96), licensed under
  [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/). See [CREDITS.md](CREDITS.md).
- **Guide, research and scripts:** Kiichi.

## License

| What | License |
| --- | --- |
| Documentation and images (`docs/`, READMEs) | [CC BY-SA 4.0](LICENSE-docs) |
| Code (`scripts/`, prefab and code snippets in `examples/`) | [MIT](LICENSE) |
| 3D model and anything derived from it | [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/), credit king_st0ne |

Arma Reforger and Enfusion are trademarks of Bohemia Interactive a.s. This project is not
affiliated with or endorsed by Bohemia Interactive.

## Contributing

Issues and pull requests are welcome, especially results from newer game versions.
Please keep the English and Portuguese chapters in sync.
