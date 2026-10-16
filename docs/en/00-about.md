> [🇧🇷 Português](../pt-BR/00-about.md) · 🇬🇧 English

[Contents](../../README.md) · [Overview →](01-overview.md)

# About this guide

**Purpose.** This guide documents the native tracked vehicle feature of Arma Reforger (`TrackedVehicle_Base` and `VehicleTrackedSimulation`). It teaches you how to use it in future mods, from the 3D model to a driving tank with moving wheels and track links.

**Scope.** Only the native tracked vehicle: the simulation, the controller, the suspension, the wheels, the tracks and their links, their animation, tuning, telemetry and debugging. Weapons, armour and crew are not covered.

**How it was made.** Everything here was discovered and tested while building a Leopard 2 mod, using only the base game. The work ran in two stages:

| Edition | Model | What was worked out |
| --- | --- | --- |
| First | Leopard 2A7 (earlier build) | Drivetrain, controller, the reverse bug and its script, top speed, the first crashes |
| This one | Leopard 2A6 | Suspension and ride height, centre of mass, road wheels and return rollers, the `CarProcAnimComponent` mapping bug, the native link chain, the scripted running gear and link chain, the native debug overlay |

Values quoted are the Leopard 2A6's unless the text says "measured on the earlier 2A7 build".

**How each fact is known.** The guide marks how it knows each fact that matters:

| Label | Meaning |
| --- | --- |
| Measured | Read in Workbench Play mode from telemetry (`LEOTEL`, `LEOSUSP` and `LEOGEAR` lines in `console.log`) or from the native debug overlay |
| Reverse-engineered | Read from the 1.8.0.13 Workbench Diag executable: attribute strings, error messages, disassembly |
| Observed in video | Seen in a recorded test clip. The caption or text gives the clip and the time |
| Not tested / Open | A reasoned expectation or an open question. Do not rely on it |

Many findings were checked both ways, in game and in the executable, and the key ones were reviewed by a second, independent analysis.

**Reference example.** The Leopard 2A6 hull prefab (`Leopard2A6_base.et` in the mod) is the example throughout the guide. Its simulation block and its hull component overrides are in [examples/leopard2a6](../../examples/leopard2a6/README.md). The scripts it uses are in [scripts/](../../scripts/), and the tool that made its `TrackPositions` is [tools/trackpositions.py](../../tools/trackpositions.py). The mod's models and textures are not in this repository.

**3D model used.** ["Leopard 2 A6"](https://sketchfab.com/3d-models/leopard-2-a6-7cb23d5322df4b409a880de635826067) by [buh-late](https://sketchfab.com/buh-late), licensed under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/), published on Sketchfab on 2024-01-17. It was modified for Enfusion: split into parts, re-parented, bones and a suspension rig, colliders, LODs and repacked textures. The author does not endorse this project. The full list of changes is in [CREDITS.md](../../CREDITS.md).

CC BY 4.0 asks these things from anyone who shares the model or a modified version of it, including the one in the mod:

- give credit to the author;
- link the license;
- say that changes were made;
- do not suggest that the author endorses you or your use.

It has no ShareAlike clause. The guide's text and images stay under CC BY-SA 4.0 ([LICENSE-docs](../../LICENSE-docs)). The model shown in the images remains buh-late's work under CC BY 4.0.

**Previous edition.** The first edition of this guide used a Leopard 2A7 built from "Leopard 2 A7" by king_st0ne (CC BY-SA 4.0). Its text, images and prefab block are kept in the git history under the tag [`v1-leopard2a7`](https://github.com/Kiichiuwu/arma-reforger-tracked-vehicle-guide/tree/v1-leopard2a7). Chapters 00-12 keep their file names, so old links still work. The old appendix, `13-appendix.md`, is now [17-appendix.md](17-appendix.md). Where an old 2A7 measurement is still the only evidence, such as the top speed of 74.3 km/h and the reverse bug's Δ table, this edition says that it was measured on the earlier 2A7 build.

**Tested version.** Workbench and Arma Reforger 1.8.0.13, in October 2026. The Leopard 2A6 tests ran from 2026-10-04 to 2026-10-06, in the Diag build of Workbench (`ArmaReforgerWorkbenchSteamDiag.exe`). The native vehicle debug entries ("Show vehicle debug", the rope's "Draw tracks" and "Reset tracks", the per-track stats box) exist only in Diag builds (the Workbench and `ArmaReforgerSteamDiag.exe`); the retail game executable (`ArmaReforgerSteam.exe`) does not contain them (string scan of the 1.8.0.13 executables). Game updates can change names, defaults and bugs.

---

[Contents](../../README.md) · [Overview →](01-overview.md)
