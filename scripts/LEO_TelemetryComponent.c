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

	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		m_fTimer += timeSlice;
		if (m_fTimer < m_fInterval)
			return;

		m_fTimer = 0;

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
