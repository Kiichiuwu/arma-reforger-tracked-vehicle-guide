> 🇧🇷 Português · [🇬🇧 English](../en/11-scripts.md)

[← Afinação com dados reais](10-tuning.md) · [Índice](../../README.pt-BR.md) · [Solução de problemas →](12-troubleshooting.md)

# Scripts prontos

Os dois componentes abaixo são os que rodam no Leopard. Ambos só usam a API vanilla.

### Como instalar

1. Salve cada arquivo em `scripts/Game/Vehicle/` dentro do seu mod.
2. Recompile os scripts com o *Play* **parado**.
3. Adicione os componentes no `components { }` do prefab do casco:

```
LEO_TrackedReverseComponent "{7A1D4C9E2B6F3085}" {
}
LEO_TelemetryComponent "{3C5E7A9B1D2F4608}" {
}
```

Troque o prefixo `LEO_` se for usar em outro mod, para não colidir com o Leopard.

### Atributos do componente de ré

| Atributo | Padrão | Função |
| --- | --- | --- |
| `m_fLaunchForceG` | 0.3 | Força parada, em frações do peso |
| `m_fMaxReverseSpeedKmh` | 31 | Velocidade máxima de ré; a força cai a zero nela |
| `m_fForwardCutoffKmh` | 1.5 | Acima desta velocidade à frente, o script não empurra |
| `m_fSteerTorque` | 900000 N·m | Torque de giro em ré |
| `m_fMaxYawRate` | 0.45 rad/s | Velocidade de giro máxima em ré |
| `m_bHoldNeutral` | 0 | Mantém o câmbio nativo em neutro durante a ré |
| `m_bDebug` | 1 | Escreve linhas `LEOREV` no log |

Ajuste `m_fSteerTorque` pela massa: o valor do Leopard serve para cerca de 65 t.

### LEO\_TrackedReverseComponent.c (ré por script, v4)

```c
//! LEO_TrackedReverseComponent v4 (from the reverse-engineering study in MODLOG / work/research2).
//!
//! Why the native reverse can never drive (Workbench/game 1.8.0.13):
//!   VehicleTrackedSimulation feeds the drivetrain load back to the engine as
//!       reaction = (sum of NON-NEGATIVE wheel loads) / (gearRatio * TripleDifferential.Ratio)
//!       engineRpm -= clutch * reaction / inertia * dt * 9.5493
//!   In reverse gearRatio = -|Reverse|, so the load SPEEDS the engine up. Rpm sticks at RpmMax + delta,
//!   where the torque curve is 0, so no reverse torque ever reaches the tracks.
//!   No config value fixes this: Reverse is always made negative and Differential Ratio must be > 0.
//!
//! What this does: while the LOCAL driver holds TrackedBrake (and not TrackedThrottle), the hull is stopped or
//! already moving backwards, the native gearbox is in R (0) or N (1) and no brake is applied, push the hull
//! backwards with a force (full at standstill, fading to zero at the top reverse speed).
//!
//! Native calls used: GetGear, GetBrake, SetGear (optional). All of these check the simulation state
//! (== loaded) before touching memory. NEVER call VehicleTrackedSimulation.GetInputs(): its out-parameter
//! binding writes through a null pointer (this was crash #1). GetThrottle(), SetThrottle() and IsHandbrakeOn()
//! are empty stubs on the tracked simulation (they always return 0 / do nothing / return false).
[ComponentEditorProps(category: "Leopard2A7", description: "Scripted reverse drive for tracked vehicles (native reverse cannot drive in 1.8.0.13)")]
class LEO_TrackedReverseComponentClass : ScriptComponentClass
{
}

class LEO_TrackedReverseComponent : ScriptComponent
{
	[Attribute("0.3", desc: "Tractive force at standstill as a fraction of the vehicle weight (g)")]
	protected float m_fLaunchForceG;

	[Attribute("31", desc: "Top reverse speed [km/h]. Keep it below the native track-speed cap in R: RpmMax / (Reverse * Differential Ratio) * 0.10472 * sprocket radius * 3.6 (about 35.6 km/h for 3000 / (2.3 * 4.9) and r = 0.355)")]
	protected float m_fMaxReverseSpeedKmh;

	[Attribute("1.5", desc: "Forward speed [km/h] above which reverse is never applied (the native sim brakes instead)")]
	protected float m_fForwardCutoffKmh;

	[Attribute("900000", desc: "Yaw torque [N.m] for steering while reversing at full steering input")]
	protected float m_fSteerTorque;

	[Attribute("0.45", desc: "Max yaw rate [rad/s] reached by steering while reversing")]
	protected float m_fMaxYawRate;

	[Attribute("0", desc: "While reversing, force the native gearbox from R to N (free-rolling tracks, engine not pinned above RpmMax). The controller tries to shift back to R, so this is off by default")]
	protected bool m_bHoldNeutral;

	[Attribute("1", desc: "Print LEOREV diagnostics to the log while the reverse key is held")]
	protected bool m_bDebug;

	protected TrackedControllerComponent m_Controller;
	protected BaseCompartmentSlot m_PilotSlot;
	protected float m_fDebugTimer;

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Controller = TrackedControllerComponent.Cast(owner.FindComponent(TrackedControllerComponent));
		if (m_Controller)
			m_PilotSlot = m_Controller.GetPilotCompartmentSlot();

		SetEventMask(owner, EntityEvent.SIMULATE);
	}

	//------------------------------------------------------------------------------------------------
	protected void Debug(string reason, float timeSlice)
	{
		if (!m_bDebug)
			return;

		m_fDebugTimer += timeSlice;
		if (m_fDebugTimer < 1)
			return;

		m_fDebugTimer = 0;
		Print("LEOREV " + reason);
	}

	//------------------------------------------------------------------------------------------------
	//! Returns the reverse input (0..1) when every condition holds, otherwise 0 (and logs why once per second).
	protected float GetReverseDemand(IEntity owner, float timeSlice, out float speed, out VehicleTrackedSimulation simulation)
	{
		InputManager input = GetGame().GetInputManager();
		float reverseInput = input.GetActionValue("TrackedBrake");
		if (reverseInput < 0.1)
			return 0; // key not held: stay silent

		if (input.GetActionValue("TrackedThrottle") > 0.1)
		{
			Debug("throttle also held", timeSlice);
			return 0;
		}

		if (!m_Controller || !m_Controller.IsEngineOn())
		{
			Debug("no controller / engine off", timeSlice);
			return 0;
		}

		// GetGear() returns 0 (= R) while the simulation is not loaded, so the engine check above must come first
		simulation = m_Controller.GetSimulation();
		if (!simulation)
		{
			Debug("controller has no simulation", timeSlice);
			return 0;
		}

		if (!m_PilotSlot)
			m_PilotSlot = m_Controller.GetPilotCompartmentSlot(); // compartments can register after OnPostInit

		if (!m_PilotSlot)
		{
			Debug("no pilot compartment found", timeSlice);
			return 0;
		}

		IEntity driver = m_PilotSlot.GetOccupant();
		if (!driver || driver != SCR_PlayerController.GetLocalControlledEntity())
			return 0; // only the driver's machine (which owns the vehicle physics) pushes

		Physics physics = owner.GetPhysics();
		if (!physics)
		{
			Debug("no physics", timeSlice);
			return 0;
		}

		speed = vector.Dot(physics.GetVelocity(), owner.GetTransformAxis(2)); // m/s, negative = backwards
		if (speed * 3.6 > m_fForwardCutoffKmh)
		{
			Debug("still moving forward: " + (speed * 3.6).ToString(5, 1) + " km/h", timeSlice);
			return 0;
		}

		// In a forward gear the native wheel step treats backward-spinning wheels as a direction mismatch:
		// it zeroes their speed and applies 6x brake. Wait until the controller has shifted to R (0) or N (1).
		int gear = simulation.GetGear();
		if (gear > EVehicleGearboxGear.NEUTRAL)
		{
			Debug("native gearbox still in forward gear " + gear.ToString(), timeSlice);
			return 0;
		}

		// brake input 6.0 means handbrake (SetBreak(x, true)); any brake fights the push
		float brake = simulation.GetBrake();
		if (brake > 0.05)
		{
			Debug("native brake applied: " + brake.ToString(4, 2), timeSlice);
			return 0;
		}

		return reverseInput;
	}

	//------------------------------------------------------------------------------------------------
	override void EOnSimulate(IEntity owner, float timeSlice)
	{
		float speed;
		VehicleTrackedSimulation simulation;
		float reverseInput = GetReverseDemand(owner, timeSlice, speed, simulation);
		if (reverseInput <= 0)
			return;

		// optional: keep the broken reverse drivetrain disengaged (gear index 1 = neutral, 0 = reverse)
		if (m_bHoldNeutral && simulation.GetGear() == EVehicleGearboxGear.REVERSE)
			simulation.SetGear(EVehicleGearboxGear.NEUTRAL);

		Physics physics = owner.GetPhysics();
		vector forward = owner.GetTransformAxis(2);
		float maxReverse = m_fMaxReverseSpeedKmh / 3.6;
		float fade = Math.Clamp(1 - (-speed) / maxReverse, 0, 1);
		float force = physics.GetMass() * 9.81 * m_fLaunchForceG * reverseInput * fade;
		physics.ApplyForce(-forward * force);

		// steering while reversing: same rotation sense as the steering keys going forwards
		InputManager input = GetGame().GetInputManager();
		float steer = input.GetActionValue("TrackedSteeringRight") - input.GetActionValue("TrackedSteeringLeft");
		float torque;
		if (Math.AbsFloat(steer) > 0.05)
		{
			vector up = owner.GetTransformAxis(1);
			float yawRate = vector.Dot(physics.GetAngularVelocity(), up);
			float wanted = steer * m_fMaxYawRate;
			torque = Math.Clamp((wanted - yawRate) / m_fMaxYawRate, -1, 1) * m_fSteerTorque;
			physics.ApplyTorque(up * torque);
		}

		Debug("APPLY force=" + force.ToString(8, 0) + " N v=" + (speed * 3.6).ToString(5, 1) + " km/h gear=" + simulation.GetGear().ToString() + " clutch=" + simulation.GetClutch().ToString(4, 2) + " steer=" + steer.ToString(4, 2), timeSlice);
	}
}
```

### LEO\_TelemetryComponent.c (telemetria para afinação)

Escreve uma linha `LEOTEL` a cada 0.5 s com o motor ligado. Na primeira vez, também registra a posição da torre e as matrizes de alguns ossos.

```c
//! Debug telemetry for tuning the tracked simulation: prints drivetrain state to the log while moving or revving.
//! Lines start with "LEOTEL" so they are easy to grep from console.log.
//! The simulation MUST be obtained through TrackedControllerComponent.GetSimulation() (as third-party mods do):
//! calling the same getters on owner.FindComponent(VehicleTrackedSimulation) crashed Workbench 1.8.0.13 with a
//! null write when the engine started.
[ComponentEditorProps(category: "Leopard2A7", description: "Prints tracked-sim telemetry (speed, gear, clutch, inputs) to the log")]
class LEO_TelemetryComponentClass : ScriptComponentClass
{
}

class LEO_TelemetryComponent : ScriptComponent
{
	[Attribute("0.5", desc: "Seconds between telemetry lines")]
	protected float m_fInterval;

	[Attribute("1", desc: "Also read gear/clutch/throttle/brake from the simulation obtained via the controller")]
	protected bool m_bReadSimulation;

	// simBrk 6.0 = handbrake (SetBreak(x, true)); gear 0 = R, 1 = N, 2.. = forward 1..n

	protected float m_fTimer;
	protected TrackedControllerComponent m_Controller;
	protected SignalsManagerComponent m_Signals;
	protected int m_iRpmSignal = -1;
	protected int m_iThrustSignal = -1;

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Controller = TrackedControllerComponent.Cast(owner.FindComponent(TrackedControllerComponent));
		m_Signals = SignalsManagerComponent.Cast(owner.FindComponent(SignalsManagerComponent));
		if (m_Signals)
		{
			m_iRpmSignal = m_Signals.FindSignal("engineRPM");
			m_iThrustSignal = m_Signals.FindSignal("engineThrust");
		}

		PrintFormat("LEOTEL init controller=%1 signals=%2", m_Controller, m_Signals);
		SetEventMask(owner, EntityEvent.FRAME);
	}

	protected bool m_bTurretLogged;

	//------------------------------------------------------------------------------------------------
	//! Logs where the turret entity really sits in hull space (expected from the model: 0 1.726 -0.301,
	//! i.e. Blender v_turret_slot (0, -0.301, 1.726) converted to Enfusion x, z, y).
	protected void LogTurretPlacement(IEntity owner)
	{
		IEntity child = owner.GetChildren();
		while (child)
		{
			if (Turret.Cast(child))
			{
				vector local = owner.CoordToLocal(child.GetOrigin());
				PrintFormat("LEOTEL turret local position %1 (expected 0 1.726 -0.301)", local);
				LogBones(child, {"v_gun_01", "v_muzzle", "gunner_idle", "gunner_getIn", "v_gunner_sight", "commander_idle", "v_commander_sight"});
				LogBones(owner, {"v_turret_slot", "v_wheel_l01", "v_sprocket_l"});
				m_bTurretLogged = true;
				return;
			}

			child = child.GetSibling();
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Logs model-space bone matrices (X, Y, Z axes and position). Identity axes = <1,0,0> <0,1,0> <0,0,1>.
	protected void LogBones(IEntity entity, array<string> names)
	{
		Animation anim = entity.GetAnimation();
		if (!anim)
		{
			PrintFormat("LEOTEL bones: %1 has no animation", entity);
			return;
		}

		foreach (string name : names)
		{
			TNodeId bone = anim.GetBoneIndex(name);
			vector mat[4];
			if (bone == -1 || !anim.GetBoneMatrix(bone, mat))
			{
				PrintFormat("LEOTEL bone %1 NOT FOUND", name);
				continue;
			}

			PrintFormat("LEOTEL bone %1 X=%2 Y=%3 Z=%4 pos=%5", name, mat[0], mat[1], mat[2], mat[3]);
		}
	}

	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		m_fTimer += timeSlice;
		if (m_fTimer < m_fInterval)
			return;

		m_fTimer = 0;
		if (!m_bTurretLogged)
			LogTurretPlacement(owner);

		if (!m_Controller || !m_Controller.IsEngineOn())
			return;

		float kmh;
		Physics physics = owner.GetPhysics();
		if (physics)
			kmh = vector.Dot(physics.GetVelocity(), owner.GetTransformAxis(2)) * 3.6;

		float rpm = -1, thrust = -1;
		if (m_Signals && m_iRpmSignal != -1)
			rpm = m_Signals.GetSignalValue(m_iRpmSignal);
		if (m_Signals && m_iThrustSignal != -1)
			thrust = m_Signals.GetSignalValue(m_iThrustSignal);

		string line = "LEOTEL v=" + kmh.ToString(5, 1) + " rpm=" + rpm.ToString(4, 0) + " thrust=" + thrust.ToString(4, 2);

		if (m_bReadSimulation)
		{
			VehicleTrackedSimulation simulation = m_Controller.GetSimulation();
			if (simulation)
			{
				line += " gear=" + simulation.GetGear().ToString();
				line += " clutch=" + simulation.GetClutch().ToString(4, 2);
				line += " simBrk=" + simulation.GetBrake().ToString(4, 2);
				line += " simRpm=" + simulation.EngineGetRPM().ToString(4, 0);
				line += " wheelRpm=" + simulation.EngineGetRPMFeedback().ToString(4, 0);
				// NOT used: GetThrottle()/IsHandbrakeOn() are empty stubs on the tracked sim (always 0/false),
				// GetInputs() writes through a null pointer (crash).
			}
			else
			{
				line += " sim=null";
			}
		}

		Print(line);
	}
}
```

O comentário do topo atribui o crash 1 aos getters em geral. A investigação posterior mostrou que o culpado era só `GetInputs()`. Mesmo assim, pegar a simulação pelo controlador (`GetSimulation()`) é o caminho mais seguro.

### Regras para escrever os seus scripts

- Pegue a simulação com `TrackedControllerComponent.GetSimulation()`.
- Use `GetGear()`, `GetClutch()`, `GetBrake()`, `EngineGetRPM()` e `EngineGetRPMFeedback()`.
- **Nunca** use `GetInputs()` nem `GetFloorNormalAngles()`.
- `Physics` e `Animation` só podem ser variáveis locais. Como membro da classe, o compilador recusa.
- Leia entradas com `GetGame().GetInputManager().GetActionValue("TrackedBrake")` e afins.

---

[← Afinação com dados reais](10-tuning.md) · [Índice](../../README.pt-BR.md) · [Solução de problemas →](12-troubleshooting.md)
