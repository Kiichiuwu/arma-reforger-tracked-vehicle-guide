> [🇧🇷 Português](../pt-BR/01-overview.md) · 🇬🇧 English

[← About this guide](00-about.md) · [Contents](../../README.md) · [Architecture →](02-architecture.md)

# Overview

Arma Reforger 1.8.0.13 (engine 192142) already ships a real track simulation, `VehicleTrackedSimulation`. Driving forward, steering, turning in place and the per-wheel suspension all work. Reverse has a bug that blocks it, which a script can work around. The simulation can also move a chain of track links by itself, but that chain behaves like a loose rope, so this guide moves the links by script instead.

No vanilla vehicle uses this system yet: it exists only as a hidden base prefab, `Prefabs/Vehicles/Core/TrackedVehicle_Base.et`. Vanilla 1.8 has no tracked vehicle and no track link model.

This guide was written from a Leopard 2 mod: the drivetrain and reverse work started on an earlier Leopard 2A7 build, and everything else was done on the current Leopard 2A6. Everything here was tested in game with telemetry, observed in recorded tests or reverse-engineered from the Workbench executable, and the text says which ([About this guide](00-about.md)).

![Right side of the Leopard 2A6 on grass: the scripted track links wrap the rear sprocket, run under the seven road wheels and over the return rollers](../images/sprocket-wrap.jpg)

*The result: the Leopard 2A6 at 7.8 km/h on grass, right side (test video A, 2026-10-06, 3:29.5). The track links wrap the rear sprocket, run under all seven road wheels and pass over the return rollers. The wheels and links are moved by script from the simulation's own wheel data. Model by buh-late, CC BY 4.0, modified.*

|  | Native track (`VehicleTrackedSimulation`) | Old mod method (7 wheel axles) |
| --- | --- | --- |
| Physics | Two tracks; each has road wheels, an idler, a drive sprocket and optional return rollers | A car with 7 axles; the front and rear axles steer in opposite directions |
| Turning in place (pivot) | Yes, native | No; only an approximation |
| Reverse | Bug in 1.8.0.13: needs a script ([bugs section](09-bugs.md)) | Works |
| Suspension | Native, per road wheel: a ray along the hull's down axis and a spring that is free at full droop. `MaxTravelDown` sets the ride height ([chapter 13](13-suspension.md)) | The wheeled simulation's suspension, per axle |
| Road wheel, idler, sprocket and roller animation | Script. `CarProcAnimComponent` gives tracked wheels another wheel's signals in 1.8.0.13, so `LEO_RunningGearComponent` reads the simulation per (track, part) and moves the bones ([chapter 14](14-running-gear.md)) | Procedural animation per axle, or script |
| Track links | Native link chain (`TrackSegment`): a 2D rope with gravity, wheel contact and sprocket drive, but no ground collision and no way to reset it from script, so it sags and can come off the sprocket. A scripted chain (`LEO_TrackChain`) follows the wheels instead ([chapter 15](15-track-links.md)) | Always script (own track entities or animated bones) |
| Who uses it | Nobody in vanilla; none of the 243 installed Workshop mods checked uses it | M113, WCS M1A1/M2A2/BMP-3, CIE T-72, Zagoria T-80U |
| Official documentation | None | Inherits from vanilla wheeled vehicles |

In practice:

- You can build a tank with native tracks using only the base game, with no dependencies.
- You must configure the simulation block by hand: the vanilla template does not load. Define the two required friction curves, list each track's road wheels first, tune the controller and add a reverse script ([step by step](04-from-scratch.md)).
- The suspension works natively. Size it from the weight: `MaxTravelDown ≈ m·g/(n·k)` puts the hull at its modelled height. `CenterOfMass` on the `RigidBody` is an offset from the model's own centre of mass, not an absolute position ([chapter 13](13-suspension.md)).
- Leave `CarProcAnimComponent` empty and drive the wheel bones from the simulation by script ([chapter 14](14-running-gear.md)).
- Show players the scripted link chain. Keep the native chain's attributes on the simulation anyway: the native debug overlay most likely needs a chain (it crashed without one), and the running gear script has only been tested with a chain present ([chapter 15](15-track-links.md)).
- Use the native debug overlay (Diag build) and the telemetry script to check what the simulation does ([chapter 16](16-debugging.md)).
- After a prefab edit, reload the world. A vehicle in a world that is already open keeps its old simulation values ([chapter 13](13-suspension.md#prefab-edits-and-the-open-world)).

Still open: multiplayer (other players' machines), the Leopard 2A6's top speed, and the scripted chain through a flip or a mine blast. [Bugs and limitations](09-bugs.md) lists what is known to be broken.

---

[← About this guide](00-about.md) · [Contents](../../README.md) · [Architecture →](02-architecture.md)
