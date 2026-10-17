> [🇧🇷 Português](../pt-BR/18-multiplayer.md) · 🇬🇧 English

[← Appendix](17-appendix.md) · [Contents](../../README.md)

# Multiplayer

This chapter covers what happens to a native tracked vehicle when a player drives it from a client: how the engine replicates it, the bug that stops pivot and neutral steering on servers, the fix in the Leopard scripts, and the engine limitation that is left. It also explains how to test multiplayer on one PC with the Workbench Peer Tool.

Everything was tested on 2026-10-16 and 2026-10-17 with Workbench 1.8.0.13 (Workbench as server plus one Peer Tool client), after a first report from a tester on a dedicated server. Each fact says how it is known: **measured** (logs), **reverse-engineered** (`ArmaReforgerWorkbenchSteamDiag.exe` 1.8.0.13, addresses in the [Appendix](17-appendix.md)), **observed** or **not tested**.

### In short

| Situation | Status |
| --- | --- |
| Singleplayer, or a listen host driving its own tank | ✅ Works as in the rest of this guide |
| Dedicated server, pivot / neutral steer from standstill | ❌ Native: the server never pivots, the client rubberbands. ✅ Fixed by `LEO_TrackedReverseComponent` v5 (the client sends its handbrake, the server restores it) |
| Dedicated server, reverse | ❌ Native: the scripted push ran only on the driver's machine. ✅ v5 runs it on the server too |
| Dedicated server, driving forward and turning at speed | ✅ Works natively |
| Dedicated server, the **driver** on a client | ⚠️ Engine limitation: the driver's head does not animate, mouse look is sluggish while stationary, the camera jumps under acceleration and turning, and inputs can stick. Not fixable from script (see below) |
| Other seats on a client (turret) | ✅ Normal |
| Scripted running gear and link chain on clients | ✅ Animate on the driver's client and on other clients (measured on the peer: `LEOGEAR` and `LEOTRACK` lines, links visible). Nothing runs on a dedicated server by design |

### How a tracked vehicle is replicated

The server is the authority. The driver's client **predicts** the vehicle and sends a **move** every frame; the server replays the move, compares its state with the client's and approves or corrects. All of this is native (`NwkTrackedMovementComponent`), reverse-engineered:

| Step | What happens | Where |
| --- | --- | --- |
| Record | The driver's client reads the simulation inputs into a move: steer, throttle, brake, handbrake, gear, clutch, shift state, the engine actions (start/stop) and its own transform and velocities. One move per frame, kept in a ring of pending moves | Move recorder `0x140fe5d10`, producer `0x141075190` |
| Send | `Rpc_USendMove_S`, a `GenericArray<TrackedMinimalMove>` to the server | — |
| Replay | The server copies the move into the controller, then applies it to the simulation: `SetInputs(steer, throttle)`, `SetBreak(brake, handbrake)`, `SetClutch`, `SetGear`, then the engine actions | `0x14106e4d0` → `0x140fe7c70` → `0x140fe4940` |
| Compare | After its own step, the server compares position, angle and velocities with the client's report, within allowances: distance |v|/300 clamped to 0.001–1.5 m, angle 0.003, linear velocity 0.3, angular velocity 0.25 | `0x141074940` → `0x14104d8f0`; defaults in the constructor `0x14106c700` |
| Correct | On a mismatch the client snaps to the server state and **re-simulates its pending moves** in a native loop. No script runs in that loop | `0x141074770` → `0x141076250`, loop `0x141075cc0` |

Other players' machines (proxies) do nothing in this component: they interpolate the server's state.

Two script levers exist on `NwkTrackedMovementComponent`: `SetAllowance(speedScaledDistance, distance, angle, linearVelocity, angularVelocity)` writes the allowances the server compares with, and `SetPrediction(bool)` only switches the client's re-simulation off. Neither reads the move.

### Bug 1: the handbrake never reaches the server

Pivot and neutral steering need the simulation's **handbrake flag**. In singleplayer the Debug panel shows `Brake 6.0` during every pivot, which only the handbrake path stores ([chapter 16](16-debugging.md)).

In multiplayer the flag is lost on the way (reverse-engineered, the two instructions checked by hand):

- the move recorder fills the handbrake field with `VehicleTrackedSimulation::IsHandbrakeOn` (`0x140fc23c0`), which is a stub: `call <validity check>; xor al, al; ret`. It always returns false;
- the server replay calls `SetBreak` through the binding `0x140fc3c10`, which clamps the brake to [0, 1]. Brake 6.0 arrives as 1.0, with the handbrake false.

So the server holds the tank with brake 1.0 while the replayed throttle and clutch rev the engine, the driver's client pivots, and the server corrects it on every move: no pivot, engine at the limiter, rubberband. Driving and turning at speed work because no handbrake is involved.

**Measured** on the Peer Tool server with the v4 scripts: no pivot, `LEONET`-less logs; with v5: `LEONET server brake=6.00 mirrored=61 wheelRpm L/R=-059.3 / 0058.1 ... steer=-1.00`, the two tracks counter-rotating on the server.

### Bug 2: the scripted reverse ran only on the driver's machine

A force applied by script exists only where it is applied. v4 pushed only on the driver's machine; the server applied gear R and throttle 1 from the move, the native reverse is pinned above `RpmMax` ([chapter 9](09-bugs.md)), the server's hull stayed put, and the client was corrected again.

### The fix: LEO\_TrackedReverseComponent v5

[`scripts/LEO_TrackedReverseComponent.c`](../../scripts/LEO_TrackedReverseComponent.c) now has three roles:

| Machine | What it does |
| --- | --- |
| Driver's client (owner, proxy) | As v4: reads the keys and pushes the hull in reverse. Also sends `{reverse, steer, handbrake}` to the server with an RPC (`RpcAsk_DriveState`, `RplRcver.Server`): on change, in 0.1 steering steps, with a 0.5 s keep-alive. The handbrake is read as `GetBrake() > 5.9` and held for 0.25 s, because the controller drops it for single frames during a pivot |
| Server, driver on another machine | Puts the handbrake back with `SetBreak(1, true)` whenever the replayed brake sits at 1.0, in `EOnSimulate` **and** `EOnPostSimulate` (the move is consumed in the post-simulate phase, so both orders are covered). Applies the same reverse push and yaw torque. Relaxes the allowances while pivoting or reversing (`SetAllowance(false, 0.3, 0.05, 0.6, 0.3)`, restored to `(true, 300, 0.003, 0.3, 0.25)` 0.5 s after), because the client's re-simulation can reproduce neither the handbrake nor the push. Stops the engine when the driver leaves (authority only) |
| Other players' machines | Nothing |

New attributes: `m_bMirrorHandbrake` (1), `m_bRelaxAllowance` (1), `m_bClientPrediction` (1; 0 calls `SetPrediction(false)` on the driver's client, which did not help with anything and stays for experiments). With `m_bDebug 1` the log shows `LEONET server got drive state …`, `LEONET server allowances relaxed=…` and, once per second during a pivot or reverse, `LEONET server brake=… mirrored=… wheelRpm L/R=…`. Opposite signs in `wheelRpm L/R` mean the server itself pivots.

Not tested: a server with real latency (50–150 ms). Expect a correction at every press and release, because the RPC and the move arrive a few frames apart.

### The engine limitation: the driver on a client

With the driver on a client, in first and third person alike, the driver character's head does not animate, mouse look is sluggish while the tank is stationary, the camera jumps far when the tank accelerates and turns, and a released key keeps acting for a while. On the same client, in the same session, a vanilla jeep (M151A2) is normal; the same tank driven from the host window is normal; the turret seats on the client are normal.

Everything that can be measured from script is identical between the jeep and the tank (measured with a temporary probe in `SCR_CharacterCameraHandlerComponent.OnBeforeCameraUpdate`):

| Measurement | Jeep (works) | Leopard (broken) |
| --- | --- | --- |
| `RplComponent` `IsOwner` / `IsProxy` | true / true | true / true |
| `NwkMovementComponent` `IsSimulationEnabled` / `IsInterpolationEnabled` / `IsInterpolating` | false / true / false | false / true / false |
| Camera keyframes per second (the scripted cameras only update on keyframes) | ~30 | ~30 |
| Character a hierarchy child of the hull | yes | yes |
| Hull position jumps (server corrections) | — | one jump of 0.19 m in a whole session |

What was tried, each in a separate run, with no change in the symptoms:

1. `SetPrediction(false)` on the driver's client.
2. Crew hiding off (`LEO_CrewViewComponent.m_bHideCrew 0`).
3. All `LEO_` components removed from the hull prefab, sleeping thresholds removed.
4. The modded camera classes removed from the mod and the turret removed from the hull slot: a hull with nothing but the vanilla components and the simulation block.
5. `EnableSimulation(true)` on the driver's client (the jeep has it false too).
6. Ownership given back to the server when a remote player sits in the pilot seat (`RplComponent.GiveExt(RplIdentity.Local(), true)`), with the driver's inputs sent by RPC and the server driving the simulation itself (`SetInputs`, `SetBreak`, `SetClutch`, `SetGear`, `ForceStartEngine`). The probe confirmed `owner=false` on the client; the driver stayed the same.

Where it comes from (reverse-engineered, partly): the owner-client move producers of `NwkCarMovementComponent` (`0x141054b50`, 36 calls) and `NwkTrackedMovementComponent` (`0x141075190`, 31 calls) have almost nothing in common, and 1.8.0.13 ships no tracked vehicle that would have exercised the tracked one. The controller ticks (`0x140fd8760` car, `0x140fe60f0` tracked) and the input readers are structurally identical, and the network component constructors write the same defaults. The exact missing piece was not found. It was reported to Bohemia Interactive with these addresses.

Practical consequence: native tracked vehicles are fine for singleplayer, listen hosts and every seat except the driver's on dedicated servers. For a vehicle that must be driven from clients today, the wheeled simulation (as the published tank mods use) is the only path.

### Testing multiplayer on one PC: the Peer Tool

The Workbench can run a server and extra game clients ("peers") on the same machine.

1. World Editor, the arrow next to *Play*: choose **Server localhost + PeerTool**. It is a mode, not an action.
2. *Plugins* → *Settings* → **Peer Tool** (the plugin's `Configure` dialog): set *Executable* to `…\Arma Reforger\ArmaReforgerSteamDiag.exe` (the Diag build, so the peer has the diag menu too) and add one entry under *Peers Windows*. The defaults are right: windowed, *Force Update*, *No Focus*, *Rpl Auto Join*. A width and height of 1280 × 720 keeps the window manageable. Without this the mode starts the server and nothing else: the plugin's default executable is `game.exe` and its peer list is empty.
3. *Play*. The Workbench becomes the server; the peer launches with `-client -rpl-reconnect -rpl-timeout-disable -profile PeerPlugin1` and joins by itself. The log shows `Running command: …`.
4. The test world runs the Game Master mode: in the Workbench window, enable a playable faction (US) and place a spawn point, otherwise the peer waits at "There are no available factions".
5. Drive in the **peer** window: it is the client. The Workbench window is the server.

The peer uses a fresh profile, so its mouse sensitivity is the default, not yours. Its logs are in `Documents\My Games\PeerPlugin1\logs\`.

Diag entries that help (both windows have the menu): *Vehicles* → *Vehicle Networking and Reconciliation* → *S: Input/Output Diags = Sim In* plots `Brake` against `BrakeClient` and `Handbrake` against `HandbrakeClient`; *Determinism testing* → *SimState to CSV* writes `$logs:VehicleTrackedStateDebug_<id>.txt` with one row per move (`MoveId`, server/client, `Corrected`, inputs, engine state); *Network Movement* → *Display Selection* / *NwkMovement Selection* / *Display buffer state* shows a vehicle's replication state live (Simulating, Interpolating, Producing, Consuming, SwitchingOwnership). Not all of these were used in this work.

### Open questions

- Which native step of the owner-client path keeps the driver character alive for cars and is missing for tracked vehicles?
- What are the units of the angle allowance (0.003), and which relaxed values keep pivots and reverse smooth under real latency?
- Does the server-side handbrake mirror hold under 100 ms+ latency, or does the correction at each transition become visible?

---

[← Appendix](17-appendix.md) · [Contents](../../README.md)
