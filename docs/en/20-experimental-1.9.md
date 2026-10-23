> [🇧🇷 Português](../pt-BR/20-experimental-1.9.md) · 🇬🇧 English

[← Kiichi Tracked System](19-kiichi-tracked-system.md) · [Contents](../../README.md)

# 1.9.0.10 Experimental

What changed for tracked vehicles in Arma Reforger **1.9.0.10 Experimental**, checked on 2026-10-19 and 2026-10-20 with the Experimental Workbench and automated headless runs. It is an experimental build: re-test every new build before relying on any of this.

Each fact says how it is known: **measured** (logs), **reverse-engineered** (Experimental `ArmaReforgerWorkbenchSteamDiag.exe`) or **observed** (video).

### In short

| Point | 1.8.0.13 | 1.9.0.10 Experimental |
| --- | --- | --- |
| Script API for tracked vehicles | — | Unchanged |
| Handbrake in the network move | Always false (stub) | Always false (same stub) |
| Road wheel suspension force | Correct sign | **Sign inverted** |
| Parked hull after spawn | Stable | **Thrown to power-of-two coordinates** in about 50–80% of spawns |
| Reloading scripts in the Workbench | No effect seen | **Corrupts the simulation state** |
| Workbench frame rate with the native simulation | Not measured | 12 fps with a driver, 45 without (one scene, observed) |
| Turret stabilisation prefab properties | Present | `TurretInitStabilization` and `TurretEnabledStabilization` removed (the prefab warns about obsolete properties) |

The native tracked simulation cannot be used on this build. [Kiichi Tracked System](19-kiichi-tracked-system.md) does not use it and works the same as on 1.8 (measured).

### Suspension force with the wrong sign (measured)

A parked Leopard 2A6 is pushed **into** the ground: −37.7 m/s² on the first steps, a force of about 1.74 MN, which is 14 road wheels × `SpringRate` 450 kN/m × 0.3 m (the full `MaxTravelUp`). The hull then hops, flips or launches. With `LEO_RunningGearComponent` moving the wheel pivot bones it grows to NaN and crashes the Workbench.

The only workaround found is negative `SpringRate`, `CompressionDamper` and `RelaxationDamper`. It makes the hull stand on 1.9.0.10 Experimental and would break it on 1.8, so never ship it in a 1.8 mod. The pivot's `Rotation` is ignored for the suspension axis.

### Hull thrown to power-of-two coordinates (measured)

In about 50–80% of spawns, the parked hull is moved within the first steps to coordinates that are powers of two, or launched at absurd speeds (84 339 m/s downwards in one logged run), and keeps tumbling. It happens with or without the mod's scripts, with or without the native rope chain, and with either spring sign. Values like these point to uninitialized state in the engine, not to the physics settings. Script `EOnSimulate` runs at a fixed 16.67 ms through it, so it is not a long time step.

Harness used: a dedicated server on the test world, 50 s per run, waiting for `Frame start`, many runs per configuration.

### Script reload corrupts the simulation (measured)

Reloading scripts in the Experimental Workbench with a tracked vehicle in the world corrupts its simulation: the next spawn launches at about 10⁹ m/s. Restart the Workbench instead of reloading.

### The handbrake stub is still there (reverse-engineered)

In the Experimental Workbench exe, the tracked `IsHandbrakeOn` (`0x141016B00`) is still `<sim-ready guard>; xor al, al; ret`: the native tracked simulation has no handbrake state to report. The move recorder (`0x141034CA0`) stores it as a float at `move + 0x0C`, so the recorded handbrake is always 0. `0x140EEFD70` (`cmp [sim+0x19F0], 2`) is only the readiness guard every getter calls.

The car move (`0x140D20940`) holds steer, throttle, brake, handbrake, gear, clutch and two controller fields. The tracked move holds the input pair, brake, handbrake (always 0), gear, clutch and one more field: the same layout apart from the handbrake. So, as on 1.8, pivoting on a dedicated server needs the server to apply the handbrake itself ([chapter 18](18-multiplayer.md)).

### Other notes

- The Experimental Workbench keeps its plugin settings under a separate registry key: configure the Peer Tool again there ([chapter 18](18-multiplayer.md#testing-multiplayer-on-one-pc-the-peer-tool)).

---

[← Kiichi Tracked System](19-kiichi-tracked-system.md) · [Contents](../../README.md)
