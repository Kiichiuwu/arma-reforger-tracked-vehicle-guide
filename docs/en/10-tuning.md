> [🇧🇷 Português](../pt-BR/10-tuning.md) · 🇬🇧 English

[← Bugs and limitations (1.8.0.13)](09-bugs.md) · [Contents](../../README.md) · [Ready-made scripts →](11-scripts.md)

# Tuning with real-world data

The goal is to start from the real vehicle's spec sheet and reach values that drive right in the game. Follow the order below; each step depends on the previous one. The worked example is the Leopard 2A6 of this guide.

**Reload properly after every prefab change.** A vehicle that is already in an open world keeps its old simulation values. Close the world, reload the scripts with no world open, then open the world again. Otherwise you measure the old values (see [Bugs and limitations](09-bugs.md)).

### Data to collect

| Data | Leopard 2A6 | Goes to | 2A6 prefab |
| --- | --- | --- | --- |
| Combat weight | 62.3 t | `RigidBody.Mass` | 62300 |
| Power and rpm | 1100 kW at 2600 rpm (MTU MB 873 Ka-501) | `MaxPower`, `RpmMaxPower` | 1100, 2600 |
| Max torque | \~4700 N·m at \~1600 rpm | `MaxTorque`, `RpmMaxTorque` | 4700, 1600 |
| Idle, cutoff, limit | 700 / 2700 / 3000 rpm | `RpmIdle`, `RpmRedline`, `RpmMax` | 700, 2700, 3000 |
| Gears | Renk HSWL 354: 4 forward, 2 reverse | `Forward`, `Reverse` (the sim accepts only one reverse) | 6.0 3.3 1.8 1.0, 2.3 |
| Top speed | \~70 km/h forward, \~31 km/h in reverse | `Ratio` calculation, reverse script cap | 4.6, 31 km/h |
| Drive sprocket radius | 0.34 m at the link ground face: the track's inner face wraps the sprocket at \~0.26 m, plus the 0.076 m link | `Sprocket.Radius` | 0.34 |
| Road wheels | 7 per side, rim radius 0.345 m + about one link thickness | `RoadWheel.Radius` | 0.418 |
| Idler | Rim radius 0.264 m + about one link thickness | `Idler.Radius` | 0.335 |
| Return rollers | 4 per side, rim radius 0.0847 m + about one link thickness | `Roller.Radius` | 0.152 |
| Road wheel positions | z = 2.478, 1.662, 0.889, 0.160, −0.722, −1.596, −2.478 m (front → rear); centre height 0.4186 m | Centre of mass levelling, suspension | From the bones |
| Track links | 71 per side on the model, pitch 0.2101 m, 0.076 m thick | `TrackLength`, `TrackThickness` | 0.2110, 0.076 |
| Track gauge | 2.656 m between belt centres | `TrackOffset1`, `TrackOffset2` | ∓1.3281 0 0 |
| Static sag | 0.097 m (follows from `SpringRate 450`) | `SpringRate`, `MaxTravelDown` | 450, 0.10 |
| Suspension travel up | \~350 mm | `MaxTravelUp` | 0.30 |
| Frontal area | \~3.75 m × 2.5 m | `ReferenceArea` | 9.5 |

The **sim radius** of every wheel is the distance from its centre to the track's **ground face** (the outer face of the links), not to the rubber rim. The engine places the wheel centre at the ground contact + `Radius`, and the link chain runs on these circles. See [Road wheels, idler, sprocket and return rollers](14-running-gear.md).

### 1. Mass and centre of mass

Use the real mass in kg. The 2A6 uses `Mass 62300`.

`CenterOfMass` is an **offset**, not a position. The executable describes it as an offset relative to the centre of mass defined in the model (reverse-engineered). If the hull model contains a `COM_` point, the physics centre of mass is that point plus the offset.

The 2A6 hull model has `COM_hull` at the centroid of the hull collider `UCX_hull_01`: (0, 1.168, −0.381) m in hull space, with the ground at y = 0. With `CenterOfMass 0 1 0.46`, the physics centre of mass sits at (0, 2.168, 0.079). This was measured: `WheelGetPosition` returns wheel centres relative to that point, and the numbers matched to 0.01 mm.

> The previous edition said to place `CenterOfMass` "low, near the hull floor" and gave `0 1 -0.3`. That advice is wrong when the model has a COM point: Y = 1 **raises** the centre of mass by 1 m above it.

#### Level the hull at rest

A hull that sits nose-up or nose-down on flat ground has its centre of mass ahead of or behind the middle of the road wheels. Fix it with the z offset:

1. Park on flat ground. Wait until the hull stops moving. The body then sleeps, and the telemetry lines repeat bit-identically.
2. Read the pitch: `LEOTEL pose pitch=` in degrees (nose up > 0), or P in the diag's Debug panel (radians; positive = nose down, observed in the video).
3. Compute the pitch sensitivity *S* from the spring rate and the road wheel positions (both sides):

```math
\theta = \frac{M \cdot g \cdot (\bar{z} - z_{\text{COM}})}{\sum_i k \,(z_i - \bar{z})^2}, \qquad S = \frac{M \cdot g}{\sum_i k \,(z_i - \bar{z})^2}\ \ [\text{rad/m}]
```

Here, *z<sub>i</sub>* are the road wheel positions along the hull, *z̄* is their mean, and *k* = `SpringRate` × 1000 N/m. Multiply *S* by 57.3 for degrees per metre.

4. Move the z offset of `CenterOfMass` forward by θ / *S* if the nose is up, or backward if it is down.
5. Reload properly (close the world, reload, reopen) and measure again.

The pitch at rest does not depend on `MaxTravelDown`: the same value appeared before and after the ride-height change.

On the 2A6: *z̄* = 0.056 m and Σ *k* (*z<sub>i</sub>* − *z̄*)² = 1.70 × 10⁷ N·m/rad, so *S* = 2.06°/m.

| `CenterOfMass` | Centre of mass z | Predicted pitch | Measured pitch |
| --- | --- | --- | --- |
| `0 1 0` | −0.381 m | +0.90° | +0.954° (nose up) |
| `0 1 0.46` | +0.079 m | −0.047° | −0.049° (median of 256 samples) |

The two measurements give 2.18°/m. The model puts the exact level at a z offset of 0.437; the prefab keeps 0.46.

**Open:** the Y offset of 1 was kept from earlier builds and has not been retuned. It puts the 2A6's centre of mass 2.17 m above the ground, about 0.47 m above the hull roof. Half the track gauge divided by that height gives a static stability factor of 0.61 g; with a Y offset of 0 (1.17 m) it would be 1.14 g (computed). One test run flipped at 33–41 km/h. Whether the high centre of mass caused it is not proven. The diag's *Show CoM* entry should draw the point (not tried).

**Update (2026-10-17).** The Y offset was lowered to 0.2 (`CenterOfMass 0 0.2 0.46`), which puts the centre of mass about 1.37 m above the ground and raises the static stability factor to about 0.97 g. The trigger was the grip rework in section 9b: once the rear road wheels had lateral grip, the 2A6 rolled over in a hard turn at speed with the old 2.17 m centre of mass. The pitch at rest did not change (it depends on z only).

**Open:** which point the engine uses when a model has no `COM_` point (collider centroid or model origin) is not known.

### 2. Engine

Copy power, torque and rpm values from the spec sheet. Then adjust the three values that are not on the sheet:

| Attribute | Rule | Leopard 2A6 |
| --- | --- | --- |
| `Friction` | About 10% of `MaxTorque`. It sets the engine braking | 470 |
| `Steepness` | 50 (vanilla engines use 4 to 50) | 50 |
| `Inertia` | Start at 3; higher values make the rpm rise slowly. It also enters the reverse bug's Δ | 3 |

With `Friction` 1400 (30% of the torque), the earlier 2A7 build accelerated slowly. Reverse could not even beat the engine's own friction.

**What `Friction` does.** With the throttle released, the Engine panel's torque fits this line within 2 N·m at ten readings (observed in video):

```math
T_{\text{engine braking}} = -\text{Friction} \cdot \frac{\text{rpm} - \text{RpmIdle}}{\text{RpmMax} - \text{RpmIdle}}
```

It stays at −471 N·m above `RpmMax`. Coasting in gear without the brake above 15 km/h, the 2A6 slowed at a median of 0.33 m/s² (0.65 in 2nd, 0.28 in 3rd; measured).

**Governor.** With `HasGovernor 1`, the torque is cut between `RpmRedline` and `RpmMax`. In 1st gear the rpm topped out at about 2930, with the torque down to 130–200 N·m (observed in video).

**Check.** In one clean standing start at thrust 0.8 (no turbo), the 2A6 reached 20 km/h in 4.1 s and 30 km/h in 9.7 s (measured).

### 3. Gearbox

Real tank transmissions have a torque converter. It multiplies the output torque by about 2 to 2.5 times. The simulation has no converter. Compensate by shortening the first gears.

Leopard 2A6: `Forward { 6.0 3.3 1.8 1.0 }`, `Reverse 2.3`. The reverse ratio only sets the native reverse cap and the reverse bug's Δ; the scripted push does the work ([Bugs and limitations](09-bugs.md)).

### 4. Final drive ratio from top speed

Pick the `Ratio` so that the top gear reaches the real speed at the max power rpm:

```math
R_d = \frac{\text{RpmMaxPower} \cdot \frac{2\pi}{60} \cdot r_{\text{sprocket}} \cdot 3.6}{g_{\text{top}} \cdot v_{\text{max}}}
```

Leopard 2A6 for 70 km/h: 2600 · 0.10472 · 0.34 · 3.6 / (1.0 · 70) ≈ 4.76. The prefab uses **4.6**. It was chosen when the sprocket radius was 0.3189 m (67.95 km/h at 2600 rpm) and was not re-tuned when the radius became 0.34 m for the link chain. With 0.34 m it gives 72.4 km/h at 2600 rpm.

Check the speed table for each gear (`Ratio` 4.6, sprocket 0.34 m):

| Gear | Ratio | At 2600 rpm | At 2700 rpm (`RpmRedline`) | Measured on the 2A6 |
| --- | --- | --- | --- | --- |
| 1st | 6.0 | 12.1 km/h | 12.5 km/h | Tops out at 13.6 km/h, \~2930 rpm (governor) |
| 2nd | 3.3 | 22.0 km/h | 22.8 km/h | — |
| 3rd | 1.8 | 40.2 km/h | 41.8 km/h | 44.3 km/h at 2860 rpm (formula: 44.3) |
| 4th | 1.0 | 72.4 km/h | 75.2 km/h | Not measured |

Automatic upshifts at thrust 0.8 happened at medians of 2924 rpm (1st → 2nd), 2812 rpm (2nd → 3rd) and 2860 rpm (3rd → 4th), above `RpmRedline` (measured).

**Top speed.** The 2A6 top speed has **not** been re-measured; the highest logged speed is 47.6 km/h. Theory gives about 72–75 km/h (2600–2700 rpm), and track slip takes a few percent off. The earlier 2A7 build (`Ratio` 4.9, sprocket 0.355 m) measured 74.3 km/h on flat ground, with the rpm going a little past 2600.

**Track slip.** The track speed is exactly the sprocket's angular velocity × 0.34 m: the diag's rope *Speed* / *AngVel* read 0.340 in every frame checked. The hull speed differs by the slip. Under power the tracks run 3–6% faster than the hull, about 10% when accelerating hard at 11 km/h, and about 20–110% in a pull-away below about 7 km/h; coasting, about 2–6% slower (observed in video). In `LEOTEL` above 10 km/h, the effective radius (hull speed / sprocket angular velocity) was about 0.32–0.33 m at thrust 0.8 in every gear, and about 0.35 m when coasting (measured).

![Launch with automatic upshifts, native vehicle debug on](../images/launch-upshifts.gif)

*Launch with automatic upshifts (rear-right 3/4 view). 1st gear tops out at 13.6 km/h and 2926 rpm, where the governor cuts the torque; the gearbox then shifts to 2nd and 3rd. In both rope boxes, Speed = AngVel × 0.34. Test video A (2026-10-06), 94.5–99.0 s · [full-size still](../images/launch-upshifts.jpg) at 97.0 s · Model: buh-late, CC BY 4.0, modified ([credits](../../CREDITS.md)).*

Conversion constants for reading telemetry (2A6, `Ratio` 4.6, no slip):

| Quantity | Per km/h |
| --- | --- |
| Engine rpm in 1st / 2nd / 3rd / 4th | 215.3 / 118.4 / 64.6 / 35.9 rpm |
| Engine rpm in reverse | 82.5 rpm |
| Sprocket (radius 0.34 m) | 7.802 rpm |
| Road wheel (`WheelGetRPM`, radius 0.418 m) | 6.346 rpm |

All parts of a track share one belt speed, so a road wheel's `WheelGetRPM` is the belt speed divided by its own `Radius`: road wheel rpm / (sprocket rpm from the engine feedback × 0.34 / 0.418) = 0.9999 (measured). The idler, sprocket and roller values were not checked.

### 5. Suspension springs and ride height

`SpringRate` is in **N/mm**, which is the same as kN/m. The executable's help text says so, and the measurements confirm it. The spring has **zero force at full droop** (`−MaxTravelDown`). So the static compression, measured from full droop, is:

```math
s = \frac{M \cdot g}{n \cdot k}
```

Here, *n* is the number of road wheels on both sides and *k* = `SpringRate` × 1000 N/m. Idler, sprocket and return rollers have no suspension.

Size the suspension in this order:

1. **Pick the static sag** *s*. It also fixes the heave frequency: *f* = (1/2π) · √(*g*/*s*). 2A6: *s* = 0.097 m → 1.60 Hz.
2. **Compute the spring rate:** *k* = *M* · *g* / (*n* · *s*). 2A6: 62300 · 9.81 / (14 · 0.097) ≈ 450 000 N/m → `SpringRate 450`.
3. **Set `MaxTravelDown` ≈ *s*.** The hull then sits at its modelled height. A larger value lifts the hull by the difference, because the spring still pushes over that extra travel.
4. **Set `MaxTravelUp`** to the room above the modelled wheel position (2A6: 0.30 m).
5. **Re-check the link chain slack** (section 9). Any ride-height change changes the path the chain must cover.

The suspension travel at rest, from the modelled position (+ = up), is:

```math
\text{travel}_{\text{rest}} = -\text{MaxTravelDown} + \frac{M \cdot g}{n \cdot k}
```

Measured on the 2A6 (`LEOSUSP`, mean travel of the logged road wheels in each frame, stopped; with 0.15 only the left track was logged):

| `MaxTravelDown` | Predicted rest travel | Measured | Hull |
| --- | --- | --- | --- |
| 0.15 m | −53.0 mm | −53.0 mm (13 of 14 frames) | 5.3 cm above its modelled height |
| 0.10 m | −3.0 mm | −3.0 mm (median) | At its modelled height |

With `CenterOfMass 0 1 0.46`, the rest travel per wheel runs from −1 mm at the front to −5 mm at the rear, the same on both tracks. The model predicts −1.0 to −5.1 mm.

More facts that matter for tuning (details in [Suspension and ride height](13-suspension.md)):

- Each road wheel casts a ray along the hull's down axis. It starts `RayStartOffsetUp` (0.5 m) above the fully compressed position.
- The engine's suspension signal is in metres, from −`MaxTravelDown` to +`MaxTravelUp`. Without ground contact the wheel hangs at full droop.
- The contact point can lie above full compression: contact-derived travel reached +0.382 m with `MaxTravelUp 0.30`. The engine clamps its own signal; scripts must clamp too.
- On sloped or uneven ground the mean travel of a track's wheels stays at the static value while single wheels spread (−27 to −88 mm with the hull pitched +0.35° and `MaxTravelDown 0.15`).

### 6. Dampers

The damper unit is N·s/m (force per 1 m/s). `CompressionDamper` acts while the suspension compresses, `RelaxationDamper` while it extends. Use a fraction ζ of critical damping:

```math
c = \zeta \cdot 2\sqrt{k \cdot m_{\text{wheel}}}, \qquad m_{\text{wheel}} = \frac{\text{Mass}}{n_{\text{wheels}}}
```

On the 2A6, each road wheel carries 4450 kg, so critical damping is about 89 500 N·s/m. With ζ = 0.3: *c* ≈ 27 000 N·s/m. The 2A6 uses 25 000 for compression (ζ ≈ 0.28) and 35 000 for rebound (ζ ≈ 0.39). These ratios are computed; the 2 s telemetry sampling is too coarse to confirm them.

**Check the hull attitude.** Under acceleration and braking the hull pitches about 0.20° per m/s² (measured, 395 samples): +0.94° nose-up at +2.66 m/s², −0.44° at −2.93 m/s², and once 1.85° nose-down at −4.3 m/s² with brake 6.0. In a hard stop the pitch peaks and then rebounds once by a small amount (observed in video, figure in section 7).

### 7. Brakes

Each wheel's `BrakeTorque` becomes a force at its own radius. Rule of thumb for a target deceleration *a*:

```math
\sum_i \frac{\text{BrakeTorque}_i}{\text{Radius}_i} \approx M \cdot a
```

On the 2A6: 14 · 6000 / 0.418 + 2 · 30 000 / 0.34 ≈ 201 kN + 176 kN = 377 kN, so about 6.1 m/s² (0.62 g). This is an estimate: it assumes each `TrackPart` brakes with its own torque at its own radius, which was not verified. The prefab sum is 14 · 6000 + 2 · 30 000 = 144 000 N·m.

Measured on the 2A6:

| Brake input (`GetBrake()`) | Deceleration | Source |
| --- | --- | --- |
| 6.0 (handbrake value, from 10–22 km/h) | 5.1–5.5 m/s² | `LEOTEL`, 7 samples |
| 6.0 (hard stops) | 5.0–5.7 m/s², G-Force 1.1–1.2 | Observed in video |
| 1.0 | 0.2–3.6 m/s² | `LEOTEL` |
| 0 (coasting in gear) | 0.33 m/s² (median) | `LEOTEL` |

During a hard stop the tracks skid: the rope speed falls well below the hull speed.

![Hard stop from 20 km/h with brake 6.0](../images/braking-dive.gif)

*Hard stop, right side. Brake 6.0 at 10.83 s takes the tank from 19.8 to 0.9 km/h in 1.0 s. P peaks at 0.05 rad nose-down and rebounds slightly. At 11.2 s the rope reads 1.535 m/s while the hull does 3.75 m/s: the tracks skid. Test video B (2026-10-06), 8.4–12.6 s · [full-size still](../images/braking-dive.jpg) at 11.2 s · Model: buh-late, CC BY 4.0, modified ([credits](../../CREDITS.md)).*

**Open:** how the brake input of 6.0 scales `BrakeTorque`. The controller holds 6.0 at standstill, in pivots and in hard stops.

### 8. Aerodynamics, retarder and controller

- `ReferenceArea` = width × height. 2A6: 9.5 m². `DragCoefficient` around 0.9 for a boxy hull.
- `RetarderBaseTorque 0` and `RetarderThrottleSpeed 20`, otherwise the retarder limits the speed (\~51–54 km/h on the earlier 2A7 build).
- `ThrottleReverseTarget 1`.

At walking pace there is almost no rolling resistance: idling in 1st with the throttle released, the 2A6 slowed only from 2.2 to 1.6 km/h in 17 s (observed in video). The script API has `WheelGetRollingDrag` and `WheelSetRollingDrag`; changing it was not tested.

### 9. Track link chain length and slack

The native link chain is a rope with gravity and no ground collision ([Track links](15-track-links.md)). Its length must cover the path around the wheel circles at every suspension position, and any extra length hangs as slack.

- The chain length is *N* × `TrackLength`. At runtime, *N* is the number of `TrackPositions` entries; `TrackSegments` is only read by the editor's generator.
- The path runs around every `TrackPart` circle at its **sim radius**, joined by outer tangents.

Size it like this:

1. Measure the link pitch on the model. 2A6: 0.2101 m (mean of 71 links).
2. Enter your wheel circles in `tools/trackpositions.py` and run `python tools/trackpositions.py 73 0.2101`. It prints the path length (`hull length`) and *N* × pitch. 2A6: 15.284 m.
3. *N* = round(path / pitch). 2A6: 15.284 / 0.2101 = 72.7 → 73.
4. Pick `TrackLength` with the table below. Drooped road wheels make the path longer.

2A6, all 7 road wheels of a side drooped by the same amount (arc length, computed with a Python port of the scripted chain's path code):

| Road wheel droop | Path length | Slack with 0.2101 (15.337 m) | Slack with 0.2110 (15.403 m) |
| --- | --- | --- | --- |
| 0 (rest) | 15.284 m | +5.3 cm | +11.9 cm |
| 3 cm | 15.319 m | +1.8 cm | +8.4 cm |
| 5 cm | 15.344 m | −0.6 cm | +6.0 cm |
| 10 cm | 15.407 m | −6.9 cm | −0.4 cm |
| 15 cm | 15.473 m | −13.5 cm | −7.0 cm |

Each extra 0.9 mm on `TrackLength` adds 73 · 0.0009 = 6.6 cm of chain. The rope is a polygon of straight segments, so it needs slightly less than the arc length: 73 equal chords around the rest path are 0.20894 m each (15.252 m). With that view, 0.2101 had about +8.5 cm at rest and −10.3 cm at 15 cm droop, and 0.2110 had +15.1 cm and −3.7 cm (computed, assuming rigid links). The conclusions are the same.

What happened on the 2A6:

| `TrackLength` | Rest droop | Result |
| --- | --- | --- |
| 0.2101 | 5 cm (`MaxTravelDown 0.15`) | About at its length limit at rest (computed: −0.6 cm as arc length, +2.5 cm as a polygon). After a flip (all wheels at 15 cm droop) it came off the sprocket and stayed off |
| 0.2110 | \~0 (`MaxTravelDown 0.10`) | 12 cm of slack hung below the road wheels and behind the sprocket. After a mine blast it came off again |

No single `TrackLength` keeps the native rope tight at rest and long enough at full droop. The 2A6 keeps `TrackLength 0.2110`, hides the native links and draws the scripted chain (`LEO_TrackChain`). The scripted chain spaces its links at path length / *N*, so the spacing follows the path: 0.2094 m at rest, 0.2110 m with all wheels at 10 cm droop, up to about 0.219 m with one middle wheel at full bump (about 4% stretch).

### 9b. Grip: the V2 friction attributes

`FrictionCurveLongitudinal` and `FrictionCurveLateral` are never read in 1.8.0.13 (reverse-engineered: the old friction path is switched off by a constant; only the lateral curve's existence is checked at load). The grip that runs is the "V2" model:

- each road wheel contact evaluates a **fixed car-tyre formula** (Magic Formula 5.2, nominal load 5 kN) with the wheel load, the slip ratio and the slip angle (`0x140a448a0`, tyre function `0x140a410a0`);
- the longitudinal force is multiplied by 2 × the surface's `FrictionTread` × `V2FrictionBaseLng`, the lateral force by 2 × `FrictionNonTread` × `V2FrictionBaseLat`;
- the lateral force is further multiplied by `V2FrictionSteerFactorCurve(|steering|)` and by `V2FrictionSteerFactorLngCurve(index / (n − 1))`, n = 13 `TrackPart`s per track.

At the Leopard's 43.7 kN per road wheel the car tyre is far outside its range: its lateral grip collapses to about 0.15–0.27 × load while the longitudinal grip still reaches 0.92 × load (computed from the transcribed formula). The vanilla position curve (1 / 0 / 1 at 0 / 0.5 / 1) maps the 7 road wheels to 0–0.5, so the rear road wheel gets **no** lateral grip at all. That was the "driving on ice" of the first Workshop tests: the rear slid out.

The 2A6 now uses:

| Attribute | Vanilla | Leopard 2A6 | Why |
| --- | --- | --- | --- |
| `V2FrictionSteerFactorLngCurve` | 1 / 0 / 1 at 0 / 0.5 / 1 | 1 / 0.5 / 1 / 1 at 0 / 0.25 / 0.5 / 1 | Symmetric over the road wheels: the rear holds as well as the front |
| `V2FrictionBaseLat` | 2 | 1.6 | 3 rolled the tank over in hard turns (lateral grip up to about 1.06 × weight on asphalt against a 0.97 g stability factor); 2.5 still did; 1.6 keeps it upright with about 26% more average lateral grip than vanilla |
| `V2FrictionSteerFactorCurve` | 1 → 0.2 | 1 / 0.35 / 0.08 at 0 / 0.5 / 1 | Less grip with the steering applied, so the pivot stays about as easy as before and mid-steering turns are more agile |

Tuning order: set the position curve first (it fixes the asymmetry), then `V2FrictionBaseLat` for the sideways hold, then the last value of the steering curve if pivots get sluggish on grass (lower it). Test a hard turn at 35–40 km/h, a quick lane change, a side slope and a pivot after every change. None of these values were measured against the per-wheel debug table; they come from the model and from the roll-over tests.

### 10. Measure with telemetry

1. Add `LEO_TelemetryComponent` to the prefab. Add `LEO_RunningGearComponent` and `LEO_TrackBeltComponent` if you use the scripted running gear ([Ready-made scripts](11-scripts.md)).
2. Drive this program: park on flat ground (engine off, then on), a standing start to top speed, a hard stop from about 20 km/h, reverse, a pivot turn, and rough ground.
3. Open the session log: `Documents\My Games\ArmaReforgerWorkbench\logs\logs_<date>\console.log`.
4. Filter the lines by tag:

| Tag | When | Fields | Good values |
| --- | --- | --- | --- |
| `LEOTEL pose` | Every 0.5 s, even with the engine off | `pitch`, `roll` in degrees (nose up > 0) | Level within \~0.1° on flat ground (section 1) |
| `LEOTEL v=` | Every 0.5 s with the engine on | `v` (km/h, forward > 0), `rpm`, `thrust`, `gear` (0 = R, 1 = N, 2… = 1st…), `clutch`, `simBrk` (6.00 = handbrake), `simRpm`, `wheelRpm` (gearbox-side rpm) | Speed per gear as in section 4 |
| `LEOSUSP` | Every 2 s with the engine on, one line per track | `worst_mm`; `diff_mm` per road wheel (bone − simulation, `air` without contact); `travel_mm` (simulation travel from the modelled height, + = up); `spin` of the 7 road wheels and the sprocket (degrees) | `diff_mm` 0–3 mm; `travel_mm` ≈ 0 at rest (2A6: −1 to −5 mm); equal spins within a track |
| `LEOGEAR` | Once, when the simulation becomes valid | `simulation: 2 tracks, WheelCount 13` | `WheelCount` = parts per track |
| `LEOTRACK` | Once | `link chain found (N links)` or `static belts kept` | 2A6: 146 links (native only) or 292 (native + scripted) |
| `LEOREV` | About once per second while the reverse key is held, and when the driver leaves | `APPLY force=… v=… gear=… clutch=… steer=…`, or the reason it does not push; `driver left: engine stopped` | `APPLY` only in R or N with the brake released |

5. Compare with the formulas:

| What to check | If it is wrong |
| --- | --- |
| Top speed lower than calculated | Retarder on, high `Friction` or weak engine. A few percent is track slip |
| Speed always capped at the same value | `RetarderThrottleSpeed` too low |
| RPM above `RpmMax` with `gear=0` | The reverse bug: 3766–3768 rpm on the 2A6, up to about 5100 with the brake applied. Normal with the scripted reverse |
| `thrust=0.80` with W | `ThrottleTurbo` 0.2. Normal |
| Speed per gear differs from the table | Wrong drive sprocket radius or `Ratio`. Under power the hull runs \~4% below the table (slip); coasting, \~2–5% above |
| Hull pitched at rest | Centre of mass z offset (section 1) |
| `travel_mm` at rest far below 0 | `MaxTravelDown` larger than the static sag; the hull rides high (section 5) |
| `diff_mm` large while all wheels touch the ground | The bones do not follow the simulation (for example `CarProcAnimComponent` mis-mapping), or the rest height in the script is wrong |
| `spin` differs within a track | With `LEO_RunningGearComponent` all road wheels of a track get the same spin by construction, so another component drives the bones. (The simulation's own `WheelGetRPM` per wheel may differ by a smooth front-to-rear gradient under 2%, and a wheel without contact can report a very different RPM) |
| Several identical lines in a row | The body is sleeping (`LinearSleepingThreashold` 0.4, `AngularSleepingThreashold` 0.5). Not a logger fault |
| A prefab change does not show | The world was not reloaded (see the top of this page) |
| RPM drops to 700 with `simBrk=6.00` in a pivot | Observed with the clutch engaged; the cause is open ([Bugs and limitations](09-bugs.md)) |

6. Cross-check with the native vehicle debug ([Vehicle debug diag and telemetry](16-debugging.md)): the rope *Speed* / *AngVel* must equal the sprocket radius (0.340 on the 2A6), and the Engine panel's *Feedback* equals sprocket rpm × gear ratio × `Ratio`.

---

[← Bugs and limitations (1.8.0.13)](09-bugs.md) · [Contents](../../README.md) · [Ready-made scripts →](11-scripts.md)
