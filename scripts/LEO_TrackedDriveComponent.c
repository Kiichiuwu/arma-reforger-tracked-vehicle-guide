//! LEO_TrackedDriveComponent (v6 of the reverse/handbrake script): server-authoritative drive for the NATIVE
//! VehicleTrackedSimulation (the vanilla prefab), built on what the scripted simulation taught us about 1.8/1.9
//! replication:
//!   - the engine hands a vehicle to the driver's client when they sit in, and the client then predicts the native
//!     tracked simulation. That owner path is unfinished for tracked vehicles (frozen head, sluggish look, camera jumps,
//!     sticky inputs, rubberband after every pivot: the move it sends carries handbrake = 0). Passengers, who only
//!     see the server's state, were always fine. So the server keeps the vehicle (RplComponent.GiveExt back) and the
//!     driver's client is a plain proxy like a passenger;
//!   - the server only accepts RplRcver.Server RPCs from an item's owner, so the driver's inputs travel through the
//!     player controller (modded SCR_PlayerController, LEO_PlayerController.c), 20 Hz and on change;
//!   - the server applies them straight to the native simulation every step (SetInputs for steer/throttle, SetBreak
//!     with the real handbrake flag, SetGear for reverse) and pushes the hull backwards in R (the native reverse gear
//!     never couples in 1.8.0.13: the drivetrain load feedback has the wrong sign in reverse, see v5);
//!   - a replicated vehicle on a client gets no entity events at all, so the clients animate the running gear from
//!     a small replicated state (belt speeds, wheel contact heights) in a call-queue tick instead.
//! Native calls used: SetInputs, SetBreak, GetBrake, GetGear, SetGear, EngineGetRPM, WheelGetRPM, WheelGetRadius,
//! WheelHasContact, WheelGetContactPosition, TrackCount, WheelCount, IsValid. NEVER call GetInputs(): its
//! out-parameter binding writes through a null pointer. GetThrottle/SetThrottle/IsHandbrakeOn are empty stubs.
[ComponentEditorProps(category: "Leopard2A7", description: "Vanilla tracked simulation driven server-side: driver inputs by RPC (handbrake kept), scripted reverse, server-owned vehicle (no client prediction), running gear state replicated to clients")]
class LEO_TrackedDriveComponentClass : ScriptComponentClass
{
}

class LEO_TrackedDriveComponent : ScriptComponent
{
	[Attribute("0.3", desc: "Reverse: tractive force at standstill as a fraction of the vehicle weight (g)")]
	protected float m_fLaunchForceG;

	[Attribute("31", desc: "Reverse: top speed [km/h]. Keep it below the native track-speed cap in R: RpmMax / (Reverse * Differential Ratio) * 0.10472 * sprocket radius * 3.6")]
	protected float m_fMaxReverseSpeedKmh;

	[Attribute("900000", desc: "Reverse: yaw torque [N.m] for steering while reversing at full steering input")]
	protected float m_fSteerTorque;

	[Attribute("0.45", desc: "Reverse: max yaw rate [rad/s] reached by steering while reversing")]
	protected float m_fMaxYawRate;

	[Attribute("2500", desc: "Automatic gearbox (the native one only shifts for an owner-driven vehicle): shift up above this engine rpm")]
	protected float m_fRpmShiftUp;

	[Attribute("1100", desc: "Automatic gearbox: shift down below this engine rpm")]
	protected float m_fRpmShiftDown;

	[Attribute("1", desc: "Stop the engine when the driver seat becomes empty (server)")]
	protected bool m_bStopEngineOnExit;

	[Attribute("1", desc: "Keep the vehicle owned by the server (take it back when the engine gives it to the driver's client). The driver's client never predicts the vehicle: no frozen head, no camera jumps, no rubberband; inputs go by RPC")]
	protected bool m_bServerOwned;

	[Attribute("1", desc: "Server: lock the native pilot controls while a driver sits in (the native controller still writes its own inputs over the RPC ones every step; kept as a belt-and-braces)")]
	protected bool m_bLockPilotControls;

	[Attribute("1", desc: "Server: detach the pilot seat from the native controller while a driver sits in, so the controller has no pilot and writes no inputs at all (without a pilot the simulation drives on SetInputs alone); the seat is given back when the driver leaves")]
	protected bool m_bDetachPilotSlot;

	[Attribute("0.05", desc: "Driver on a client: seconds between input packets to the server (also sent on every change)")]
	protected float m_fNetSendInterval;

	[Attribute("0.05", desc: "Server: seconds between replication updates of the running gear state for the clients")]
	protected float m_fNetStateInterval;

	[Attribute("0.418", desc: "Road wheel radius [m], for the clients' running gear when the simulation has not reported it yet")]
	protected float m_fWheelRadius;

	// ---- test inputs (headless experiments and the automated multiplayer test) ----
	[Attribute("0", desc: "Test input: the test schedule replaces the driver's keys once a driver sits in (automated multiplayer test). Without a driver the schedule is then NOT applied")]
	protected bool m_bTestDriverSchedule;

	[Attribute("0", desc: "Test input: throttle 0..1 applied after the test delay (no driver: on the server)")]
	protected float m_fTestThrottle;

	[Attribute("0", desc: "Test input: steering -1..1 applied after the test delay")]
	protected float m_fTestSteer;

	[Attribute("0", desc: "Test input: brake 0..1 applied after the test delay")]
	protected float m_fTestBrake;

	[Attribute("0", desc: "Test input: handbrake (pivot with the steering) while the test runs")]
	protected bool m_bTestPivot;

	[Attribute("5", desc: "Seconds after the first step before the test inputs apply")]
	protected float m_fTestDelay;

	[Attribute("10", desc: "Seconds the test inputs stay applied")]
	protected float m_fTestDuration;

	[Attribute("0", desc: "Test input: brake 0..1 applied right after the test window")]
	protected float m_fTestBrakeAfter;

	[Attribute("0", desc: "Test input: steering -1..1 applied right after the test window (throttle released)")]
	protected float m_fTestSteerAfter;

	[Attribute("5", desc: "Seconds the after-window phase lasts")]
	protected float m_fTestBrakeDuration;

	[Attribute("0", desc: "Print a LEONET line once per second (server) and a proxy heartbeat (clients)")]
	protected bool m_bDebug;

	protected TrackedControllerComponent m_Controller;
	protected BaseCompartmentSlot m_PilotSlot;
	protected RplComponent m_Rpl;
	protected NwkTrackedMovementComponent m_Nwk;
	protected LEO_RunningGearComponent m_Gear;
	protected bool m_bHadDriver;
	protected float m_fTime;
	protected float m_fDebugTimer;
	protected int m_iOwnershipTakebacks;
	protected float m_fOwnershipTakebackTime;

	// commands applied to the simulation this step (server)
	protected float m_fThrottle;
	protected float m_fBrake;
	protected float m_fSteer;
	protected bool m_bHandbrake;
	protected float m_fReverse;
	protected bool m_bEngineRequest;      // a start/stop request waits for the next step
	protected bool m_bEngineWanted;
	protected float m_fShiftTimer;        // gearbox lockout after a shift [s]

	// driver's machine: raw inputs
	protected bool m_bLocalDriver;
	protected float m_fDriverThrottle;
	protected float m_fDriverBrake;
	protected float m_fDriverSteer;
	protected bool m_bDriverHandbrake;
	protected float m_fDriveTime;
	protected float m_fAutoStartTimer;    // driver's machine: seconds before the next automatic start request
	protected float m_fSentThrottle;
	protected float m_fSentBrake;
	protected float m_fSentSteer;
	protected bool m_bSentHandbrake;
	protected float m_fSendTimer;
	protected int m_iSentPackets;

	// server: last inputs from a remote driver
	protected float m_fNetThrottle;
	protected float m_fNetBrake;
	protected float m_fNetSteer;
	protected bool m_bNetHandbrake;
	protected float m_fNetAge;

	// replicated running gear state (server writes, clients animate from it):
	// belts = (belt speed L, belt speed R, wheel radius) [m/s, m/s, m]; heights = contact height of each road wheel's
	// contact point in hull space, 14 values over 5 vectors, the 15th value a bit mask of the wheels in contact
	[RplProp()]
	protected vector m_vRplBelts;
	[RplProp()]
	protected vector m_vRplHeightsA;
	[RplProp()]
	protected vector m_vRplHeightsB;
	[RplProp()]
	protected vector m_vRplHeightsC;
	[RplProp()]
	protected vector m_vRplHeightsD;
	[RplProp()]
	protected vector m_vRplHeightsE;
	protected float m_fStateTimer;
	protected int m_iRoadWheels = 7;

	// client
	protected float m_fProxyLastTime;
	protected int m_iProxyTicks;
	protected vector m_vProxyLastPos;

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Controller = TrackedControllerComponent.Cast(owner.FindComponent(TrackedControllerComponent));
		if (m_Controller)
			m_PilotSlot = m_Controller.GetPilotCompartmentSlot();

		m_Rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
		m_Nwk = NwkTrackedMovementComponent.Cast(owner.FindComponent(NwkTrackedMovementComponent));
		m_Gear = LEO_RunningGearComponent.Cast(owner.FindComponent(LEO_RunningGearComponent));
		if (m_Nwk && m_bServerOwned)
			m_Nwk.SetPrediction(false);   // nobody predicts: the server drives, the clients interpolate

		SetEventMask(owner, EntityEvent.SIMULATE | EntityEvent.POSTSIMULATE | EntityEvent.FRAME);

		if (m_Rpl && m_Rpl.IsProxy())
		{
			// clients: no entity events reach a replicated vehicle, so the driver's inputs and the running gear
			// visuals run from the call queue
			if (m_Gear)
				m_Gear.SetNetSource(this);

			GetGame().GetCallqueue().CallLater(ClientTick, 50, true);
			GetGame().GetCallqueue().CallLater(ProxyTick, 0, true);
			if (m_bDebug)
				GetGame().GetCallqueue().CallLater(ProxyHeartbeat, 1000, true);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsAuthority()
	{
		return !m_Rpl || !m_Rpl.IsProxy();
	}

	//------------------------------------------------------------------------------------------------
	IEntity GetDriver()
	{
		if (!m_PilotSlot && m_Controller)
			m_PilotSlot = m_Controller.GetPilotCompartmentSlot();   // compartments can register after OnPostInit

		if (!m_PilotSlot)
			return null;

		return m_PilotSlot.GetOccupant();
	}

	// ---- replicated state for the running gear on clients ----
	int GetRoadWheels() { return m_iRoadWheels; }

	float GetWheelRadius()
	{
		if (m_vRplBelts[2] > 0.01)
			return m_vRplBelts[2];
		return m_fWheelRadius;
	}

	//! Belt speed over the ground face, + = forward [m/s]
	float GetBeltSpeed(int track)
	{
		if (track == 0)
			return m_vRplBelts[0];
		return m_vRplBelts[1];
	}

	bool WheelContact(int track, int k)
	{
		int mask = m_vRplHeightsE[2];
		return (mask & (1 << (track * m_iRoadWheels + k))) != 0;
	}

	//! Height of the wheel's contact point in hull space [m]
	float WheelContactHeight(int track, int k)
	{
		int i = track * m_iRoadWheels + k;
		if (i < 3)
			return m_vRplHeightsA[i];
		if (i < 6)
			return m_vRplHeightsB[i - 3];
		if (i < 9)
			return m_vRplHeightsC[i - 6];
		if (i < 12)
			return m_vRplHeightsD[i - 9];
		return m_vRplHeightsE[i - 12];
	}

	//------------------------------------------------------------------------------------------------
	//! Server, every frame: ownership, driver presence, local driver inputs (listen server host)
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!m_Controller || !IsAuthority())
			return;

		m_fTime += timeSlice;
		if (m_bServerOwned && m_Rpl && !m_Rpl.IsOwner() && m_fTime - m_fOwnershipTakebackTime > 0.5)
		{
			m_fOwnershipTakebackTime = m_fTime;
			m_Rpl.GiveExt(RplIdentity.Local(), false);
			m_iOwnershipTakebacks++;
			if (m_iOwnershipTakebacks <= 5)
				Print("LEONET vehicle ownership taken back by the server (" + m_iOwnershipTakebacks.ToString() + ")");
		}

		IEntity driver = GetDriver();
		bool hasDriver = driver != null;
		if (m_bStopEngineOnExit && m_bHadDriver && !hasDriver && m_Controller.IsEngineOn())
		{
			m_Controller.StopEngine(false);
			if (m_bDebug)
				Print("LEONET driver left: engine stopped");
		}
		if (hasDriver != m_bHadDriver)
		{
			if (m_bLockPilotControls)
				m_Controller.LockPilotControls(hasDriver);

			if (m_bDetachPilotSlot && m_PilotSlot)
			{
				if (hasDriver)
					m_Controller.SetPilotCompartmentSlot(null);
				else
					m_Controller.SetPilotCompartmentSlot(PilotCompartmentSlot.Cast(m_PilotSlot));
			}

			if (m_bDebug)
				Print("LEONET driver " + hasDriver.ToString() + ": pilot controls locked=" + m_Controller.ArePilotControlsLocked().ToString() + " controller pilot slot=" + (m_Controller.GetPilotCompartmentSlot() != null).ToString());
		}
		m_bHadDriver = hasDriver;

		// the hull's rigid body sleeps a couple of seconds after it settles and EOnSimulate stops with it; script
		// inputs do not wake it the way the native controller's do, so keep it awake while somebody drives
		bool testing = !m_bTestDriverSchedule && m_fTestThrottle > 0 && m_fTime >= m_fTestDelay;
		Physics physics = owner.GetPhysics();
		if (physics)
		{
			if ((hasDriver || testing) && !physics.IsActive())
				physics.SetActive(ActiveState.ALWAYS_ACTIVE);
			else if (!hasDriver && !testing && m_bHadDriver != hasDriver)
				physics.SetActive(ActiveState.ACTIVE);
		}

		bool wasLocal = m_bLocalDriver;
		m_bLocalDriver = driver && driver == SCR_PlayerController.GetLocalControlledEntity();
		if (m_bLocalDriver)
		{
			if (!wasLocal)
				m_fDriveTime = 0;
			m_fDriveTime += timeSlice;
			ReadDriverInputs(timeSlice);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! The driver's machine reads the tracked context (kept active by the native controller for the pilot)
	protected void ReadDriverInputs(float timeSlice)
	{
		InputManager input = GetGame().GetInputManager();
		if (!input)
			return;

		input.ActivateContext("TrackedContext");
		m_fDriverThrottle = input.GetActionValue("TrackedThrottle");
		m_fDriverBrake = input.GetActionValue("TrackedBrake");
		m_fDriverSteer = input.GetActionValue("TrackedSteeringRight") - input.GetActionValue("TrackedSteeringLeft");
		m_bDriverHandbrake = input.GetActionValue("TrackedHandBrake") > 0.5;

		if (m_bTestDriverSchedule)
		{
			bool pivot;
			TestSchedule(m_fDriveTime, m_fDriverThrottle, m_fDriverSteer, m_fDriverBrake, pivot);
			m_bDriverHandbrake = pivot;
		}

		// engine keys on every machine: the native controller no longer has a pilot while we drive (seat detached),
		// so it starts nothing itself, on the host either. Pressing the throttle or the brake also starts the engine,
		// as vanilla vehicles do
		bool start = input.GetActionTriggered("TrackedEngineStart") || input.GetActionTriggered("VehicleEngineStart");
		bool stop = input.GetActionTriggered("TrackedEngineStop") || input.GetActionTriggered("VehicleEngineStop");
		if (start || stop)
			RequestEngine(start);

		m_fAutoStartTimer -= timeSlice;
		if (!m_Controller.IsEngineOn() && !stop && (m_fDriverThrottle > 0.1 || m_fDriverBrake > 0.1) && m_fAutoStartTimer <= 0)
		{
			m_fAutoStartTimer = 2;
			RequestEngine(true);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Client timer (20 Hz): sends the driver's inputs (read every frame by ProxyTick) through the player controller
	protected void ClientTick()
	{
		IEntity owner = GetOwner();
		if (!owner || owner.IsDeleted() || !m_bLocalDriver)
			return;

		m_fSendTimer += 0.05;
		bool changed = Math.AbsFloat(m_fDriverThrottle - m_fSentThrottle) > 0.02 || Math.AbsFloat(m_fDriverBrake - m_fSentBrake) > 0.02 || Math.AbsFloat(m_fDriverSteer - m_fSentSteer) > 0.02 || m_bDriverHandbrake != m_bSentHandbrake;
		if (!changed && m_fSendTimer < m_fNetSendInterval)
			return;

		m_fSendTimer = 0;
		m_fSentThrottle = m_fDriverThrottle;
		m_fSentBrake = m_fDriverBrake;
		m_fSentSteer = m_fDriverSteer;
		m_bSentHandbrake = m_bDriverHandbrake;
		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (pc && m_Rpl)
		{
			pc.LEO_SendTrackedInputs(m_Rpl.Id(), m_fDriverThrottle, m_fDriverBrake, m_fDriverSteer, m_bDriverHandbrake);
			m_iSentPackets++;
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Server: inputs of the remote driver, delivered by the modded player controller
	void ReceiveNetInputs(float throttle, float brake, float steer, bool handbrake)
	{
		m_fNetThrottle = Math.Clamp(throttle, 0, 1);
		m_fNetBrake = Math.Clamp(brake, 0, 1);
		m_fNetSteer = Math.Clamp(steer, -1, 1);
		m_bNetHandbrake = handbrake;
		m_fNetAge = 0;
	}

	//------------------------------------------------------------------------------------------------
	//! Server: engine start/stop (from the player controller RPC or the test schedule)
	void SetEngine(bool on)
	{
		m_bEngineRequest = true;
		m_bEngineWanted = on;
	}

	//------------------------------------------------------------------------------------------------
	protected void RequestEngine(bool on)
	{
		if (IsAuthority())
		{
			SetEngine(on);
			return;
		}

		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (pc && m_Rpl)
			pc.LEO_SendTrackedEngine(m_Rpl.Id(), on);
	}

	//------------------------------------------------------------------------------------------------
	//! Driver inputs -> commands: brake input while standing or rolling back is reverse; handbrake + steer pivots
	protected void CommandsFromInputs(float throttleIn, float brakeIn, float steerIn, bool handbrake, float forwardSpeed)
	{
		float throttle = throttleIn;
		float brake = brakeIn;
		float reverse = 0;
		if (brake > 0.1 && throttle < 0.1 && forwardSpeed < 0.5)
		{
			reverse = brake;
			brake = 0;
		}

		m_fThrottle = throttle;
		m_fBrake = brake;
		m_fSteer = steerIn;
		m_bHandbrake = handbrake;
		m_fReverse = reverse;
	}

	//------------------------------------------------------------------------------------------------
	protected void TestSchedule(float t, out float throttle, out float steer, out float brake, out bool pivot)
	{
		float windowEnd = m_fTestDelay + m_fTestDuration;
		bool on = t >= m_fTestDelay && t < windowEnd;
		bool after = (m_fTestBrakeAfter > 0 || m_fTestSteerAfter != 0) && t >= windowEnd && t < windowEnd + m_fTestBrakeDuration;
		throttle = 0;
		steer = 0;
		brake = 0;
		pivot = false;
		if (on)
		{
			throttle = m_fTestThrottle;
			steer = m_fTestSteer;
			brake = m_fTestBrake;
			pivot = m_bTestPivot;
		}
		else if (after)
		{
			steer = m_fTestSteerAfter;
			brake = m_fTestBrakeAfter;
		}
	}

	//------------------------------------------------------------------------------------------------
	override void EOnSimulate(IEntity owner, float timeSlice)
	{
		if (!m_Controller || !IsAuthority())
			return;

		VehicleTrackedSimulation simulation = m_Controller.GetSimulation();
		if (!simulation || !simulation.IsValid())
			return;

		Physics physics = owner.GetPhysics();
		if (!physics)
			return;

		vector forward = owner.GetTransformAxis(2);
		float speed = vector.Dot(physics.GetVelocity(), forward);   // m/s, negative = backwards

		IEntity driver = GetDriver();
		bool drive = true;
		if (m_bLocalDriver)
		{
			CommandsFromInputs(m_fDriverThrottle, m_fDriverBrake, m_fDriverSteer, m_bDriverHandbrake, speed);
		}
		else if (driver)
		{
			m_fNetAge += timeSlice;
			if (m_fNetAge > 1)
			{
				m_fNetThrottle = 0;
				m_fNetSteer = 0;
				m_fNetBrake = 0;
			}
			CommandsFromInputs(m_fNetThrottle, m_fNetBrake, m_fNetSteer, m_bNetHandbrake, speed);
		}
		else if (!m_bTestDriverSchedule && (m_fTestThrottle > 0 || m_fTestSteer != 0 || m_fTestBrake > 0 || m_fTestSteerAfter != 0 || m_fTestBrakeAfter > 0))
		{
			// headless experiment without a driver: the schedule drives the server's simulation
			float throttle, steer, brake;
			bool pivot;
			TestSchedule(m_fTime, throttle, steer, brake, pivot);
			if (m_fTime >= m_fTestDelay && m_fTime - timeSlice < m_fTestDelay)
				SetEngine(true);
			CommandsFromInputs(throttle, brake, steer, pivot, speed);
		}
		else
		{
			drive = false;   // nobody drives: leave the native controller alone (handbrake etc.)
		}

		if (m_bEngineRequest)
		{
			m_bEngineRequest = false;
			if (m_bEngineWanted && !m_Controller.IsEngineOn())
				m_Controller.ForceStartEngine();
			else if (!m_bEngineWanted && m_Controller.IsEngineOn())
				m_Controller.StopEngine(false);
		}

		if (drive)
		{
			ApplyCommands(owner, simulation, physics, speed);
		}

		ReplicateState(owner, simulation, timeSlice);
		if (m_bDebug)
			Debug(owner, simulation, physics, timeSlice, speed, drive);
	}

	//------------------------------------------------------------------------------------------------
	//! Set the native inputs again after the step: the native controller consumes its own inputs in the
	//! post-simulate phase, so ours must be the last word whichever order the two run in
	override void EOnPostSimulate(IEntity owner, float timeSlice)
	{
		if (!m_Controller || !IsAuthority())
			return;

		VehicleTrackedSimulation simulation = m_Controller.GetSimulation();
		if (!simulation || !simulation.IsValid())
			return;

		if (m_bLocalDriver || GetDriver() || m_fTestThrottle > 0)
			SetNativeInputs(simulation);
	}

	//------------------------------------------------------------------------------------------------
	protected void SetNativeInputs(VehicleTrackedSimulation simulation)
	{
		float throttle = m_fThrottle;
		if (!m_Controller.IsEngineOn())
			throttle = 0;

		bool pivot = m_bHandbrake && Math.AbsFloat(m_fSteer) > 0.1;
		simulation.SetInputs(m_fSteer, throttle);

		// the clutch is the native controller's: with a pilot seated it keeps it open, so the engine revs and the
		// tracks never turn. Engaged whenever the driver wants to move or the hull moves, open at a standstill
		float speed = vector.Dot(GetOwner().GetPhysics().GetVelocity(), GetOwner().GetTransformAxis(2));
		bool engaged = m_Controller.IsEngineOn() && (throttle > 0.05 || pivot || (Math.AbsFloat(speed) > 2 && simulation.GetGear() > EVehicleGearboxGear.NEUTRAL));
		if (engaged)
			simulation.SetClutch(1);
		else
			simulation.SetClutch(0);
		if (pivot)
			simulation.SetBreak(1, true);                    // only SetBreak(x, true) stores the handbrake (6.0)
		else if (m_bHandbrake)
			simulation.SetBreak(1, false);
		else
			simulation.SetBreak(m_fBrake, false);
	}

	//------------------------------------------------------------------------------------------------
	//! Server: native inputs, gear for reverse, the reverse push
	protected void ApplyCommands(IEntity owner, VehicleTrackedSimulation simulation, Physics physics, float speed)
	{
		SetNativeInputs(simulation);

		bool reversing = m_fReverse > 0 && m_Controller.IsEngineOn();
		if (reversing)
		{
			// in a forward gear the native wheel step treats backward-spinning wheels as a direction mismatch (zeroes
			// their speed, 6x brake): select R first, push once the gearbox is in R or N and no brake is applied
			if (simulation.GetGear() > EVehicleGearboxGear.NEUTRAL)
			{
				simulation.SetGear(EVehicleGearboxGear.REVERSE);
				return;
			}

			if (simulation.GetBrake() > 0.05)
				return;

			vector forward = owner.GetTransformAxis(2);
			float maxReverse = m_fMaxReverseSpeedKmh / 3.6;
			float fade = Math.Clamp(1 - (-speed) / maxReverse, 0, 1);
			physics.ApplyForce(-forward * (physics.GetMass() * 9.81 * m_fLaunchForceG * m_fReverse * fade));

			if (Math.AbsFloat(m_fSteer) > 0.05)
			{
				vector up = owner.GetTransformAxis(1);
				float yawRate = vector.Dot(physics.GetAngularVelocity(), up);
				float torque = Math.Clamp((m_fSteer * m_fMaxYawRate - yawRate) / m_fMaxYawRate, -1, 1) * m_fSteerTorque;
				physics.ApplyTorque(up * torque);
			}
		}
		else
		{
			ShiftGears(simulation, speed);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Server: the native automatic gearbox only works for an owner-driven vehicle (it lives in the controller's
	//! move handling), so the forward gears are selected here from the engine rpm. Gear numbering: 0 = R, 1 = N,
	//! 2.. = forward
	protected void ShiftGears(VehicleTrackedSimulation simulation, float speed)
	{
		int gear = simulation.GetGear();
		int first = EVehicleGearboxGear.NEUTRAL + 1;
		int top = EVehicleGearboxGear.NEUTRAL + simulation.GearboxForwardGearsCount();
		if (gear < first)
		{
			// out of R / N once the driver wants to go forward
			if (m_fThrottle > 0.05 && speed > -0.5 && m_Controller.IsEngineOn())
				simulation.SetGear(first);
			return;
		}

		m_fShiftTimer -= 1.0 / 60;
		if (m_fShiftTimer > 0)
			return;

		float rpm = simulation.EngineGetRPM();
		if (rpm > m_fRpmShiftUp && gear < top)
		{
			simulation.SetGear(gear + 1);
			m_fShiftTimer = 1;
		}
		else if (rpm < m_fRpmShiftDown && gear > first)
		{
			simulation.SetGear(gear - 1);
			m_fShiftTimer = 1;
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Server: belt speeds and wheel contact heights for the clients' running gear, on change
	protected void ReplicateState(IEntity owner, VehicleTrackedSimulation simulation, float timeSlice)
	{
		m_fStateTimer += timeSlice;
		if (m_fStateTimer < m_fNetStateInterval || simulation.TrackCount() < 2)
			return;

		m_fStateTimer = 0;
		vector belts = Vector(simulation.WheelGetRPM(0, 0) * Math.PI2 / 60 * simulation.WheelGetRadius(0, 0), simulation.WheelGetRPM(1, 0) * Math.PI2 / 60 * simulation.WheelGetRadius(1, 0), simulation.WheelGetRadius(0, 0));
		vector heights[5];
		int mask = 0;
		for (int track = 0; track < 2; track++)
		{
			for (int k = 0; k < m_iRoadWheels; k++)
			{
				int i = track * m_iRoadWheels + k;
				float h = -10;
				if (simulation.WheelHasContact(track, k))
				{
					mask |= 1 << i;
					h = owner.CoordToLocal(simulation.WheelGetContactPosition(track, k))[1];
				}
				heights[i / 3][i % 3] = h;
			}
		}
		heights[4][2] = mask;

		bool changed = vector.DistanceSq(belts, m_vRplBelts) > 0.0004 || vector.DistanceSq(heights[0], m_vRplHeightsA) > 0.0001 || vector.DistanceSq(heights[1], m_vRplHeightsB) > 0.0001 || vector.DistanceSq(heights[2], m_vRplHeightsC) > 0.0001 || vector.DistanceSq(heights[3], m_vRplHeightsD) > 0.0001 || vector.DistanceSq(heights[4], m_vRplHeightsE) > 0.0001;
		if (!changed)
			return;

		m_vRplBelts = belts;
		m_vRplHeightsA = heights[0];
		m_vRplHeightsB = heights[1];
		m_vRplHeightsC = heights[2];
		m_vRplHeightsD = heights[3];
		m_vRplHeightsE = heights[4];
		Replication.BumpMe();
	}

	//------------------------------------------------------------------------------------------------
	//! Client, every frame: running gear visuals from the replicated state (the hull itself is moved by the native
	//! movement replication, as for any server-driven vehicle)
	protected void ProxyTick()
	{
		IEntity owner = GetOwner();
		if (!owner || owner.IsDeleted())
			return;

		float now = owner.GetWorld().GetWorldTime() * 0.001;
		float dt = Math.Clamp(now - m_fProxyLastTime, 0, 0.1);
		m_fProxyLastTime = now;
		m_iProxyTicks++;

		// the local driver's keys, every frame (input context and engine start/stop edges are per frame)
		IEntity driver = GetDriver();
		bool wasLocal = m_bLocalDriver;
		m_bLocalDriver = driver && driver == SCR_PlayerController.GetLocalControlledEntity();
		if (m_bLocalDriver)
		{
			if (!wasLocal)
				m_fDriveTime = 0;
			m_fDriveTime += dt;
			ReadDriverInputs(dt);
		}

		if (m_Gear)
			m_Gear.UpdateVisuals(owner, dt);
	}

	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		ScriptCallQueue queue = GetGame().GetCallqueue();
		if (queue)
		{
			queue.Remove(ClientTick);
			queue.Remove(ProxyTick);
			queue.Remove(ProxyHeartbeat);
		}

		if (m_Controller && m_bDetachPilotSlot && m_PilotSlot && !m_Controller.GetPilotCompartmentSlot())
			m_Controller.SetPilotCompartmentSlot(PilotCompartmentSlot.Cast(m_PilotSlot));

		super.OnDelete(owner);
	}

	//------------------------------------------------------------------------------------------------
	protected void ProxyHeartbeat()
	{
		IEntity owner = GetOwner();
		if (!owner || owner.IsDeleted())
			return;

		vector pos = owner.GetOrigin();
		float kmh = vector.Distance(pos, m_vProxyLastPos) * 3.6;
		m_vProxyLastPos = pos;
		string engine = "n/a";
		if (m_Controller)
			engine = m_Controller.IsEngineOn().ToString();
		Print("LEONET proxy heartbeat ticks/s=" + m_iProxyTicks.ToString() + " pos=" + pos.ToString() + " kmh=" + kmh.ToString() + " owner=" + m_Rpl.IsOwner().ToString() + " localDriver=" + m_bLocalDriver.ToString() + " driveTime=" + m_fDriveTime.ToString() + " packets=" + m_iSentPackets.ToString() + " engine=" + engine + " rplBeltKmh L/R=" + (m_vRplBelts[0] * 3.6).ToString() + "/" + (m_vRplBelts[1] * 3.6).ToString() + " contacts=" + m_vRplHeightsE[2].ToString());
		m_iProxyTicks = 0;
	}

	//------------------------------------------------------------------------------------------------
	protected void Debug(IEntity owner, VehicleTrackedSimulation simulation, Physics physics, float timeSlice, float speed, bool drive)
	{
		m_fDebugTimer += timeSlice;
		if (m_fDebugTimer < 1)
			return;

		m_fDebugTimer = 0;
		string source = "none";
		if (m_bLocalDriver)
			source = "local";
		else if (GetDriver())
			source = "net(age " + m_fNetAge.ToString() + ")";
		else if (drive)
			source = "test";
		string rpm = "?";
		if (simulation.TrackCount() >= 2 && simulation.WheelCount() > 0)
			rpm = simulation.WheelGetRPM(0, 0).ToString(6, 1) + "/" + simulation.WheelGetRPM(1, 0).ToString(6, 1);
		Print("LEONET t=" + m_fTime.ToString() + " pos=" + owner.GetOrigin().ToString() + " kmh=" + (speed * 3.6).ToString() + " yawRate=" + physics.GetAngularVelocity()[1].ToString() + " in thr=" + m_fThrottle.ToString() + " brk=" + m_fBrake.ToString() + " steer=" + m_fSteer.ToString() + " hb=" + m_bHandbrake.ToString() + " rev=" + m_fReverse.ToString() + " driver=" + source + " engine=" + m_Controller.IsEngineOn().ToString() + " | gear=" + simulation.GetGear().ToString() + " simBrake=" + simulation.GetBrake().ToString() + " rpm=" + simulation.EngineGetRPM().ToString() + " wheelRpm L/R=" + rpm + " clutch=" + simulation.GetClutch().ToString() + " locked=" + m_Controller.ArePilotControlsLocked().ToString() + " owner=" + m_Rpl.IsOwner().ToString());
	}
}
