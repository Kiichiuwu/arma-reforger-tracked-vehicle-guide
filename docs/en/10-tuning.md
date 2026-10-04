> [🇧🇷 Português](../pt-BR/10-tuning.md) · 🇬🇧 English

[← Bugs and limitations (1.8.0.13)](09-bugs.md) · [Contents](../../README.md) · [Ready-made scripts →](11-scripts.md)

# Tuning with real-world data

The goal is to start from the real vehicle's spec sheet and reach values that drive right in the game. Follow the order below; each step depends on the previous one.

### Data to collect

| Data | Leopard 2A7 | Goes to |
| --- | --- | --- |
| Combat weight | 64.5 t | `RigidBody.Mass` |
| Power and rpm | 1100 kW at 2600 rpm (MTU MB 873 Ka-501) | `MaxPower`, `RpmMaxPower` |
| Max torque | \~4700 N·m at \~1600 rpm | `MaxTorque`, `RpmMaxTorque` |
| Idle, cutoff, limit | 700 / 2700 / 3000 rpm | `RpmIdle`, `RpmRedline`, `RpmMax` |
| Gears | Renk HSWL 354: 4 forward, 2 reverse | `Forward`, `Reverse` (the sim accepts only one reverse) |
| Top speed | \~70 km/h forward, \~31 km/h in reverse | `Ratio` calculation |
| Drive sprocket radius | 0.355 m (measured on the model) | `Sprocket.Radius` |
| Road wheels | 7 per side, radius 0.35 m + track shoe | `RoadWheel.Radius` |
| Suspension travel | \~350 mm up | `MaxTravelUp` |
| Frontal area | \~3.75 m × 2.5 m | `ReferenceArea` |

### 1. Mass and center of mass

Use the real mass in kg. Place the `CenterOfMass` low, near the hull floor. On the Leopard: `0 1 -0.3`.

### 2. Engine

Copy power, torque and rpm values from the spec sheet. Then adjust the three values that are not on the sheet:

| Attribute | Rule | Leopard |
| --- | --- | --- |
| `Friction` | About 10% of `MaxTorque` | 470 |
| `Steepness` | 50 (vanilla engines use 4 to 50) | 50 |
| `Inertia` | Start at 3; higher values make the rpm rise slowly | 3 |

With `Friction` 1400 (30% of the torque), the Leopard accelerated slowly. Reverse could not even beat the engine's own friction.

### 3. Gearbox

Real tank transmissions have a torque converter. It multiplies the output torque by about 2 to 2.5 times. The simulation has no converter. Compensate by shortening the first gears.

Leopard: `Forward { 6.0 3.3 1.8 1.0 }`, `Reverse 2.3`.

### 4. Final drive ratio from top speed

Pick the `Ratio` so that the top gear reaches the real speed at the max power rpm:

```math
R_d = \frac{\text{RpmMaxPower} \cdot \frac{2\pi}{60} \cdot r_{\text{sprocket}} \cdot 3.6}{g_{\text{top}} \cdot v_{\text{max}}}
```

Leopard: 2600 · 0.10472 · 0.355 · 3.6 / (1.0 · 71) ≈ **4.9**.

Check the speed table for each gear, still at 2600 rpm:

| Gear | Ratio | Speed |
| --- | --- | --- |
| 1st | 6.0 | 11.8 km/h |
| 2nd | 3.3 | 21.5 km/h |
| 3rd | 1.8 | 39.5 km/h |
| 4th | 1.0 | 71.0 km/h |

Measured in game: 74.3 km/h on flat ground, with the rpm going a little past 2600.

### 5. Suspension

Calculate the spring from the static load on each wheel and the desired sag:

```math
k = \frac{\text{Mass} \cdot 9.81}{n_{\text{wheels}} \cdot \text{sag}}
```

Leopard: 64500 · 9.81 / (14 · 0.10 m) ≈ 452 000 N/m. That gives `SpringRate 450`, if the unit is N/mm (likely, not confirmed).

For the damper, use a fraction ζ of critical damping:

```math
c = \zeta \cdot 2\sqrt{k \cdot m_{\text{wheel}}}, \qquad m_{\text{wheel}} = \frac{\text{Mass}}{n_{\text{wheels}}}
```

With ζ = 0.3: c ≈ 27 000 N·s/m. The Leopard uses 25 000 for compression and 35 000 for rebound.

### 6. Brakes

Rule of thumb: the sum of all `BrakeTorque` values should be close to mass × deceleration × radius. For 0.6 g on the Leopard, that is about 140 000 N·m. The prefab has 14 × 6000 + 2 × 30000 = 144 000 N·m.

### 7. Aerodynamics, retarder and controller

- `ReferenceArea` = width × height. `DragCoefficient` around 0.9 for a boxy hull.
- `RetarderBaseTorque 0` and `RetarderThrottleSpeed 20`, otherwise the retarder limits the speed.
- `ThrottleReverseTarget 1`.

### 8. Measure with telemetry

1. Add the `LEO_TelemetryComponent` ([Ready-made scripts](11-scripts.md) section) to the prefab.
2. Drive: a standing start to top speed, braking, reverse and a pivot turn.
3. Open the session log: `Documents\My Games\ArmaReforgerWorkbench\logs\logs_<date>\console.log`.
4. Filter the `LEOTEL` lines. Each line shows speed, rpm, throttle, gear, clutch and brake.
5. Compare with the formulas:

| What to check | If it is wrong |
| --- | --- |
| Top speed lower than calculated | Retarder on, high `Friction` or weak engine |
| Speed always capped at the same value | `RetarderThrottleSpeed` too low |
| RPM above `RpmMax` with `gear=0` | This is the reverse bug. Normal without the script |
| `thrust=0.80` with W | `ThrottleTurbo` 0.2. Normal |
| Speed per gear differs from the table | Wrong drive sprocket radius or `Ratio` |

---

[← Bugs and limitations (1.8.0.13)](09-bugs.md) · [Contents](../../README.md) · [Ready-made scripts →](11-scripts.md)
