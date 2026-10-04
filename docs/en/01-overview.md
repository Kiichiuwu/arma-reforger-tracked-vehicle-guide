> [🇧🇷 Português](../pt-BR/01-overview.md) · 🇬🇧 English

[← About this guide](00-about.md) · [Contents](../../README.md) · [Architecture →](02-architecture.md)

# Overview

Arma Reforger 1.8.0.13 (engine 192142) already ships a real track simulation, `VehicleTrackedSimulation`. It works for driving forward, turning and turning in place. But it has a bug that blocks reverse, which a script can work around. No vanilla vehicle uses this system yet: it exists only as a hidden base prefab, `Prefabs/Vehicles/Core/TrackedVehicle_Base.et`.

This guide was written from the Leopard 2A7 mod. Everything here was tested in game with telemetry or reverse-engineered from the Workbench executable, and checked by a second, independent analysis.

|  | Native track (`VehicleTrackedSimulation`) | Old mod method (7 wheel axles) |
| --- | --- | --- |
| Physics | Two tracks, each with a drive sprocket, an idler and road wheels | A car with 7 axles; the front and rear axles steer in opposite directions |
| Turning in place (pivot) | Yes, native | No; only an approximation |
| Reverse | Bug in 1.8.0.13: needs a script ([bugs section](09-bugs.md)) | Works |
| Track animation | Native link chain (optional) or script | Always script |
| Who uses it | Nobody in vanilla; none of the 243 installed mods tested | M113, WCS M1A1/M2A2/BMP-3, CIE T-72, Zagoria T-80U |
| Official documentation | None | Inherits from vanilla wheeled vehicles |

In practice: you can build a tank with native tracks using only the base game, with no dependencies. You must configure the simulation block by hand (the vanilla template is not enough). You must also define two required friction curves, tune the controller and add a reverse script.

---

[← About this guide](00-about.md) · [Contents](../../README.md) · [Architecture →](02-architecture.md)
