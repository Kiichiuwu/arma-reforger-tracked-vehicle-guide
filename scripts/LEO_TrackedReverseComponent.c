//! LEO_TrackedReverseComponent v5: scripted reverse for tracked vehicles, multiplayer aware.
//!
//! Why the native reverse can never drive (Workbench/game 1.8.0.13):
//!   VehicleTrackedSimulation feeds the drivetrain load back to the engine as
//!       reaction = (sum of NON-NEGATIVE wheel loads) / (gearRatio * TripleDifferential.Ratio)
//!       engineRpm -= clutch * reaction / inertia * dt * 9.5493
//!   In reverse gearRatio = -|Reverse|, so the load SPEEDS the engine up. Rpm sticks at RpmMax + delta,
//!   where the torque curve is 0, so no reverse torque ever reaches the tracks.
//!   No config value fixes this: Reverse is always made negative and Differential Ratio must be > 0.
//!
//! Why multiplayer needs more (reverse-engineered, ArmaReforgerWorkbenchSteamDiag.exe 1.8.0.13):
//!   The server is the authority. The driver's machine predicts and sends a move every frame with steer, throttle,
//!   brake, handbrake, gear and clutch (NwkTrackedMovementComponent); the server replays it and corrects the driver
//!   when their states differ.
//!   - The move's handbrake is read with IsHandbrakeOn, a stub that always returns false (0x140fc23c0), and the server
//!     replays SetBreak(min(brake, 1), false) (0x140fc3c10). Pivot and neutral steering only run with the handbrake
//!     flag set (0x140ea7117), so a server never turns in place: the driver's machine pivots, the server holds the
//!     tank, and the driver rubberbands.
//!   - A script force exists only where it is applied. v4 pushed only on the driver's machine.
//!   After a correction the driver re-simulates its pending moves natively (0x141075cc0): no script and no handbrake
//!   run there.
//!
//! What this does:
//!   - Driver's machine: while the local driver holds TrackedBrake (and not TrackedThrottle), the hull is stopped or
//!     already moving backwards, the native gearbox is in R (0) or N (1) and no brake is applied, push the hull
//!     backwards (full at standstill, fading to zero at the top reverse speed), with a yaw torque for steering.
//!     If the vehicle is remote-controlled by a server, also send {reverse, steer, handbrake} to the server by RPC.
//!   - Server, driver on another machine: apply the same push from the received state, put the lost handbrake back
//!     (SetBreak(1, true)) while the replayed brake sits at its clamp 1.0, and relax the reconciliation allowances while
//!     pivoting or reversing, because the driver's re-simulation can reproduce neither.
//!   - Other players' machines: nothing (they show the server's state).
//!   - Singleplayer and a driving listen host: as v4.
//!
//! Native calls used: GetGear, GetBrake, SetBreak, IsValid, WheelGetRPM. NEVER call VehicleTrackedSimulation.GetInputs():
//! its out-parameter binding writes through a null pointer. GetThrottle(), SetThrottle() and IsHandbrakeOn() are empty
//! stubs on the tracked simulation.
[ComponentEditorProps(category: "Leopard2A7", description: "Scripted reverse drive for tracked vehicles; restores the handbrake (pivot turn) on servers")]
class LEO_TrackedReverseComponentClass : ScriptComponentClass
{
}

class LEO_TrackedReverseComponent : ScriptComponent
{
	[Attribute("0.3", desc: "Tractive force at standstill as a fraction of the vehicle weight (g)")]
	protected float m_fLaunchForceG;

	[Attribute("31", desc: "Top reverse speed [km/h]. Keep it below the native track-speed cap in R: RpmMax / (Reverse * Differential Ratio) * 0.10472 * sprocket radius * 3.6")]
	protected float m_fMaxReverseSpeedKmh;

	[Attribute("1.5", desc: "Forward speed [km/h] above which reverse is never applied (the native sim brakes instead)")]
	protected float m_fForwardCutoffKmh;

	[Attribute("900000", desc: "Yaw torque [N.m] for steering while reversing at full steering input")]
	protected float m_fSteerTorque;

	[Attribute("0.45", desc: "Max yaw rate [rad/s] reached by steering while reversing")]
	protected float m_fMaxYawRate;

	[Attribute("1", desc: "Stop the engine when the driver seat becomes empty (on the server / in singleplayer). Without a driver the controller pulls the handbrake, which at standstill selects reverse, and the reverse bug pins the engine above RpmMax")]
	protected bool m_bStopEngineOnExit;

	[Attribute("1", desc: "Multiplayer: give the server the driver's handbrake, which NwkTrackedMovementComponent drops. Without it a server never pivots or neutral-steers")]
	protected bool m_bMirrorHandbrake;

	[Attribute("1", desc: "Multiplayer: relax the server's reconciliation allowances while pivoting or reversing")]
	protected bool m_bRelaxAllowance;

	[Attribute("1", desc: "Multiplayer: let the driver's machine predict the vehicle and re-simulate its pending moves after a server correction (NwkTrackedMovementComponent.SetPrediction). Off: the driver's machine only follows the server's corrections")]
	protected bool m_bClientPrediction;

	[Attribute("0", desc: "Print LEOREV / LEONET diagnostics (state changes, once per second while pivoting or reversing on a server, and hull position jumps on the driver's machine)")]
	protected bool m_bDebug;

	protected TrackedControllerComponent m_Controller;
	protected BaseCompartmentSlot m_PilotSlot;
	protected RplComponent m_Rpl;
	protected NwkTrackedMovementComponent m_Nwk;
	protected bool m_bHadDriver;
	protected float m_fDebugTimer;

	// server: last state received from the driver's machine, and the driver it belongs to
	protected float m_fNetReverse;
	protected float m_fNetSteer;
	protected bool m_bNetHandbrake;
	protected IEntity m_NetDriver;
	protected int m_iMirrored; // SetBreak(1, true) calls since the last debug line

	// driver's machine: last state sent
	protected float m_fSentReverse = -1;
	protected float m_fSentSteer;
	protected bool m_bSentHandbrake;
	protected float m_fSendTimer;
	protected bool m_bOwnerWarned;

	protected bool m_bRelaxed;
	protected float m_fRelaxHold;      // server: seconds the allowances stay relaxed
	protected float m_fHandbrakeHold;  // driver's machine: seconds the handbrake still counts as on

	// driver's machine: prediction switch and hull jump detector (debug)
	protected bool m_bPredictionApplied;
	protected bool m_bHaveLastPos;
	protected vector m_vLastPos;
	protected int m_iSnaps;
	protected float m_fMaxSnap;
	protected float m_fSnapTimer;

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Controller = TrackedControllerComponent.Cast(owner.FindComponent(TrackedControllerComponent));
		if (m_Controller)
			m_PilotSlot = m_Controller.GetPilotCompartmentSlot();

		m_Rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
		m_Nwk = NwkTrackedMovementComponent.Cast(owner.FindComponent(NwkTrackedMovementComponent));
		SetEventMask(owner, EntityEvent.SIMULATE | EntityEvent.POSTSIMULATE | EntityEvent.FRAME);
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsAuthority()
	{
		return !m_Rpl || !m_Rpl.IsProxy();
	}

	//------------------------------------------------------------------------------------------------
	protected IEntity GetDriver()
	{
		if (!m_PilotSlot && m_Controller)
			m_PilotSlot = m_Controller.GetPilotCompartmentSlot(); // compartments can register after OnPostInit

		if (!m_PilotSlot)
			return null;

		return m_PilotSlot.GetOccupant();
	}

	//------------------------------------------------------------------------------------------------
	//! Runs every frame (physics may sleep once nobody drives, so this is not done in EOnSimulate).
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!m_Controller || !IsAuthority())
			return; // the engine state and the received drive state belong to the server

		bool hasDriver = GetDriver() != null;
		if (m_bStopEngineOnExit && m_bHadDriver && !hasDriver && m_Controller.IsEngineOn())
		{
			m_Controller.StopEngine(false);
			if (m_bDebug)
				Print("LEOREV driver left: engine stopped");
		}

		if (!hasDriver)
			ClearNetState();

		m_bHadDriver = hasDriver;
	}

	//------------------------------------------------------------------------------------------------
	override void EOnSimulate(IEntity owner, float timeSlice)
	{
		if (!m_Controller)
			return;

		VehicleTrackedSimulation simulation = m_Controller.GetSimulation();
		if (!simulation || !simulation.IsValid())
			return;

		IEntity driver = GetDriver();
		if (!driver)
			return;

		bool authority = IsAuthority();
		bool localDriver = driver == SCR_PlayerController.GetLocalControlledEntity();
		float reverse;
		float steer;
		bool handbrake;
		if (localDriver)
		{
			InputManager input = GetGame().GetInputManager();
			reverse = input.GetActionValue("TrackedBrake");
			if (reverse < 0.1 || input.GetActionValue("TrackedThrottle") > 0.1)
				reverse = 0;

			steer = input.GetActionValue("TrackedSteeringRight") - input.GetActionValue("TrackedSteeringLeft");
			// only SetBreak(x, true) stores 6.0. The controller drops it for single frames during a pivot, so hold it
			// for a moment to keep the server's state (and the RPCs) steady
			if (simulation.GetBrake() > 5.9)
				m_fHandbrakeHold = 0.25;
			else
				m_fHandbrakeHold -= timeSlice;

			handbrake = m_fHandbrakeHold > 0;
			if (!authority)
			{
				ApplyPredictionSetting();
				ClientSnapCheck(owner, timeSlice);

				// the server needs the steering only while pivoting or reversing; 0.1 steps keep analog input from
				// sending an RPC every frame
				float netSteer = 0;
				if (handbrake || reverse > 0)
					netSteer = Math.Round(steer * 10) / 10;

				SendToServer(reverse, netSteer, handbrake, timeSlice);
			}
		}
		else if (authority && driver == m_NetDriver)
		{
			reverse = m_fNetReverse;
			steer = m_fNetSteer;
			handbrake = m_bNetHandbrake;
			MirrorHandbrake(simulation);
		}
		else
		{
			return; // another player's machine, or a server before the driver's first state arrived
		}

		bool reversing = ApplyReverse(owner, simulation, reverse, steer);
		if (authority && !localDriver)
		{
			bool pivoting = handbrake && Math.AbsFloat(steer) > 0.1;
			// keep the allowances relaxed a little after the pivot or reverse ends: the driver's re-simulation still
			// differs from the server until the hull settles
			if (reversing || pivoting)
				m_fRelaxHold = 0.5;
			else
				m_fRelaxHold -= timeSlice;

			RelaxAllowance(m_fRelaxHold > 0);
			if (reversing || pivoting)
				ServerDebug(simulation, reverse, steer, timeSlice);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! The move is consumed in the post-simulate phase; set the handbrake again here so it holds for the next vehicle
	//! step whichever order the script and the move replay run in.
	override void EOnPostSimulate(IEntity owner, float timeSlice)
	{
		if (!m_Controller || !IsAuthority() || !m_NetDriver)
			return;

		VehicleTrackedSimulation simulation = m_Controller.GetSimulation();
		if (!simulation || !simulation.IsValid())
			return;

		if (GetDriver() == m_NetDriver)
			MirrorHandbrake(simulation);
	}

	//------------------------------------------------------------------------------------------------
	//! Server only: the replayed move set SetBreak(min(6, 1), false). Put the handbrake back.
	protected void MirrorHandbrake(VehicleTrackedSimulation simulation)
	{
		if (!m_bMirrorHandbrake || !m_bNetHandbrake)
			return;

		float brake = simulation.GetBrake();
		if (brake > 0.999 && brake < 5.9)
		{
			simulation.SetBreak(1, true);
			m_iMirrored++;
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Same gates on every machine; gear and brake reach the server through the move.
	protected bool ApplyReverse(IEntity owner, VehicleTrackedSimulation simulation, float reverse, float steer)
	{
		if (reverse <= 0 || !m_Controller.IsEngineOn())
			return false;

		Physics physics = owner.GetPhysics();
		if (!physics)
			return false;

		vector forward = owner.GetTransformAxis(2);
		float speed = vector.Dot(physics.GetVelocity(), forward); // m/s, negative = backwards
		if (speed * 3.6 > m_fForwardCutoffKmh)
			return false;

		// In a forward gear the native wheel step treats backward-spinning wheels as a direction mismatch:
		// it zeroes their speed and applies 6x brake. Wait until the controller has shifted to R (0) or N (1).
		if (simulation.GetGear() > EVehicleGearboxGear.NEUTRAL)
			return false;

		// brake 6.0 means handbrake; any brake fights the push
		if (simulation.GetBrake() > 0.05)
			return false;

		float maxReverse = m_fMaxReverseSpeedKmh / 3.6;
		float fade = Math.Clamp(1 - (-speed) / maxReverse, 0, 1);
		physics.ApplyForce(-forward * (physics.GetMass() * 9.81 * m_fLaunchForceG * reverse * fade));

		// steering while reversing: same rotation sense as the steering keys going forwards
		if (Math.AbsFloat(steer) > 0.05)
		{
			vector up = owner.GetTransformAxis(1);
			float yawRate = vector.Dot(physics.GetAngularVelocity(), up);
			float torque = Math.Clamp((steer * m_fMaxYawRate - yawRate) / m_fMaxYawRate, -1, 1) * m_fSteerTorque;
			physics.ApplyTorque(up * torque);
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Driver's machine: send the state on change, and every 0.5 s as a keep-alive.
	protected void SendToServer(float reverse, float steer, bool handbrake, float timeSlice)
	{
		if (m_Rpl && !m_Rpl.IsOwner())
		{
			if (m_bDebug && !m_bOwnerWarned)
				Print("LEONET this machine drives but does not own the vehicle: the server cannot receive the drive state", LogLevel.WARNING);

			m_bOwnerWarned = true;
			return;
		}

		m_fSendTimer -= timeSlice;
		bool changed = handbrake != m_bSentHandbrake || Math.AbsFloat(reverse - m_fSentReverse) > 0.05 || Math.AbsFloat(steer - m_fSentSteer) > 0.05;
		if (!changed && m_fSendTimer > 0)
			return;

		m_fSendTimer = 0.5;
		m_fSentReverse = reverse;
		m_fSentSteer = steer;
		m_bSentHandbrake = handbrake;
		Rpc(RpcAsk_DriveState, reverse, steer, handbrake);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_DriveState(float reverse, float steer, bool handbrake)
	{
		bool wasActive = m_bNetHandbrake || m_fNetReverse > 0;
		m_fNetReverse = Math.Clamp(reverse, 0, 1);
		m_fNetSteer = Math.Clamp(steer, -1, 1);
		m_bNetHandbrake = handbrake;
		m_NetDriver = GetDriver();

		bool active = m_bNetHandbrake || m_fNetReverse > 0;
		if (m_bDebug && active != wasActive)
			Print("LEONET server got drive state: handbrake=" + handbrake.ToString() + " reverse=" + m_fNetReverse.ToString(4, 2) + " steer=" + m_fNetSteer.ToString(4, 2));
	}

	//------------------------------------------------------------------------------------------------
	//! Driver's machine, once: switch the client re-simulation off when the attribute says so.
	protected void ApplyPredictionSetting()
	{
		if (m_bPredictionApplied || !m_Nwk)
			return;

		m_bPredictionApplied = true;
		if (m_bClientPrediction)
			return;

		m_Nwk.SetPrediction(false);
		if (m_bDebug)
			Print("LEONET client prediction switched off");
	}

	//------------------------------------------------------------------------------------------------
	//! Driver's machine, debug: a hull position change far beyond what the velocity explains is a server correction.
	//! Printed once per second: how many steps jumped and the largest jump.
	protected void ClientSnapCheck(IEntity owner, float timeSlice)
	{
		if (!m_bDebug)
			return;

		vector pos = owner.GetOrigin();
		Physics physics = owner.GetPhysics();
		if (m_bHaveLastPos && physics)
		{
			float moved = vector.Distance(pos, m_vLastPos);
			float expected = physics.GetVelocity().Length() * timeSlice;
			if (moved > expected * 3 + 0.05)
			{
				m_iSnaps++;
				if (moved > m_fMaxSnap)
					m_fMaxSnap = moved;
			}
		}

		m_vLastPos = pos;
		m_bHaveLastPos = true;

		m_fSnapTimer += timeSlice;
		if (m_fSnapTimer < 1)
			return;

		m_fSnapTimer = 0;
		if (m_iSnaps > 0)
			Print("LEONET client hull jumps/s=" + m_iSnaps.ToString() + " max=" + m_fMaxSnap.ToString(5, 2) + " m");

		m_iSnaps = 0;
		m_fMaxSnap = 0;
	}

	//------------------------------------------------------------------------------------------------
	protected void ClearNetState()
	{
		m_fNetReverse = 0;
		m_fNetSteer = 0;
		m_bNetHandbrake = false;
		m_NetDriver = null;
		RelaxAllowance(false);
	}

	//------------------------------------------------------------------------------------------------
	//! Defaults of NwkTrackedMovementComponent (constructor 0x14106c700): speed-scaled distance |v| / 300 clamped to
	//! 0.001..1.5 m, angle 0.003, linear velocity 0.3, angular velocity 0.25. The relaxed values are starting points.
	protected void RelaxAllowance(bool relax)
	{
		if (!m_bRelaxAllowance || !m_Nwk || relax == m_bRelaxed)
			return;

		m_bRelaxed = relax;
		if (relax)
			m_Nwk.SetAllowance(false, 0.3, 0.05, 0.6, 0.3);
		else
			m_Nwk.SetAllowance(true, 300, 0.003, 0.3, 0.25);

		if (m_bDebug)
			Print("LEONET server allowances relaxed=" + relax.ToString());
	}

	//------------------------------------------------------------------------------------------------
	//! Server, once per second while pivoting or reversing for a remote driver. Counter-rotating road wheels
	//! (opposite WheelGetRPM signs) prove the server itself pivots.
	protected void ServerDebug(VehicleTrackedSimulation simulation, float reverse, float steer, float timeSlice)
	{
		if (!m_bDebug)
			return;

		m_fDebugTimer += timeSlice;
		if (m_fDebugTimer < 1)
			return;

		m_fDebugTimer = 0;
		string rpm = "?";
		if (simulation.TrackCount() >= 2 && simulation.WheelCount() > 0)
			rpm = simulation.WheelGetRPM(0, 0).ToString(6, 1) + " / " + simulation.WheelGetRPM(1, 0).ToString(6, 1);

		Print("LEONET server brake=" + simulation.GetBrake().ToString(4, 2) + " mirrored=" + m_iMirrored.ToString() + " wheelRpm L/R=" + rpm + " reverse=" + reverse.ToString(4, 2) + " steer=" + steer.ToString(4, 2) + " gear=" + simulation.GetGear().ToString());
		m_iMirrored = 0;
	}
}
