> [🇧🇷 Português](../pt-BR/16-debugging.md) · 🇬🇧 English

[← Track links: the native rope chain and the scripted chain](15-track-links.md) · [Contents](../../README.md) · [Appendix →](17-appendix.md)

# Vehicle debug diag and telemetry

Two tools show what the tracked simulation really does:

- **The native vehicle debug diag.** It is built into the Diag executables. It draws the simulation's wheels, the link chain and the suspension over the vehicle, and it shows panels with the inputs, the gearbox and the engine.
- **The LEO telemetry lines.** The scripts of this guide write them to the log (`LEOTEL`, `LEOSUSP`, `LEOGEAR`, `LEOTRACK`). You can search them and compare them with the formulas afterwards.

This chapter also lists the Workbench traps that cost the most time while iterating on the Leopard.

Everything here was tested in Workbench 1.8.0.13 with the Leopard 2A6 example. Each fact says how it is known: **measured** (logs, files), **reverse-engineered** (static disassembly of `ArmaReforgerWorkbenchSteamDiag.exe` 1.8.0.13), **observed in the video**, **computed**, **from the Bohemia wiki** or **not tested**.

The videos are the ones listed in [Video examples](05-video-examples.md). *Test video A* is the 5:00 clip of 2026-10-06 (vehicle debug on, scripted wheels and links). *Test video B* is the 1:17 clip of the same session.

### In short

| Tool | What it shows | Where | Status |
| --- | --- | --- | --- |
| *Show vehicle debug* overlay | Wheel circles at the sim radius, the native rope, suspension markers, one stats box per track | Diag executable, *Play*, left `Win` + `Alt` | ✅ Works once a native link chain exists. ❌ It crashed Workbench on 2026-10-02, when there was none |
| *Debug* and *Engine* panels | Inputs, gear, speed, attitude; torque, power, rpm and drivetrain rpm (*Feedback*) | Same | ✅ |
| *Tracked* submenu | Chain switches: *Draw tracks*, *TrackSim*, *Track dynamic*, *Track gravity*, *Reset tracks* | Same | ⚠️ Only *Draw tracks* has a known effect |
| *Show Controller Diags* | Controller *Inputs* panel | Same | ✅ With a chain (one test) |
| `LEOTEL`, `LEOSUSP`, `LEOGEAR`, `LEOTRACK` | Drivetrain, hull attitude, suspension and animation checks, chain and rig checks | `console.log` | ✅ |

### You need a Diag executable

The diag menu exists only in Diag builds:

- The Workbench installs only `ArmaReforgerWorkbenchSteamDiag.exe` (`Arma Reforger Tools\Workbench\`), so *Play* in Workbench always has the menu (measured on the install folder).
- The game folder has `ArmaReforgerSteamDiag.exe` as well. The tracked-chain diag strings (*Draw tracks*, *Reset tracks*, the rope stats) are in both Diag executables. The retail `ArmaReforgerSteam.exe` has none of them (measured by a string scan).

It is a development tool: players on the retail game do not have it.

The vehicle also needs a **native link chain** before you switch on *Show vehicle debug*: `TrackPositions` with at least 2 entries and `TrackLength` greater than 0 ([chapter 15](15-track-links.md)). See [Crash history and a safe enabling order](#crash-history-and-a-safe-enabling-order).

### Opening and navigating the menu

1. Start *Play* and get in the vehicle as the driver.
2. Hold left `Win` + `Alt`. The diag menu appears near the top left of the viewport.
3. Keep holding them and use the arrow keys.

| Keys (while holding `Win` + `Alt`) | Action |
| --- | --- |
| `↑` / `↓` | Move the cursor. `Shift` moves faster |
| `←` / `→` | Change the selected value |
| `→` on a submenu | Open it |
| `Backspace` | Leave the submenu |
| `Home` | Jump to the root menu |
| `Insert` / `Del` | Reset the options of this menu / of all menus |
| `F1` / `F2` | Save the options to the user profile / load them |

The keys come from the Bohemia wiki page [Arma Reforger: Diag Menu](https://community.bistudio.com/wiki/Arma_Reforger:Diag_Menu). The left `Win` + `Alt` shortcut was used in every session of this work. The videos show the cursor moving and values switching; the other keys were not checked one by one.

The vehicle entries are under *Vehicles*. The cursor line is yellow and submenu names are purple.

### Where the entries are

![Diag menu open over the Leopard 2A6: Helicopter and Tracked submenus, then Show stats to Show Compartment Events with their values; Debug and Engine panels at the top left](../images/diag-menu-tracked.jpg)

*The Vehicles list of the diag menu over the stopped Leopard 2A6 (test video A, 2026-10-06, 0:41). Helicopter and Tracked, in purple, are submenus. The entries below them belong to the Vehicles list itself. True at this moment: Show vehicle debug, Player vehicle only, Show engine, Show contacts, Show suspension, Wheel engine brake and MT simulation. The Debug and Engine panels are at the top left and the rope stats boxes are under the menu. Model by buh-late, CC BY 4.0, modified.*

The Vehicles list has, from top to bottom:

1. The submenus *Helicopter* and *Tracked*.
2. The entries *Show stats* … *Show Compartment Events* (table below).
3. The submenus *Portals*, *Vehicle Networking and Reconciliation* and *Compartments*.
4. *Show dust materials*, *Fuel consumption*, *Visualize push force* and *Collisions debug prints*. They are not specific to tracked vehicles and were not used.

**Correction to the earlier notes.** The diary of 2026-10-02 wrote the path as *Vehicles > Tracked > Show vehicle debug*. The 2026-10-06 video shows that *Show vehicle debug* and the other *Show* entries are in the Vehicles list. *Tracked* is a separate submenu with six entries (observed in the video at 0:11, 3:18 and 3:21).

#### The Vehicles list

"Tried" means it was switched during test video A (times are clip times). There was no crash in any of these tests; the native chain was present.

| Entry | Value seen | What it did on the 2A6 | Tested |
| --- | --- | --- | --- |
| *Show stats* | false | — | Not tried |
| *Show vehicle debug* | true | Turns on the vehicle debug. The overlay, the rope stats boxes and the *Debug* panel were on whenever it was on | ✅ On in every frame |
| *Player vehicle only* | true | By its name, limits the debug to the vehicle you drive. Never switched off | On |
| *Show CoM* | false | — | Not tried |
| *Show inertia* | true at 0:45 | A white box around the hull and an `Inertia: 327602.7, 395692.4, 94730.0` line in the *Debug* panel | Tried |
| *Show forces* | true at 0:50 | A red line straight up from every wheel. At 62.3 t the lines leave the top of the screen, so they cannot be read | Tried |
| *Show engine* | true | The *Engine* panel. The panel stayed visible while the entry was briefly false | On |
| *Show contacts* | true / false | Not isolated (see the markers below) | Toggled |
| *Show raycast* | false | Toggled at 0:59; no effect identified | Toggled |
| *Show suspension* | true | A vertical line at each road wheel centre | On |
| *Show swaybar* | false | — | Not tried |
| *Show wheels* | false | Briefly true between 1:00 and 1:29; no effect identified | Toggled |
| *Show bones* | false | — | Not tried |
| *Show slope* | false | — | Not tried |
| *Test raycast* | false | Briefly true between 1:00 and 1:29; no effect identified | Toggled |
| *Toggle signals* | disabled | — | Not tried |
| *Reset vehicle* | disabled, shortcut `lctrl+lalt+r` | — | Not tried |
| *Teleport up* | disabled, shortcut `lctrl+lalt+t` | — | Not tried |
| *Wheel drag set* / *Wheel drag value* | false / 0 | — | Not tried |
| *Disable freelook* | false | — | Not tried |
| *Enable debugger trace* | false | Briefly true between 1:00 and 1:29; no effect identified | Toggled |
| *Force vehicle active* | false | — | Not tried |
| *Show hidden properties* | true from about 1:18 | No effect identified | Toggled |
| *Wheel engine brake* | true | On in every recorded session; effect not isolated | On |
| *MT simulation* | true | On in every recorded session; effect not isolated | On |
| *Show Controller Diags* | true at 1:25 | Opens the controller *Inputs* panel | Tried |
| *Show Compartment Events* | false | — | Not tried |

The *Inputs* panel of *Show Controller Diags* lists `SteeringTarget`, `Steering`, `ThrottleTarget`, `Throttle`, `BrakeTarget`, `Brake` (each marked *digital* for keyboard input), `HandbrakeP`, `Handbrake`, `State`, `Clutch`, `Selection` (`D`, with the previous one), `GearboxState`, `GearW`, `EngineSt`, `EngineStSt`, `StartupSt` and an *Enable input overrides* box (observed in the video at 1:25). The overrides were not used.

The *Inertia* values are consistent with a solid 62 300 kg box 3.96 m wide, 1.60 m high and 7.78 m long. The three values are the moments about the lateral (pitch), vertical (yaw) and longitudinal (roll) axes. This is computed from the panel values; how the engine derives the inertia was not checked.

#### The *Tracked* submenu

| Entry | Value seen | What it did on the 2A6 | Tested |
| --- | --- | --- | --- |
| *Steering mode* | wheeled | It stayed *wheeled* when selected (0:25–0:27) | Value did not change |
| *TrackSim* | true | False for a few seconds (0:20–0:23, 3:23): nothing visible changed, and the stats boxes kept updating | Toggled |
| *Draw tracks* | true | Switches the chain drawing: the wheel circles, the rope lines and the stats boxes. The suspension markers stay | ✅ Tried (0:13–0:18, 3:26) |
| *Track dynamic* | true | — | Not changed |
| *Track gravity* | true | False from about 0:12 for a few seconds: nothing visible from the rear camera at 2 km/h | Toggled |
| *Reset tracks* | disabled | — | Not tried |

These entries belong to the tracked simulation component: their strings sit right before `VehicleTrackedSimulationPrefabData` in the executable (reverse-engineered). *Reset tracks* is the only reset for the native rope. The script API has none, so a rope that came off a wheel cannot be put back from script ([chapter 15](15-track-links.md)). The *TrackSim* and *Track gravity* tests were short and at walking pace with the native links hidden, so "no effect" only means no visible effect there.

### What the overlay draws

![Right side of the stopped Leopard 2A6 with the native debug overlay on both tracks: white wheel circles, red and green rope lines, link numbers at the sprockets and two stats boxes](../images/debug-overlay-both-tracks.jpg)

*Test video A (2026-10-06), 0:30, stopped on the runway: Gear R, Brake 6.0, RPM 700. Both ropes are drawn; the far track's overlay shows through the hull. The numbers 37 and 41 next to the rear sprockets are rope point indices. Both stats boxes read AngVel 0 and Speed 0. Model by buh-late, CC BY 4.0, modified.*

![Close right-side view of the stopped Leopard 2A6: the white road wheel circles reach the ground face of the links, the red rope line runs along the bottom of the scripted links, blue suspension lines at the wheel centres](../images/debug-overlay-stationary.jpg)

*Test video A, 1:49, stopped, close right side. The white circles are larger than the visible rims because they use the sim `Radius` (road wheel 0.418 m, rim 0.345 m). The red rope line with cyan ticks lies on the ground face of the scripted links, under every road wheel and around the sprocket and idler. Model by buh-late, CC BY 4.0, modified.*

| Element | Meaning | How it is known |
| --- | --- | --- |
| White 12-sided circles | One per `TrackPart` at its sim `Radius`: 13 per track on the 2A6 (7 road wheels, idler, sprocket, 4 return rollers), the same as `WheelCount()` | Observed in the video; radii from the prefab |
| Road wheel circles reaching below the rims | The sim `Radius` is the distance from the wheel centre to the track's ground face ([chapter 13](13-suspension.md)) | Observed; matches the telemetry |
| White spoke in each circle | Turns with the wheel | Observed |
| Red and green polyline with cyan ticks | The native rope, with a tick at each of its 73 points. The colour pattern changes from frame to frame and turns mostly green during hard braking. Its meaning is unknown | Observed |
| Number next to each sprocket | Counts down when driving forward and up in reverse. It wraps between 0 and 72 and freezes when the tank stops. It is most likely the index of the rope point the sprocket grips | Observed; meaning inferred from the reverse-engineered grip logic ([chapter 15](15-track-links.md)) |
| Vertical line at each road wheel centre | Suspension (*Show suspension*) | Observed |
| Red and green crosses at the wheel centres | Also at the idler, sprocket and rollers. They stay when *Draw tracks* is off. Whether *Show contacts* or *Show suspension* draws them was not isolated | Observed |
| One black box per track | Rope stats ([next section](#the-per-track-rope-stats-box)). Each box is drawn next to its own track; in a side view the lower box is probably the near track | Observed; the side assignment is judged from front and side views |
| Far track | Drawn through the hull | Observed |

The overlay still draws the native rope when its link entities are hidden. On the 2A6, `LEO_RunningGearComponent` only clears their `VISIBLE` flag; the rope keeps running ([chapter 15](15-track-links.md)).

What to check with it:

1. **Radii.** The road wheel circles must reach the ground face of your track links. If they stop at the rims or go below the links, `Radius` is wrong ([chapter 13](13-suspension.md), [chapter 14](14-running-gear.md)).
2. **Part count.** Count the circles: one per `TrackPart` on each side.
3. **Chain path.** At rest the rope line should lie on the ground face of your visible links. On the 2A6 it lies exactly on the scripted links (1:49), so the scripted chain runs where the native rope runs.
4. **Slack.** The top run of the hidden native rope moves up and down around the return roller line at 15–25 km/h, with the slack gathering in front of the rear sprocket (3:08–3:10 and 1:40). The scripted links are not affected. With the native links visible, this is the sag described in [chapter 15](15-track-links.md).

### The per-track rope stats box

The box uses these format strings from the executable: `Sim: %s`, `Substep: %d/%d`, `Momentum: %f`, `Energy: %f`, `AngVel: %f(%.2f)` and `Speed: %f`. The executable has two words for `Sim:`, `Paused` and `SubStepping` (reverse-engineered).

| Line | Example | Meaning | How it is known |
| --- | --- | --- | --- |
| `Sim:` | `Paused` | Unknown. It read `Paused` in every frame checked, also while driving at 27 km/h and while the rope visibly whipped | Observed in both videos |
| `Substep:` | `0/25` | Unknown. 25 is the rope's iteration count per substep in the executable; the first number was always 0 | Observed; the link to the 25 iterations is inferred |
| `Momentum:` | `0.000000` | Unknown; always 0 | Observed |
| `Energy:` | `0.000000` | Unknown; always 0 | Observed |
| `AngVel:` | `18.413(175.83)` | Sprocket angular velocity in rad/s, rpm in brackets. Negative in reverse | Measured |
| `Speed:` | `6.2605` | Chain speed in m/s = `AngVel` × sprocket `Radius` | Measured |

`Speed` / `AngVel` was 0.34000 in every sample read, which is the 2A6's `Sprocket` `Radius` 0.34. If your ratio is different, your sprocket radius is not what you think.

One cross-check at 1:45.1 in test video A:

| Source | Value |
| --- | --- |
| Rope box | `AngVel` 18.413 rad/s (175.83 rpm), `Speed` 6.2605 m/s → 6.2605 / 18.413 = 0.3400 |
| HUD | 23.0 kph, 3rd gear |
| `LEOTEL` at the same moment | `v=022.8` km/h |
| Engine | 175.83 × 1.8 × 4.6 = 1456 rpm, the engine rpm shown |

#### Chain speed against hull speed

The chain speed is the speed of the track relative to the hull. The hull speed is the panel's `Speed`. The difference is track slip (observed in the videos):

| Situation | Track against hull | Example |
| --- | --- | --- |
| Pulling away | +20 % to about +110 % | 0:32: tracks 0.998 m/s (3.6 km/h), hull 1.7 km/h |
| Accelerating | +3 to +10 % | 3:07: track 3.254 m/s, hull 10.7 km/h (2.97 m/s) |
| Steady | About equal | 0:59.9: 0.882 m/s = 3.18 km/h, hull 3.2 km/h |
| Engine braking | −2 to −6 % | 3:10: track 20.0 km/h, hull 20.8 km/h |
| Hard braking (Brake 6.0) | Far below: the tracks skid | Test video B, 0:11.2: track 1.535 m/s, hull 13.5 km/h (3.75 m/s) |

#### The two tracks in a turn

![Leopard 2A6 pulling away into a left turn with the overlay on: the two stats boxes show the outer track at about twice the speed of the inner track](../images/steering-track-speeds.gif)

*Test video A, 0:31–0:33, right side. Both tracks start at 2.934 rad/s (28.02 rpm). With Steering −0.5 at 0:32.75 the boxes read 9.678 rad/s (92.42 rpm, 3.29 m/s) and 4.940 rad/s (47.18 rpm), about 2:1. Their mean, 7.31 rad/s × 0.34 = 2.49 m/s (9.0 km/h), matches the 9.3 km/h hull speed. [Full-size still](../images/steering-track-speeds.jpg) (0:32.75). Model by buh-late, CC BY 4.0, modified.*

- **Steering sign.** Steering +1 is a right turn: in a pivot the left track runs forward and the right one backward. `Y` grows in right turns (observed from front and side views).
- **Full lock while driving.** The inner track runs at about 1/6 of the outer one: 99.08 rpm against 16.51 rpm at 2:29, the `SteeringRatio` 6 of the prefab (observed).
- **Neutral steer from a standstill.** The tracks turn in opposite directions at about ±0.52 m/s (1:50). As the turn goes on, the track running backward becomes faster and the hull creeps backward at −0.1 to −3.8 km/h (observed in both videos).

### The *Debug* panel

| Field | Meaning | How it is known |
| --- | --- | --- |
| *Options*, *Info* | Panel switches; not explored | — |
| `Steering` | −1 to 1; positive is a right turn | Observed |
| `Clutch` | 0 to 1. A reading of 1.0 does not prove the clutch is locked: at idle the rpm stays at 700 while `Feedback` differs | Observed |
| `Brake` | 0 to 1 when braking. Holding the brake key ramps it 0.7 → 1.0 → 6.0. It reads 6.0 when the tank is held at a standstill, in every pivot and in hard stops | Observed |
| `Throttle` | 0.8 with W (`ThrottleTurbo` 0.2), 1.0 with turbo and in the recorded pivots | Observed |
| `Gearbox Gear` | R, N or 1–4. In the log: `gear=0` is R, 1 is N, 2 is 1st | Observed, matched against the log |
| `Speed` | Hull speed in kph, negative in reverse | Observed |
| `Slope` | Read −1 % in every frame, even at `P` 0.10–0.12. Do not use it | Observed |
| `G-Force` | Total acceleration in g: 1.0 at rest, 1.1–1.2 when braking hard | Observed |
| `Inertia` | Only with *Show inertia* | Observed |
| `Y`, `P`, `R` | Yaw, pitch and roll. `Y` is in radians (it wraps at ±π). `P` is positive nose down, the opposite of `LEOTEL pose`. `P` and `R` are probably radians too (0.05 in a hard stop) | Observed; units inferred |

`Brake` 6.0 is the value the simulation stores for the handbrake: `SetBreak(x, true)` stores 6.0, so `GetBrake()` reads 6.0 (reverse-engineered). In a pivot with the hull almost stopped, the controller also holds 6.0 (measured in the log).

### The *Engine* panel

| Field | Meaning | How it is known |
| --- | --- | --- |
| `Running` | yes / no | Observed |
| `Load` | \|`Torque`\| / `MaxTorque` (4700). It has a `%` sign but is a 0–1 fraction: 3409 N·m → 0.73, 4601 N·m → 0.98 | Fit to the panel readings |
| `Torque` | Engine torque in N·m. Negative under engine braking (formula below) | Observed |
| `Power` | `Torque` × `RPM` × 2π/60, in kW: 3409 N·m at 2700 rpm → 964 kW; 4601 N·m at 700 rpm → 337 kW | Fit to the panel readings |
| `RPM` | Engine rpm. It does not drop below `RpmIdle` 700 | Observed |
| `Feedback` | Drivetrain-side rpm: sprocket rpm × gear ratio × final drive. It equals `RPM` when the clutch is locked | Observed ([next section](#the-feedback-check)) |
| Graph | Two scrolling traces, white and yellow. The white one seems to follow the engine rpm; not confirmed | Observed |

Engine braking with the throttle at 0 fits this formula within 2 N·m on ten readings, and stays at −471 N·m above `RpmMax` (observed in test video B):

```math
T_{\text{brake}} = -\text{Friction} \cdot \frac{\text{RPM} - \text{RpmIdle}}{\text{RpmMax} - \text{RpmIdle}}
```

On the 2A6 (`Friction` 470, 700 and 3000 rpm): −306 N·m at 2198 rpm, −78 N·m at 1082 rpm.

Between `RpmRedline` 2700 and `RpmMax` 3000 the governor cuts the torque. In 1st gear the tank tops out at 13.6 km/h and 2930 rpm with only 130–200 N·m left. The automatic gearbox then usually shifts up; once it stayed in 1st at 13.0–13.4 km/h (observed).

### The Feedback check

`Feedback` lets you check the gear ratios and the final drive the simulation really uses:

```math
\text{Feedback} = n_{\text{sprocket}} \cdot g \cdot R_d, \qquad n_{\text{sprocket}} = \text{AngVel} \cdot \frac{60}{2\pi}
```

*g* is the gear ratio (`Forward` 6.0 / 3.3 / 1.8 / 1.0, `Reverse` 2.3) and *R<sub>d</sub>* is the `TripleDifferential` `Ratio` 4.6. *n*<sub>sprocket</sub> is the rpm in brackets in the rope box.

| Test video A | Gear | Rope box (rpm) | *g* · *R<sub>d</sub>* | Predicted | `Feedback` shown |
| --- | --- | --- | --- | --- | --- |
| 0:02 | 1 | 101.55 | 27.6 | 2803 | 2803 |
| 0:04 | 2 | 132.32 | 15.18 | 2009 | 2009 |
| 0:59.9 | 1 (coasting) | 24.77 | 27.6 | 684 | 682 |
| 2:16 | R | 75.43 | 10.58 | 798 | 797 |

Two special cases (observed in the video):

- **Neutral steer** (clutch 0, `Brake` 6.0): `Feedback` ≈ mean \|track rpm\| × `NeutralSteeringRatio` 12. At 0:00: (13.42 + 20.78) / 2 × 12 = 205; the panel shows 206.
- **Full lock while driving** with the clutch locked: `Feedback` = outer track rpm × 27.6 (2735 = 99.08 × 27.6 at 2:29).

![Leopard 2A6 left side pulling away and shifting from 1st to 2nd gear with the debug overlay and the Debug and Engine panels readable](../images/debug-pullaway.gif)

*Test video A, 0:00–0:04, left side. A neutral steer at 0:00 (clutch 0, tracks turning in opposite directions), then the pull-away with the clutch at 0.5 and 1.0 and the shift from 1st to 2nd, 0.4 to 18 kph. At 0:02 (13.2 kph, 1st gear) RPM = Feedback = 2803 and both boxes read 10.63 rad/s (101.55 rpm) and 3.615 m/s: 101.55 × 27.6 = 2803 and 3.615 / 10.63 = 0.34. [Full-size still](../images/debug-pullaway.jpg) (0:02). Model by buh-late, CC BY 4.0, modified.*

`LEOTEL` logs the same value as `wheelRpm` (`EngineGetRPMFeedback()`).

### Reading the panels: typical states

All values from test videos A and B on the 2A6 (observed).

| State | *Debug* panel | *Engine* panel | Rope boxes |
| --- | --- | --- | --- |
| Held at a standstill | `Brake` 6.0, `Clutch` 0.0, `Gearbox Gear` R or 1, `Speed` −0.0 | `RPM` 700, `Feedback` 0 | `AngVel` 0, `Speed` 0 |
| Pulling away in 1st | `Clutch` 0.4 (1:34) | `RPM` ≠ `Feedback` (812 / 283), `Torque` about 3700 N·m | Faster than the hull |
| Top of 1st gear | `Clutch` 1.0, 13.6 kph | `RPM` = `Feedback` ≈ 2930, `Torque` 130–200 N·m | 3.79 m/s |
| Coasting, throttle 0 | `Clutch` 1.0 | Negative `Torque` (formula above) | 2–6 % slower than the hull |
| Neutral steer from a standstill | `Steering` ±1, `Clutch` 0.0, `Brake` 6.0, `Throttle` 1.0 | `RPM` 2700, `Torque` 3409 N·m, `Power` 964 kW, `Feedback` ≈ 12 × mean track rpm | Opposite signs |
| Pivot with the clutch locked | `Clutch` 1.0, `Brake` 6.0 | `RPM` 700, `Torque` 4601 N·m, `Load` 0.98, `Power` 337 kW | Opposite signs, the backward one faster |
| Hard stop | `Brake` 0.7 → 1.0 → 6.0, `G-Force` 1.1–1.2, `P` +0.05 going forward, −0.04 in reverse | `RPM` falls to 700 | Far below the hull speed (skid) |
| Reverse (native bug) | `Gearbox Gear` R, `Clutch` 1.0 | `RPM` 3766–3768, `Torque` 0, `Feedback` = sprocket rpm × 10.58 | Negative |

The last row is the native reverse bug of [chapter 9](09-bugs.md): the engine sticks above `RpmMax` with no torque. The formula there predicts 3766.1 rpm for the 2A6. The tank still moved backward in these clips because `LEO_TrackedReverseComponent` pushes the hull ([chapter 11](11-scripts.md)). With the throttle released or braking in R, `RPM` climbed to 4300–4800.

### Crash history and a safe enabling order

**2026-10-02, about 07:04 local.** Turning on *Show vehicle debug* / *Show Controller Diags* (the diary names both) closed Workbench with an access violation (read of address `0x4`) in `PostFrame`. `logs\CrashReports.log` has the matching entry `2026-10-02 10:04:44 (UTC) : Version: 1.8.0.13 - CANCELED by user`. At that time the prefab had no `TrackPositions` and no `TrackLength`, so no link chain existed (measured).

**Why (reverse-engineered).** The vehicle debug draw (`0x140fbdf30`, slot 37 of the component's vtable) passes the two rope pointers (component + `0x1638` / + `0x1640`) to the rope draw function `0x140a41ce0` without a null check. With no chain, both pointers are null. A read at `0x4` fits a field read through a null pointer (inferred). The disassembly explains the vehicle debug draw only. Whether *Show Controller Diags* has its own crash is unknown.

**2026-10-06, 00:12–00:19.** With the native chain present (146 native links plus 146 scripted ones), the debug ran through both test videos without a crash. The `crash.log` of that Workbench run has only *Resources are leaking* asserts (measured). These were switched without a crash: *Show vehicle debug*, *Player vehicle only*, *Show engine*, *Show suspension*, *Show contacts*, *Show inertia*, *Show forces*, *Show raycast*, *Show wheels*, *Test raycast*, *Enable debugger trace*, *Show hidden properties*, *Show Controller Diags*, *Wheel engine brake*, *MT simulation*, and in *Tracked*: *TrackSim*, *Draw tracks* and *Track gravity*. *Steering mode* was selected but its value did not change (observed in the video).

Likely conclusion: the overlay is safe once a native chain exists (inferred). It was tested on one vehicle in one Workbench version.

**Safe enabling order.** This is a precaution, not a proven requirement:

1. Give the vehicle a **native** link chain: `TrackPositions` with at least 2 entries and `TrackLength` > 0 ([chapter 15](15-track-links.md)). A scripted chain alone does not count, because the overlay reads the engine's rope (reverse-engineered). Keep the native chain attributes even if you hide its links, as the 2A6 does.
2. Check that the rope exists. With `LEO_TrackBeltComponent` on the hull, the log shows `LEOTRACK link chain found (146 links)` (292 with the scripted chain).
3. Save the world and the prefab.
4. Start *Play*, get in as the driver and start the engine (as in the recorded sessions).
5. Open the menu. Turn on *Player vehicle only*, then *Show vehicle debug*.
6. Check the overlay. Then add *Show suspension* and *Show engine*.
7. Add other entries one at a time. Leave *Show forces* off on a heavy vehicle: the lines cannot be read.
8. Leave the untried entries alone (*Reset tracks*, *Track dynamic*, *Reset vehicle*, *Teleport up*, *Toggle signals*, *Force vehicle active*, *Wheel drag set* …) unless you are testing them, with your work saved.
9. Switch *Show vehicle debug* off before you drive a vehicle that has no native chain.

### *Reset vehicle* and *Teleport up*

The menu shows the shortcuts `lctrl+lalt+r` for *Reset vehicle* and `lctrl+lalt+t` for *Teleport up*. Both entries show the value *disabled* in the list. Neither was used in this work, so what they do to a tracked vehicle is not known. After the run-1 flip, the tank was set upright with the Game Master interface instead (observed in diagnostic video 1, 3:18–3:23). Whether either entry also puts back a native rope that came off the sprocket is unknown. *Reset tracks* in the *Tracked* submenu is the entry named for that, and it was not tried either.

### LEO telemetry

The scripts are in [chapter 11](11-scripts.md) and in `scripts/`. They print tagged lines that you can filter.

#### Where the log is

| File | Content |
| --- | --- |
| `Documents\My Games\ArmaReforgerWorkbench\logs\logs_<date>_<time>\console.log` | Everything, including the script lines. The folder is created when Workbench starts, and every *Play* session of that run goes into the same file. The 2A6 sessions from 2026-10-05 21:34 to 10-06 00:19 are all in `logs_2026-10-05_19-03-45` |
| `script.log`, same folder | Script output only, with the same `LEO` lines |
| `crash.log`, same folder | Asserts such as *Resources are leaking* |
| `logs\CrashReports.log` | One line per crash report |

The Workbench *Log Console* shows the same lines live.

Each *Play* start prints `LEOTEL init controller=… signals=…`. Use it to split the file into sessions. To filter:

```
grep -E "LEOTEL|LEOSUSP|LEOGEAR|LEOTRACK|LEOREV" console.log
```

```
Select-String -Path console.log -Pattern 'LEOTEL|LEOSUSP|LEOGEAR|LEOTRACK|LEOREV'
```

Real lines from the 2A6 session of 2026-10-06:

```
00:12:48.164   SCRIPT       : LEOGEAR simulation: 2 tracks, WheelCount 13
00:12:48.166   SCRIPT       : LEOTRACK link chain found (292 links): 2 static belt meshes hidden
00:16:00.385   SCRIPT       : LEOTEL pose pitch=-0.0834567 roll=-0.0047316
00:16:00.385   SCRIPT       : string line = 'LEOTEL v=022.4 rpm=1418 thrust=0.00 gear=4 clutch=1.00 simBrk=0.00 simRpm=1418 wheelRpm=1418'
00:16:00.895   SCRIPT       : LEOSUSP track=l worst_mm=001 diff_mm= -01 -00 -00 -00 000 000 001 travel_mm= 001 -00 -01 -03 -04 -06 -07 spin= 0145 0145 0145 0145 0145 0145 0145 sprocket -160
```

The drivetrain line is printed with `Print(line)`, so Enfusion writes it as `string line = '…'`. Search for `LEOTEL v=` to get only those lines.

#### `LEOTEL` (`LEO_TelemetryComponent`)

| Line | When |
| --- | --- |
| `LEOTEL init controller=… signals=…` | Once per vehicle, at start-up |
| `LEOTEL pose pitch=… roll=…` | Every `m_fInterval` (0.5 s), engine on or off |
| `LEOTEL v=… rpm=… …` | Every 0.5 s with the engine on |

| Field | Source | Meaning |
| --- | --- | --- |
| `pitch`, `roll` | `GetYawPitchRoll()` of the hull | Degrees. Pitch is positive nose up. Use it to level the hull ([chapter 13](13-suspension.md)) |
| `v` | Physics velocity along the hull's forward axis | km/h, negative in reverse |
| `rpm` | `engineRPM` signal | Can differ from `simRpm`: 1398 against 1154 at 00:16:00.9 |
| `thrust` | `engineThrust` signal | 0.80 with W, 1.0 with turbo |
| `gear` | `GetGear()` | 0 = R, 1 = N, 2 = 1st … 5 = 4th |
| `clutch` | `GetClutch()` | 0–1 |
| `simBrk` | `GetBrake()` | 0–1, 6.00 = handbrake |
| `simRpm` | `EngineGetRPM()` | Engine rpm (the panel's `RPM`) |
| `wheelRpm` | `EngineGetRPMFeedback()` | Drivetrain-side rpm (the panel's `Feedback`) |

The script never calls `GetThrottle()` or `IsHandbrakeOn()` (empty on the tracked simulation) or `GetInputs()` (it crashes Workbench). See [chapter 9](09-bugs.md).

#### `LEOSUSP` (`LEO_TelemetryComponent`)

One line per track every 2 s with the engine on. It compares each animated road wheel bone with the simulation's wheel.

| Field | Meaning |
| --- | --- |
| `track` | `l` or `r` |
| `worst_mm` | Largest \|`diff_mm`\| of the wheels in contact |
| `diff_mm` | Per road wheel, front to rear: bone centre − simulation wheel centre (contact point + `Radius`), in mm. `air` when the wheel has no contact |
| `travel_mm` | Per road wheel: simulation wheel centre − `m_fWheelRestHeight` (0.4186), in mm, positive up. Not clamped. `air` without contact |
| `spin` | Bone angle about the axle in degrees, 7 road wheels then `sprocket` |

`?` replaces a value when a bone is missing. The bone names are `v_wheel_<s>0<k>` and `v_sprocket_<s>`, with `<s>` = `l` / `r` ([chapter 14](14-running-gear.md)).

What good values look like on the 2A6 (measured):

| Field | Good | Not good |
| --- | --- | --- |
| `worst_mm`, `diff_mm` | 0 to 3 mm in normal driving: median 0, and 95 % / 90 % of samples within 3 mm in the 23:58 and 00:12 sessions | Up to about 124 mm only while a wheel eases back after losing contact (8/s easing): normal. Tens of mm all the time: the bones do not follow the simulation |
| `travel_mm` at rest, flat ground | −1, −2, −2, −3, −4, −4, −5 mm front to rear, the same on both sides (`MaxTravelDown` 0.10) | About −53 mm on every wheel with `MaxTravelDown` 0.15: the hull rode 5 cm high ([chapter 13](13-suspension.md)) |
| `travel_mm` on obstacles | Up to `MaxTravelUp` (+300) | Up to +382 mm was logged: the contact was above the bump stop. The bones are clamped there, so `diff_mm` grows too |
| `spin` | The 7 road wheels of a track equal. The sprocket has its own angle | A repeating pattern (odd wheels equal, even wheels equal): the bones take another wheel's signal |

How `LEOSUSP` caught the `CarProcAnimComponent` mis-mapping (run 2, 2026-10-05 22:55; an earlier version of the line, without `travel_mm`):

```
22:55:09.837   SCRIPT       : LEOSUSP track=l worst_mm=002 diff_mm= 000 001 001 001 001 002 002 spin= 0000 0000 0000 0000 0000 0000 0000 sprocket 0000
22:55:09.837   SCRIPT       : LEOSUSP track=r worst_mm=054 diff_mm= -04 051 051 052 053 053 054 spin= 0000 0000 0000 0000 0000 0000 0000 sprocket 0000
```

Right road wheels 2–7 sat 51–54 mm above the simulation's wheels: the floating wheels of diagnostic video 2. The full analysis is in [chapter 14](14-running-gear.md).

#### `LEOGEAR` (`LEO_RunningGearComponent`)

| Line | Meaning |
| --- | --- |
| `LEOGEAR simulation: 2 tracks, WheelCount 13` | Normal. Printed once, on the first frame the simulation is valid. `WheelCount` is per track (7 + idler + sprocket + 4 rollers). `TrackCount()` returns the constant 2 whenever the simulation is valid and 0 before (reverse-engineered) |
| `LEOGEAR no TrackedControllerComponent or no skeleton: running gear not animated` | The hull has no tracked controller or its model has no bones |
| `LEOGEAR <n> running gear bones missing or unreadable: the scripted chain is off` | Bone names do not match ([chapter 14](14-running-gear.md)) |
| `LEOGEAR the simulation has <n> wheels, expected …: running gear not animated` | The `TrackPart` count differs from `m_iRoadWheels` + 2 + `m_iRollers` |
| `LEOGEAR link model not found: … - the scripted chain is off` | Wrong `.xob` in the link model attribute |
| `LEOGEAR could not spawn all track links` | Link entities failed to spawn |

The simulation became valid 0.78–1.26 s after the vehicle entity was created, and 8–13 s before the engine was started (measured). The wheel getters work with the engine off from then on. `LEOSUSP` still waits for the engine, by choice of the script.

#### `LEOTRACK` (`LEO_TrackBeltComponent`)

| Line | Meaning |
| --- | --- |
| `LEOTRACK link chain found (146 links): 2 static belt meshes hidden` | The native rope exists (2 × 73 links); the hull's static belts are hidden |
| `LEOTRACK link chain found (292 links): …` | Native rope plus scripted chain (146 + 146) |
| `LEOTRACK no link chain after 5 s: static belts kept` | No link entity appeared: chain attributes or link model missing ([chapter 15](15-track-links.md)) |
| `LEOTRACK static belt mesh not found: <name>` | The mesh names in `m_sStaticBeltMeshes` do not exist in the hull model |

The component counts every hull child whose model name contains the link model name, native or scripted.

#### `LEOREV` (`LEO_TrackedReverseComponent`)

`LEOREV` lines explain why the scripted reverse does or does not push (`APPLY force=…`, `native brake applied`, `still moving forward` …). `LEOREV driver left: engine stopped` means `m_bStopEngineOnExit` stopped the engine. See [chapter 11](11-scripts.md).

#### Reading telemetry

- **Diag and log names.** The panel's `RPM` is `simRpm`, `Feedback` is `wheelRpm`, `Gearbox Gear` R / N / 1 is `gear` 0 / 1 / 2, `Brake` is `simBrk`, `Speed` is `v`. `P` is positive nose down; `LEOTEL pose` pitch is positive nose up.
- **Identical lines mean the body is asleep.** A stationary tank goes to sleep (`RigidBody` `LinearSleepingThreashold` 0.4, `AngularSleepingThreashold` 0.5). In run 1, 9 consecutive `LEOSUSP` samples and the pose lines were bit-identical (measured). Count such a series as one sample.
- **Aligning a video with the log.** Note the recording start time and check it once against a value on screen. Test video A starts at log time 00:14:14.8 (from the file's metadata). Check: Gear R and Brake 6.0 at 0:30 against `gear=0 … simBrk=6.00` at 00:14:44.9; HUD 23.0 kph at 1:45.1 against `v=022.8` at 00:15:59.9.

### Workbench iteration traps

| Trap | What happens | What to do | How it is known |
| --- | --- | --- | --- |
| Prefab edit with the world open | The vehicle in the open world keeps its old simulation values. In run 2 (2026-10-05 22:55) the wheels still rested at −51 to −54 mm, the `MaxTravelDown` 0.15 behaviour, although the prefab already had 0.10 | Close the world, reload scripts with no world open, then reopen the world. Run 3 (23:18) then rested at −3 mm | Measured (`LEOSUSP`, prefab backups) |
| Instance override in the layer | A `VehicleTrackedSimulation` block copied into the placed entity in the world layer replaces the prefab's whole block, so prefab edits never reach it | Remove the block from the layer | Observed (2026-10-05) |
| Script reload with a world open | Diag assert *Resources are leaking! Check log!* (`GameApp.cpp` line 1287), 2–3 s after *Reloading game scripts*. It lists 77 vanilla resources (terrain materials, import configs), none from the mod. Seven times on 2026-10-05. Workbench can also reload scripts by itself after a script file changes. The dialog can open on another monitor | Close the world before reloading. If the assert appears, choose *Ignore* | Measured (`crash.log`) |
| Script reload during *Play* | Workbench crashed (2026-10-02 06:34; the reload was sent through the Workbench NET API) | Stop *Play* first | Observed once |
| Replacing the hull `.fbx` with the world open | Workbench reimports a `.fbx` when its content changes. A hot reimport of the hull with the world open froze Workbench (one core at 100 %, a `FREEZE` dump) | Close the World Editor before replacing models. If it freezes, end the process and reopen. With no world open, a later hull reimport took 6.5 s and did not freeze | Observed |
| Editing only the `.xob.meta`, or rewriting an identical `.fbx` | No reimport | Resource Browser → right-click the `.xob` → *Reimport* | Observed |
| Opening a world | — | Double-click the `.ent` in the Resource Browser. If you drive Workbench through its NET API, opening the `.ent` there opened it as text | Observed |

### Multiplayer on one PC

The Workbench can host the server and launch extra game clients with the **Peer Tool** (Play mode *Server localhost + PeerTool*, plugin configured under *Plugins* → *Settings* → *Peer Tool*). The diag menu has network tools too: *Vehicle Networking and Reconciliation* (input plots, *SimState to CSV*) and *Network Movement* (a vehicle's replication state). Setup, pitfalls and what they showed on the Leopard are in [Multiplayer](18-multiplayer.md).

### Open questions

- What do `Sim: Paused`, `Substep: 0/25`, `Momentum` and `Energy` report? They never changed while driving. Would they read differently with the native links visible (no scripted chain)?
- What do *Track dynamic*, *Track gravity*, *TrackSim* and *Steering mode* change? Does *Reset tracks* re-seed the rope from `TrackPositions`?
- What do *Show CoM*, *Show stats*, *Show swaybar*, *Show bones*, *Show slope*, *Show wheels*, *Show raycast*, *Test raycast*, *Wheel drag set*, *Toggle signals*, *Reset vehicle*, *Teleport up*, *Wheel engine brake* and *MT simulation* do on a tracked vehicle?
- What do the rope line colours mean? Is the number at the sprocket the index of the gripped point?
- Which entry draws the red and green crosses at the wheel centres?
- Did *Show Controller Diags* crash on its own on 2026-10-02, or only together with the vehicle debug?
- Are `P` and `R` in radians? What are the two traces of the engine graph?
- Does the overlay work on multiplayer clients or with a dedicated server? Not tested.

---

[← Track links: the native rope chain and the scripted chain](15-track-links.md) · [Contents](../../README.md) · [Appendix →](17-appendix.md)
