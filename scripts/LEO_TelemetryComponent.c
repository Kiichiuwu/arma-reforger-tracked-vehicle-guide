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
