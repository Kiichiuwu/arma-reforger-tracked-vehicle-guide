> [🇧🇷 Português](../pt-BR/19-kiichi-tracked-system.md) · 🇬🇧 English

[← Multiplayer](18-multiplayer.md) · [Contents](../../README.md) · [1.9 Experimental →](20-experimental-1.9.md)

# Kiichi Tracked System

Kiichi Tracked System is a tracked-vehicle simulation written entirely in script: one component, [`LEO_TrackSimComponent`](../../scripts/LEO_TrackSimComponent.c). It does not use `VehicleTrackedSimulation`. The hull is a plain dynamic rigid body, and the component applies every force to it in `EOnSimulate` (the fixed physics step), on the server only.

It exists because the native simulation cannot be fixed from script: the network move drops the handbrake, reverse does not couple, the owner-client path of tracked vehicles is unfinished ([chapter 18](18-multiplayer.md)), and on 1.9.0.10 Experimental the native simulation cannot even keep a parked hull on the ground ([chapter 20](20-experimental-1.9.md)). A system that never touches the native path avoids all of that, and the same code runs on 1.8 and 1.9.

It reuses the **tuning values** of the native Leopard prefab (gear ratios, final drive, engine curve, wheel radius, spring rate, the hull mass from `RigidBody`), not the native simulation.

Each fact says how it is known: **measured** (logs of automated runs), **design** (what the code does) or **not tested**.

### In short

| | Vanilla (native, patched with `LEO_TrackedDriveComponent`) | Kiichi Tracked System |
| --- | --- | --- |
| Physics | `VehicleTrackedSimulation` (engine side) | Script, on a plain rigid body |
| Pivot / neutral steer on a dedicated server | ✅ Only because the server applies the handbrake itself | ✅ The belts counter-rotate (design) |
| Reverse | ⚠️ A push on the hull; the native reverse does not couple | ✅ The engine drives the belts backwards |
| Driver on a client | Server-owned, inputs by RPC; the native movement replication moves the hull | Server-owned, inputs by RPC; own transform replication with dead reckoning |
| 1.8.0.13 | ✅ | ✅ (measured) |
| 1.9.0.10 Experimental | ❌ The native simulation breaks before driving | ✅ (measured) |
| Native debug overlay, native rope chain | Available | Not available (no native simulation); `LEOSIM` log lines instead |

### The model

All of it runs on the server, once per physics step:

- **Suspension.** One raycast per road wheel, from the modelled wheel position (bones `v_suspension_<l|r>0<k>`), along the hull's down axis. Spring and damper act along the hull's up axis at the wheel position, with a preload so the hull rests near its modelled height. The wheels that touch the ground carry the hull and form the contact area.
- **Powertrain.** Engine torque curve (peak torque, peak power, idle, maximum rpm, friction), clutch that slips at idle, automatic gearbox by rpm, final drive, sprocket. The engine's torque turns both sprockets, so **each belt has its own speed state**, and the rpm follows the belts through the gearbox.
- **Traction.** At every road wheel in contact, the belt slips against the ground by (belt speed − ground speed at that wheel). The shear force grows with the slip and saturates at μ × that wheel's load. The sum of these forces moves the hull; their reaction loads the belt, and through it the engine. More wheels on the ground give more tractive force; a belt that spins faster than the ground slips and pulls less.
- **Steering (clutch-brake).** Forward alone drives both belts equally. Forward + side moves part of the engine drive to the outer belt and brakes the inner one a little. Side alone while moving brakes the inner belt hard and lets the outer one roll, so the hull swings as it slows. Near standstill the steering drive counter-rotates the belts: a real pivot. The sideways slip of the front and rear wheels is the turning resistance, and it falls with speed.
- **Reverse.** At brake input while standing, the gearbox selects reverse and the engine drives the belts backwards.
- **Brakes and grip.** The brakes act on the belts. Each wheel in contact has lateral grip against sideways sliding, plus rolling resistance.

All the coefficients are attributes, listed with their defaults in [chapter 11](11-scripts.md#leo_tracksimcomponent-kiichi-tracked-system).

### Multiplayer

The server owns the vehicle and the simulation (design, measured):

| Machine | What it does |
| --- | --- |
| Server | Runs the simulation. Takes the vehicle back if the engine gives it to the driver's client (a client that owns a vehicle with an inert native simulation crashes, measured). Replicates belt speeds, rpm, gear, the engine state and the hull transform (position, angles, velocity) as `RplProp`s, 20 times a second |
| Driver's client | Reads the driver's keys (`TrackedContext`) every frame and sends throttle, brake, steer and handbrake through `LEO_PlayerController` at 20 Hz and on change; engine start/stop goes reliable |
| Every client | Follows the replicated hull transform with dead reckoning and smoothing (`m_fProxySmoothing`). The hull's rigid body is never simulated there. Runs the suspension raycasts in visual mode (no forces), so the wheels follow the ground under the replicated hull, and feeds the running gear and link chain from the replicated belt speeds |

The replication rules this is built on are in [chapter 18](18-multiplayer.md#replication-rules-for-scripted-vehicles-measured). Client code runs from the call queue, because a replicated vehicle gets no entity events on clients.

### The three prefabs of the mod

The Workshop mod ships three Leopard 2A6 prefabs, children of a common `Leopard2A6_base.et` that carries no drive system ([example](../../examples/leopard2a6/Prefabs_Leopard2A6_variants.et.txt)):

| Prefab | Name in the Game Master | Drive system |
| --- | --- | --- |
| `Leopard2A6.et` | Leopard 2A6 (vanilla tracked simulation) | Native `VehicleTrackedSimulation` + `LEO_TrackedDriveComponent` |
| `Leopard2A6_LEO18.et` | Leopard 2A6 (Kiichi Tracked System, 1.8) | `LEO_TrackSimComponent` |
| `Leopard2A6_LEO19.et` | Leopard 2A6 (Kiichi Tracked System, 1.9) | `LEO_TrackSimComponent` |

The 1.8 and 1.9 Kiichi prefabs are identical apart from the name; there are two so that each game version has an entry next to the vanilla one for a side-by-side comparison. The Kiichi prefabs have no `VehicleTrackedSimulation` block, so the log shows `Tracked - failed to load!` and `RoadWheel - configuration is missing!` when one spawns. That is expected: the native simulation stays inert.

### Using it on your own tank

1. Copy all the scripts from [`scripts/`](../../scripts/) ([chapter 11](11-scripts.md#how-to-install)).
2. Build the hull prefab on `TrackedVehicle_Base.et` as usual, with `RigidBody` (mass, centre of mass), `CarProcAnimComponent` empty, `LEO_RunningGearComponent` and `LEO_TrackBeltComponent`, but **without** the `VehicleTrackedSimulation` block.
3. Rig the suspension bones `v_suspension_<l|r>01` … `0<n>` at the modelled road wheel centres: the raycasts start there. If a bone is missing, the log says `LEOSIM … suspension bones missing` and the Leopard layout is used.
4. Add `LEO_TrackSimComponent` and set its attributes from your vehicle's data: road wheel count and radius, spring rate per wheel and preload (the hull rests at its modelled height when n · `m_fSpringRate` · `m_fPreload` ≈ m · g; Leopard: 62300 · 9.81 / (14 · 0.10) ≈ 437 000 N/m, set to 450 000), engine curve, gear ratios, final drive, sprocket radius.
5. Test with `m_bDebug 1` (one `LEOSIM` line per second: speed, yaw rate, inputs, rpm, gear, belt speeds, wheels in contact, load per side), then switch it off for a release.

### Measured

Automated runs of the game, 2026-10-22, 1.8.0.13, test world on Arland, Leopard 2A6 (62.3 t):

| Run | Result |
| --- | --- |
| Headless server, scripted test inputs (throttle 1 for 7 s, then full steer, then release) | 0 to 38.7 km/h in about 6 s, gears 2 to 4, yaw up to 0.72 rad/s, stops, no instability |
| Listen host seated as driver, same inputs | The engine starts on throttle, 0 to 40 km/h, yaw up to 1.1 rad/s |
| Diag dedicated server + Diag client, the client's player seated as driver, inputs through the real RPC path | The client's copy follows the hull up to 40 km/h, through the turn and the stop. No crash on either side |

The same component was measured on 1.9.0.10 Experimental on 2026-10-20 with the same harness, with the same behaviour.

### Known limits

- **Not yet driven by a person on a client.** Everything above comes from scripted inputs. Feel, camera and latency are not tested.
- **Parked creep.** Parked on a slight slope without brake input, the hull creeps at 1–2 cm/s (measured). Not fixed.
- **No native debug overlay** and no native rope chain: the scripted chain of [chapter 15](15-track-links.md) is the only one.
- **Sound** uses the vanilla vehicle sound component; rpm and gear are replicated but not yet fed to sound signals.

---

[← Multiplayer](18-multiplayer.md) · [Contents](../../README.md) · [1.9 Experimental →](20-experimental-1.9.md)
