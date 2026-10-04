> [🇧🇷 Português](../pt-BR/05-video-examples.md) · 🇬🇧 English

[← Step by step from scratch](04-from-scratch.md) · [Contents](../../README.md) · [Converting an existing vehicle →](06-converting.md)

# Video examples

These examples come from a 2 min 13 s test of the Leopard 2A7 mod, recorded in Workbench 1.8.0.13 on the airfield runway of the Arland map. Each example has a short GIF (or a still) and the exact moment in the original video.

**Reading the HUD.** Two dials at the bottom left matter here: the speedometer on the left (km/h, up to 120) and the tachometer in the middle (rpm × 1000, up to 3).

Every reverse, pivot, grass and turning moment was checked frame by frame against the ground (runway markings, cracks, grass), not against the camera, which the player orbits freely.

## Summary

| Example | Time in video | Illustrates |
| --- | --- | --- |
| [Forward acceleration from standstill](#accelerate-forward) | 14.75–19.25 s | [Tuning with real-world data](10-tuning.md) |
| [Scripted reverse](#scripted-reverse) | 27.75–31.75 s | [Bugs and limitations](09-bugs.md) |
| [Pivot in place](#pivot-in-place) | 37–42 s | [How it works inside](08-internals.md) |
| [Reverse with steering](#reverse-steering) | 45.25–50.25 s | [Bugs and limitations](09-bugs.md) |
| [Long run down the runway](#runway-run) | 73.5–77.75 s | [Tuning with real-world data](10-tuning.md) |
| [Steering while moving](#steer-while-moving) | 77.5–81 s | [How it works inside](08-internals.md) |
| [Driving off the runway onto grass](#offroad-grass) | 84.75–88.75 s | [Attribute reference](07-attribute-reference.md) |
| [Wheels and tracks not animated](#static-tracks) | 110.6–113.6 s | [Bugs and limitations](09-bugs.md) |

<a id="accelerate-forward"></a>

## Forward acceleration from standstill

![Forward acceleration from standstill](../images/accelerate-forward.gif)

With the engine idling, the tank pulls away from a standstill and drives forward along the runway (W). The speedometer (left gauge) reads about 13 km/h in this frame. The run reaches about 30 km/h around 22.5 s, and the gear changes are visible on the RPM needle.

*Video: 14.75–19.25 s · [full-size still](../images/accelerate-forward.jpg) (16.5 s) · Illustrates: [Tuning with real-world data](10-tuning.md)*

<a id="scripted-reverse"></a>

## Scripted reverse

![Scripted reverse](../images/scripted-reverse.gif)

Holding S at a standstill starts the scripted reverse: the hull moves toward its rear (the runway surface slides toward the front of the tank while the camera stays fixed) without changing heading. The RPM needle stays at the top of the scale. The speedometer reads about 16 km/h in this frame and reaches about 20 km/h before the tank stops. The speedometer gives no reverse indication.

*Video: 27.75–31.75 s · [full-size still](../images/scripted-reverse.jpg) (29.5 s) · Illustrates: [Bugs and limitations](09-bugs.md)*

<a id="pivot-in-place"></a>

## Pivot in place

![Pivot in place](../images/pivot-in-place.gif)

Pivot (neutral) steering: from a standstill the hull turns left on the spot (A at standstill). The camera stays locked behind the tank while the painted runway numbers circle around it at a nearly constant distance. The hull turns about 180 degrees between 35.6 s and 44.9 s with the speedometer at 0. During part of the turn the RPM needle drops to idle.

*Video: 37–42 s · [full-size still](../images/pivot-in-place.jpg) (40 s) · Illustrates: [How it works inside](08-internals.md)*

<a id="reverse-steering"></a>

## Reverse with steering

![Reverse with steering](../images/reverse-steering.gif)

Scripted reverse with steering. From a standstill the tank backs up (the painted runway letters come in from behind the camera and slide forward along the hull's right side) and reaches about 17 km/h. Then the hull swings left while it keeps moving backwards. The speedometer never drops to 0 during the manoeuvre, and the RPM needle alternates between the top of the scale and idle.

*Video: 45.25–50.25 s · [full-size still](../images/reverse-steering.jpg) (47.25 s) · Illustrates: [Bugs and limitations](09-bugs.md)*

<a id="runway-run"></a>

## Long run down the runway

![Long run down the runway](../images/runway-run.gif)

Long forward run down the runway while the camera orbits the moving tank (front, right side, rear). The speedometer reads about 50 km/h at 77.5 s, the highest speed in this video; another test measured a top speed of ~74 km/h.

*Video: 73.5–77.75 s · [full-size still](../images/runway-run.jpg) (77.5 s) · Illustrates: [Tuning with real-world data](10-tuning.md)*

<a id="steer-while-moving"></a>

## Steering while moving

![Steering while moving](../images/steer-while-moving.gif)

Steering while moving: coming off the fast run, the tank brakes and turns right at the end of the runway. With the camera locked behind the hull, the tree line and hangar slide across the frame from right to left. The speedometer falls from about 50 km/h to about 11 km/h by 80.5 s.

*Video: 77.5–81 s · [full-size still](../images/steer-while-moving.jpg) (78.75 s) · Illustrates: [How it works inside](08-internals.md)*

<a id="offroad-grass"></a>

## Driving off the runway onto grass

![Driving off the runway onto grass](../images/offroad-grass.gif)

Driving off the concrete onto the grass beside the runway: the white edge line passes under the hull and the tank carries on straight on grass past the yellow edge markers, at about 25 km/h on the speedometer. The terrain here is flat, so no suspension movement can be seen.

*Video: 84.75–88.75 s · [full-size still](../images/offroad-grass.jpg) (87 s) · Illustrates: [Attribute reference](07-attribute-reference.md)*

<a id="static-tracks"></a>

## Wheels and tracks not animated

![Wheels and tracks not animated](../images/static-tracks.gif)

Side view on a concrete pad at about 17 km/h on the speedometer, followed by a hard stop. The slab joints slide under the tank while the road wheels and track links stay identical from frame to frame: wheels and tracks are not animated (static mesh).

*Video: 110.6–113.6 s · [full-size still](../images/static-tracks.jpg) (111.2 s) · Illustrates: [Bugs and limitations](09-bugs.md)*

## What the video reveals about the tracked simulation

The test also showed tracked-simulation behaviour that does not appear in the log. It applies to any tank built with this guide, not only the Leopard.

| Problem | Where it shows | Status |
| --- | --- | --- |
| Road wheels and tracks do not turn | 16.5 s · 29.5 s · 74.5 s · 110.6–111.7 s | Expected: the native system does not animate the model ([details](06-converting.md)) |
| The HUD gives no reverse indication: the speedometer shows a positive value and the tachometer sits at the top | 28–31 s · 45.3–53.8 s | Side effect of the native reverse bug ([details](09-bugs.md)) |
| The scripted reverse reached ~20 km/h, below the configured 31 km/h | 28–31 s · 45.3–48.5 s | The force fades with speed; ~3 s pushes do not reach the cap |
| During the pivot turn the tachometer drops to idle while the hull keeps turning | 37.5–41.5 s · 113.5–115 s | Open |

---

[← Step by step from scratch](04-from-scratch.md) · [Contents](../../README.md) · [Converting an existing vehicle →](06-converting.md)
