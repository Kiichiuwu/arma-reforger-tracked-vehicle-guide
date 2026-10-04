> [🇧🇷 Português](../pt-BR/08-internals.md) · 🇬🇧 English

[← Attribute reference](07-attribute-reference.md) · [Contents](../../README.md) · [Bugs and limitations (1.8.0.13) →](09-bugs.md)

# How it works inside

This section summarizes the reverse engineering of the 1.8.0.13 executable. Knowing the signal path helps you understand why each value matters.

### Driver inputs

The tracked vehicle has its own set of input actions, separate from the car:

| Action | Function |
| --- | --- |
| `TrackedThrottle` | Accelerate |
| `TrackedBrake` | Brake; when stopped, it becomes reverse |
| `TrackedSteeringLeft` / `TrackedSteeringRight` | Turn |
| `TurboToggle` / `TurboHold` | Turbo (100% throttle) |
| `HandBrake` / `HandBrakePersistent` | Handbrake |
| `EngineStart` / `EngineStop` | Start and stop the engine |

There is **no** action to shift gears, select reverse or use the clutch. The car has `CarShift` and `CarShiftReverse`; the tracked vehicle does not.

Inputs only go through if the driver is alive, is not using an item, and is not getting in, getting out or switching seats.

### The controller (SCR\_TrackedControllerComponent)

The controller turns the inputs into throttle, brake, gear and clutch.

**Throttle.** W delivers `1 − ThrottleTurbo`. With the default of 0.2, that gives 80%. The turbo unlocks the remaining 20%.

**Direction selection (automatic):**

- From **neutral** to **reverse**: brake above 1/32 and speed below 1.5 m/s. The switch is immediate.
- From **forward** to **reverse**: brake above 1/32, throttle below 1/32 and speed below 1.5 m/s for 0.5 s.
- In reverse, the pedals are swapped every frame. `TrackedBrake` becomes the throttle, with the value `ThrottleReverseTarget`.

**Gears.** Index 0 is reverse, index 1 is neutral, and the forward gears start at index 2. The first value of `Forward { }` is index 2.

**Clutch.** It is a multiplier from 0 to 1, driven by time (`ClutchCoupleTime`, `ClutchUncoupleTime`). `ClutchCouple*Rpm` only applies when pulling away. `MaxClutchTorque` is not used.

**Shifts.** Automatic shifts use the filtered RPM (`RpmSmoothing`), the slope (`SlopeSmoothing`) and the upshift and downshift factors.

### The simulation (VehicleTrackedSimulation)

On each substep, the simulation computes engine → clutch → gearbox → differential → tracks → wheels. Then it applies the forces to the `RigidBody`.

**Time step:**

```math
\Delta t = \frac{1}{60 \cdot \text{SolverSubsteps}} \quad (\text{4 substeps} \Rightarrow 1/240\ \text{s})
```

**Gearbox table.** Internally, the list becomes `[−|Reverse|, 0, Forward...]`. The reverse ratio is **always negative**, even if you write a negative value.

**Track speed.** It uses the **drive sprocket** radius (`Sprocket.Radius`), not the road wheel radius:

```math
v\,[\text{km/h}] = \frac{\text{rpm}}{g \cdot R_d} \cdot \frac{2\pi}{60} \cdot r_{\text{sprocket}} \cdot 3.6
```

Here, *g* is the gear ratio and *R\_d* is the `Ratio` of the `TripleDifferential`. Check on the Leopard: with *R\_d* = 6.0, telemetry showed 45 km/h at \~2000 rpm in 4th gear, and the formula gives 44.6 km/h.

**Load reaction on the engine.** The wheel load goes back to the engine through the same chain of ratios:

```math
\text{reaction} = \frac{\sum \text{load}_{\text{wheels}}}{g \cdot R_d}
```

```math
\text{rpm} \mathrel{-}= \frac{\text{clutch} \cdot \text{reaction}}{\text{Inertia} + \text{clutch} \cdot \text{InertiaCoupled}} \cdot \Delta t \cdot 9.5493
```

The load is always positive; when stopped, it equals the vehicle's weight. So the sign of *g* decides whether the load brakes or accelerates the engine. In reverse, *g* is negative, and the load **accelerates** the engine. The [next section](09-bugs.md) explains the effect.

**Engine torque.** The curve runs from `RpmIdle` to `RpmMax`, with its peak at `RpmMaxTorque`. At `RpmMax`, the torque is zero.

**Steering.** On the move, the differential interpolates up to `SteeringRatio` according to the steering input. When stopped, it uses `NeutralSteeringRatio`. `PivotSteering` locks the inner track; `NeutralSteering` makes it spin backward.

**Wheels.** Each wheel casts a ray from its bone, starting `RayStartOffsetUp` above it. The suspension computes the spring and the damper. Friction uses the `FrictionCurve*` curves and the `V2Friction*` values.

### Network (multiplayer)

`NwkTrackedMovementComponent` sends the driver's commands to the server, which runs them again. A force applied by script only on the driver's machine is **not** part of these commands. In multiplayer, it must also run on the server.

---

[← Attribute reference](07-attribute-reference.md) · [Contents](../../README.md) · [Bugs and limitations (1.8.0.13) →](09-bugs.md)
