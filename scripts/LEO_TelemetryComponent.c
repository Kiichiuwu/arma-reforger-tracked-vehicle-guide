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

	[Attribute("0.4186", desc: "Modelled road wheel centre height (bone v_wheel_l01 at rest) [m], for the absolute suspension travel in LEOSUSP")]
	protected float m_fWheelRestHeight;

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

	protected float m_fSuspensionTimer;

	//------------------------------------------------------------------------------------------------
	//! Every 2 s with the engine on, per track (7 road wheels, bones v_wheel_<s>0X): how far each animated road wheel
	//! bone (model space, includes the suspension translation) sits from where the simulation has that wheel (ground
	//! contact + radius, hull space) in mm, the simulation's travel from the modelled height (mm, + = up), and the
	//! spin of each road wheel and the sprocket about the axle (degrees).
	//! With LEO_RunningGearComponent driving the bones, all diffs are ~0 and a track's road wheel spins are equal.
	//! With CarProcAnimComponent (1.8.0.13) they were not: most bones got another wheel's signals.
	//! Only called with the engine on: the wheel getters are only read from a loaded simulation.
	protected void LogSuspension(IEntity owner)
	{
		m_fSuspensionTimer += m_fInterval;
		if (m_fSuspensionTimer < 2)
			return;

		m_fSuspensionTimer = 0;
		VehicleTrackedSimulation simulation = m_Controller.GetSimulation();
		Animation anim = owner.GetAnimation();
		if (!simulation || !anim)
			return;

		for (int track = 0; track < 2; track++)
		{
			string side = "l";
			if (track == 1)
				side = "r";

			string diffs, spins, travels;
			float worst;
			vector mat[4];
			for (int i = 0; i < 7; i++)
			{
				TNodeId bone = anim.GetBoneIndex("v_wheel_" + side + "0" + (i + 1).ToString());
				if (bone == -1 || !anim.GetBoneMatrix(bone, mat))
				{
					diffs += " ?";
					spins += " ?";
					continue;
				}

				float spin = Math.Atan2(mat[1][2], mat[1][1]) * Math.RAD2DEG;	// rotation about the axle (X)
				spins += " " + spin.ToString(4, 0);
				if (!simulation.WheelHasContact(track, i))
				{
					diffs += " air";
					travels += " air";
					continue;
				}

				float simY = owner.CoordToLocal(simulation.WheelGetContactPosition(track, i))[1] + simulation.WheelGetRadius(track, i);
				float diff = (mat[3][1] - simY) * 1000;
				diffs += " " + diff.ToString(3, 0);
				travels += " " + ((simY - m_fWheelRestHeight) * 1000).ToString(3, 0);
				if (Math.AbsFloat(diff) > worst)
					worst = Math.AbsFloat(diff);
			}

			TNodeId sprocket = anim.GetBoneIndex("v_sprocket_" + side);
			if (sprocket != -1 && anim.GetBoneMatrix(sprocket, mat))
				spins += " sprocket " + (Math.Atan2(mat[1][2], mat[1][1]) * Math.RAD2DEG).ToString(4, 0);

			PrintFormat("LEOSUSP track=%1 worst_mm=%2 diff_mm=%3 travel_mm=%4 spin=%5", side, worst.ToString(3, 0), diffs, travels, spins);
		}
	}

	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		m_fTimer += timeSlice;
		if (m_fTimer < m_fInterval)
			return;

		m_fTimer = 0;
		// hull attitude even with the engine off (pitch > 0 = nose up), to tune the centre of mass
		vector ypr = owner.GetYawPitchRoll();
		PrintFormat("LEOTEL pose pitch=%1 roll=%2", ypr[1], ypr[2]);

		if (!m_Controller || !m_Controller.IsEngineOn())
			return;

		LogSuspension(owner);

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
