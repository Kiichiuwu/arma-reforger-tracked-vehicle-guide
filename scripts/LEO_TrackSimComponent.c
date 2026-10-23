//! Scripted tracked-vehicle simulation (1.9 line). Replaces VehicleTrackedSimulation entirely: the hull is a plain
//! dynamic rigid body and this component applies every force to it from EOnSimulate (fixed physics step, server only).
//!
//! The vehicle is modelled the way a real one works:
//!   - road wheel suspension: one raycast per wheel from the modelled wheel position, spring + damper along hull up,
//!     applied at the wheel position. The wheels that touch the ground carry the hull and are the contact area;
//!   - powertrain: engine (torque curve, rpm follows the belts through the gearbox, slips at idle), automatic gearbox,
//!     final drive, sprocket. The engine's torque turns both sprockets, so each BELT has its own speed state;
//!   - belt-ground coupling: at every road wheel in contact the belt slips against the ground by (belt speed - ground
//!     speed there); the shear force grows with the slip and saturates at friction * that wheel's load. The sum of
//!     these forces moves the hull, their reaction loads the belt (and through it the engine). More wheels on the
//!     ground = more tractive force; a belt that spins faster than the ground slips and pulls less;
//!   - steering (clutch-brake): forward alone drives both belts equally; forward + side shifts the engine drive to
//!     the outer belt and brakes the inner one a little; side alone while moving brakes the inner belt hard and lets
//!     the outer one roll (the hull swings as it slows); near standstill the steering drive counter-rotates the
//!     belts (pivot). The sideways slip of the front and rear wheels is the turning resistance;
//!   - brakes act on the belts, lateral grip per wheel against sideways sliding.
//!
//! Multiplayer: the server owns the simulation. A driver on a client reads the TrackedContext inputs and sends them
//! to the server (unreliable at ~20 Hz plus a reliable engine start/stop); the server replicates belt speeds, rpm,
//! gear and the engine state (RplProp) for the running gear visuals and the sound on every machine, and proxies run
//! the suspension raycasts in visual mode (no forces) so the wheels follow the ground under the replicated hull.
//! v2.6 (2026-10-20). Inputs: the local driver (TrackedContext), a remote driver (RPC) or test attributes.
[ComponentEditorProps(category: "Leopard2A7", description: "Scripted tracked simulation: suspension, engine, gearbox, belts, traction and steering forces on a plain rigid body; server authoritative")]
class LEO_TrackSimComponentClass : ScriptComponentClass
{
}

class LEO_TrackSimComponent : ScriptComponent
{
	// ---- running gear ----
	[Attribute("7", desc: "Road wheels per track, bones v_suspension_<l|r>0<k> give the modelled wheel centres")]
	protected int m_iRoadWheels;

	[Attribute("0.418", desc: "Road wheel radius [m]")]
	protected float m_fWheelRadius;

	[Attribute("450000", desc: "Spring rate per road wheel [N/m]")]
	protected float m_fSpringRate;

	[Attribute("25000", desc: "Compression damping per road wheel [N.s/m]")]
	protected float m_fCompressionDamper;

	[Attribute("35000", desc: "Relaxation damping per road wheel [N.s/m]")]
	protected float m_fRelaxationDamper;

	[Attribute("0.30", desc: "Suspension travel up from the modelled wheel position [m]")]
	protected float m_fTravelUp;

	[Attribute("0.10", desc: "Suspension travel down from the modelled wheel position [m]")]
	protected float m_fTravelDown;

	[Attribute("0.10", desc: "Spring preload as travel [m]: the force at the modelled position equals SpringRate * this, so the hull rests near the modelled height")]
	protected float m_fPreload;

	// ---- engine ----
	[Attribute("4700", desc: "Engine peak torque [N.m]")]
	protected float m_fMaxTorque;

	[Attribute("1600", desc: "Engine rpm at peak torque")]
	protected float m_fRpmMaxTorque;

	[Attribute("1100", desc: "Engine peak power [kW]")]
	protected float m_fMaxPowerKw;

	[Attribute("2600", desc: "Engine rpm at peak power")]
	protected float m_fRpmMaxPower;

	[Attribute("700", desc: "Idle rpm: below it the clutch slips and the engine stays at idle")]
	protected float m_fRpmIdle;

	[Attribute("3000", desc: "Maximum engine rpm: the torque is zero there")]
	protected float m_fRpmMax;

	[Attribute("3", desc: "Engine + transmission moment of inertia [kg.m2], referred to the belts through the gear ratio")]
	protected float m_fEngineInertia;

	[Attribute("470", desc: "Engine friction torque [N.m]: engine braking when the throttle is released")]
	protected float m_fEngineFriction;

	// ---- transmission ----
	[Attribute("6.0 3.3 1.8 1.0", desc: "Forward gear ratios, space separated")]
	protected string m_sForwardGears;

	[Attribute("2.3", desc: "Reverse gear ratio")]
	protected float m_fReverseGear;

	[Attribute("4.6", desc: "Final drive ratio")]
	protected float m_fFinalDrive;

	[Attribute("0.9", desc: "Transmission efficiency")]
	protected float m_fEfficiency;

	[Attribute("0.34", desc: "Sprocket radius at the chain [m]: belt speed = sprocket angular speed * this")]
	protected float m_fSprocketRadius;

	[Attribute("2500", desc: "Automatic gearbox: shift up above this rpm")]
	protected float m_fRpmShiftUp;

	[Attribute("1100", desc: "Automatic gearbox: shift down below this rpm (far enough below the up-shift point divided by the ratio step not to hunt)")]
	protected float m_fRpmShiftDown;

	[Attribute("400", desc: "Mass of one belt with its sprocket and wheels, as inertia of the belt itself [kg]")]
	protected float m_fBeltMass;

	[Attribute("900000", desc: "Maximum brake force of both tracks together [N], acting on the belts")]
	protected float m_fMaxBrakeForce;

	// ---- belt - ground ----
	[Attribute("800000", desc: "Shear stiffness of a track on the ground [N per m/s of slip] before the friction limit")]
	protected float m_fSlipStiffness;

	[Attribute("0.9", desc: "Longitudinal friction coefficient of a track on the ground")]
	protected float m_fFrictionLng;

	[Attribute("0.5", desc: "Lateral friction coefficient of a track on the ground (lower than longitudinal: a track slides sideways more easily than it slips along). Also the turning resistance: higher = slower pivots and wider turns")]
	protected float m_fFrictionLat;

	[Attribute("3", desc: "Lateral grip stiffness: sideways force = this * normal load * lateral slip speed [1/(m/s)] before the friction limit")]
	protected float m_fLateralStiffness;

	[Attribute("3", desc: "Turning resistance: the lateral friction falls with speed, to 45% of its standing value beyond a few times this speed [m/s] (a moving track slides sideways more easily)")]
	protected float m_fLateralFrictionHalfSpeed;

	[Attribute("0.02", desc: "Rolling resistance as a fraction of the normal load")]
	protected float m_fRollingResistance;

	// ---- steering ----
	[Attribute("300000", desc: "Steering drive at standstill: maximum force it uses to counter-rotate the belts [N] (limits how fast a pivot starts)")]
	protected float m_fSteerForce;

	[Attribute("2.6", desc: "Steering drive at standstill: belt speed difference at full steer [m/s]. Sets the pivot rate (difference / track gauge)")]
	protected float m_fSteerSpeedDiffMax;

	[Attribute("18", desc: "Speed at which the pivot speed difference has halved [km/h]")]
	protected float m_fSteerHalfSpeedKmh;

	[Attribute("1.5", desc: "Below this speed [m/s] the steering drive pivots the belts (neutral steer); above it the clutch-brake logic takes over")]
	protected float m_fPivotBlendSpeed;

	[Attribute("0.8", desc: "Clutch-brake steering while driving: fraction of the engine drive moved from the inner belt to the outer one at full steer (0.8 = 90/10 split)")]
	protected float m_fSteerDriveShare;

	[Attribute("300000", desc: "Clutch-brake steering without throttle while moving: brake force on the inner belt at full steer [N]; the outer belt rolls free and the hull swings as it slows")]
	protected float m_fSteerBrakeForce;

	[Attribute("150000", desc: "Clutch-brake steering with the throttle pressed: brake force on the inner belt at full steer [N]. Smaller than without throttle: the drive shift to the outer belt does part of the turning")]
	protected float m_fSteerBrakeWithThrottle;

	[Attribute("54", desc: "Speed at which the inner-belt steering brake has halved [km/h], so a full steer at speed does not spin the hull")]
	protected float m_fSteerBrakeHalfSpeedKmh;

	[Attribute("31", desc: "Reverse: the automatic gearbox selects reverse at brake input while standing [km/h is informative only, the reverse gear sets the speed]")]
	protected float m_fMaxReverseSpeedKmh;

	// ---- network ----
	[Attribute("0.05", desc: "Driver on a client: seconds between input packets to the server (they are also sent on every change)")]
	protected float m_fNetSendInterval;

	[Attribute("0.05", desc: "Server: seconds between replication updates of the hull transform, belt speeds, rpm and gear for the clients")]
	protected float m_fNetStateInterval;

	[Attribute("0.08", desc: "Clients: time constant of the smoothing towards the replicated (dead-reckoned) hull transform [s]")]
	protected float m_fProxySmoothing;

	// ---- test inputs (headless experiments, used when no driver sits in the vehicle) ----
	[Attribute("0", desc: "Test input: throttle 0..1 applied after the test delay")]
	protected float m_fTestThrottle;

	[Attribute("0", desc: "Test input: steering -1..1 applied after the test delay")]
	protected float m_fTestSteer;

	[Attribute("0", desc: "Test input: brake 0..1 applied after the test delay")]
	protected float m_fTestBrake;

	[Attribute("0", desc: "Test input: pivot steering while the test runs (informative)")]
	protected bool m_bTestPivot;

	[Attribute("5", desc: "Seconds after the first step before the test inputs apply")]
	protected float m_fTestDelay;

	[Attribute("10", desc: "Seconds the test inputs stay applied")]
	protected float m_fTestDuration;

	[Attribute("0", desc: "Test input: brake 0..1 applied right after the test window for the after-window duration")]
	protected float m_fTestBrakeAfter;

	[Attribute("0", desc: "Test input: steering -1..1 applied right after the test window (throttle released) for the after-window duration")]
	protected float m_fTestSteerAfter;

	[Attribute("5", desc: "Seconds the after-window phase lasts")]
	protected float m_fTestBrakeDuration;

	[Attribute("0", desc: "Test input: the test schedule replaces the driver's keys once a driver sits in (automated multiplayer test: the driver's machine sends it to the server exactly like real inputs). Without a driver the schedule is then NOT applied")]
	protected bool m_bTestDriverSchedule;

	[Attribute("0", desc: "Print a LEOSIM line once per second")]
	protected bool m_bDebug;

	// current commands of the simulation (0..1, -1..1), server
	protected float m_fThrottle;
	protected float m_fReverse;
	protected float m_fSteer;
	protected float m_fBrake;
	protected bool m_bPivot;

	// driver's machine: raw inputs read every frame
	protected bool m_bLocalDriver;
	protected float m_fDriverThrottle;
	protected float m_fDriverBrake;
	protected float m_fDriverSteer;
	protected bool m_bDriverHandbrake;

	// driver's machine when it is a client: what was last sent
	protected float m_fSentThrottle;
	protected float m_fSentBrake;
	protected float m_fSentSteer;
	protected bool m_bSentHandbrake;
	protected float m_fSendTimer;
	protected float m_fDriveTime;       // seconds since the local driver sat in (test schedule time base)
	protected float m_fAutoStartTimer;  // driver's machine: seconds before the next automatic start request
	protected vector m_vProxyLastPos;
	protected float m_fProxyTimer;

	// server: last inputs received from a client driver
	protected float m_fNetThrottle;
	protected float m_fNetBrake;
	protected float m_fNetSteer;
	protected bool m_bNetHandbrake;
	protected float m_fNetAge;          // seconds since the last packet
	protected bool m_bNetFresh;

	// replicated state (server writes, everyone reads): engine and what the visuals and sound need
	[RplProp()]
	protected bool m_bEngineOn;
	[RplProp()]
	protected float m_fRplBeltL;
	[RplProp()]
	protected float m_fRplBeltR;
	[RplProp()]
	protected float m_fRplRpm;
	[RplProp()]
	protected int m_iRplGear;
	// hull transform: the inherited NwkTrackedMovementComponent replicates nothing without a working native
	// simulation, so the server sends the transform itself and clients dead-reckon between updates
	[RplProp()]
	protected vector m_vRplPos;
	[RplProp()]
	protected vector m_vRplAngles;      // yaw pitch roll [deg]
	[RplProp()]
	protected vector m_vRplVel;
	protected float m_fStateTimer;

	// client: dead reckoning state
	protected vector m_vBasePos;
	protected vector m_vBaseVel;
	protected vector m_vBaseAngles;
	protected float m_fBaseTime;
	protected vector m_vProxyPos;
	protected vector m_vProxyAngles;
	protected bool m_bProxyInit;
	protected float m_fProxyLastTime;
	protected int m_iProxyTicks;
	protected LEO_RunningGearComponent m_Gear;

	// per road wheel, index track * m_iRoadWheels + k
	protected ref array<vector> m_aWheelRest = {};   // modelled wheel centre, hull space
	protected ref array<float> m_aTravel = {};       // + = compressed (wheel pushed up), - = drooped
	protected ref array<bool> m_aContact = {};
	protected ref array<float> m_aLoad = {};         // spring force [N]
	protected ref array<float> m_aGroundSpeed = {};  // ground speed along the hull at the contact [m/s], this step

	// powertrain state (server)
	protected ref array<float> m_aGears = {};
	protected ref array<float> m_aBeltSpeed = {};    // per track: belt speed over the ground face, + = forward [m/s]
	protected ref array<float> m_aBeltMass = {};     // per track: equivalent mass of the belt this step [kg]
	protected float m_fRpm;
	protected int m_iGear;                           // 1..n forward, -1 reverse, 0 neutral
	protected float m_fCenterSpeed;                  // mean ground speed of the tracks along the hull [m/s]
	protected float m_fShiftTimer;

	protected TrackedControllerComponent m_Controller;   // the inherited controller: seat and compartment only
	protected RplComponent m_Rpl;
	protected NwkTrackedMovementComponent m_Nwk;         // inherited movement replication: transform only, no prediction
	protected int m_iOwnershipTakebacks;
	protected float m_fOwnershipTakebackTime;
	protected int m_iFrameCalls;                          // proxy heartbeat: EOnFrame calls since the last beat
	protected int m_iSentPackets;
	protected float m_fTime;
	protected float m_fDebugTimer;

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		// no IsConsoleApp() early return: a dedicated server is a console app and it is exactly where this runs.
		// Measured on 1.9: a replicated vehicle on a client gets NO entity events at all (frame, post frame, fixed
		// frame, visible), whatever its event mask or ACTIVE flag; the client side therefore runs from the call queue

		m_Rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
		m_Controller = TrackedControllerComponent.Cast(owner.FindComponent(TrackedControllerComponent));
		m_Nwk = NwkTrackedMovementComponent.Cast(owner.FindComponent(NwkTrackedMovementComponent));
		if (m_Nwk)
		{
			// the inherited native tracked simulation is inert (no wheel config) and must never be stepped on a
			// client: no owner-side prediction, and proxies only interpolate the replicated transform
			m_Nwk.SetPrediction(false);
			if (m_Rpl && m_Rpl.IsProxy())
				m_Nwk.EnableSimulation(false);
		}
		m_aBeltSpeed.Insert(0);
		m_aBeltSpeed.Insert(0);
		m_aBeltMass.Insert(m_fBeltMass);
		m_aBeltMass.Insert(m_fBeltMass);
		m_fRpm = m_fRpmIdle;
		m_iGear = 1;

		array<string> gears = {};
		m_sForwardGears.Split(" ", gears, true);
		foreach (string gear : gears)
		{
			float ratio = gear.ToFloat();
			if (ratio > 0)
				m_aGears.Insert(ratio);
		}
		if (m_aGears.IsEmpty())
			m_aGears.Insert(1);

		Animation anim = owner.GetAnimation();
		int missing;
		for (int track = 0; track < 2; track++)
		{
			string side = "l";
			if (track == 1)
				side = "r";

			for (int k = 1; k <= m_iRoadWheels; k++)
			{
				vector rest = Vector(-1.329, m_fWheelRadius, 0);
				if (track == 1)
					rest[0] = 1.329;

				TNodeId bone = -1;
				if (anim)
					bone = anim.GetBoneIndex("v_suspension_" + side + "0" + k.ToString());

				if (bone != -1)
				{
					vector mat[4];
					anim.GetBoneMatrix(bone, mat);
					rest = mat[3];
				}
				else
				{
					missing++;
				}

				m_aWheelRest.Insert(rest);
				m_aTravel.Insert(0);
				m_aContact.Insert(false);
				m_aLoad.Insert(0);
				m_aGroundSpeed.Insert(0);
			}
		}

		if (missing > 0)
			Print("LEOSIM " + missing.ToString() + " suspension bones missing: wheel positions default to the Leopard layout", LogLevel.WARNING);

		SetEventMask(owner, EntityEvent.SIMULATE | EntityEvent.FRAME);

		if (m_Rpl && m_Rpl.IsProxy())
		{
			// clients: a replicated vehicle gets no entity frame events here, so everything client-side runs from
			// timers: the driver's inputs (20 Hz), the hull transform follow + visuals (every frame) and a 1 Hz
			// heartbeat (debug). The hull's rigid body is never simulated on a client: it follows the server
			Physics physics = owner.GetPhysics();
			if (physics && physics.IsDynamic())
				physics.ChangeSimulationState(SimulationState.COLLISION);   // collides, never simulated here

			m_Gear = LEO_RunningGearComponent.Cast(owner.FindComponent(LEO_RunningGearComponent));
			if (m_Gear)
				m_Gear.SetExternallyDriven(true);

			GetGame().GetCallqueue().CallLater(ClientTick, 50, true);
			GetGame().GetCallqueue().CallLater(ProxyTick, 0, true);
			if (m_bDebug)
				GetGame().GetCallqueue().CallLater(ProxyHeartbeat, 1000, true);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Client, every frame: follow the replicated hull transform (dead reckoning between updates), then the visuals
	protected void ProxyTick()
	{
		IEntity owner = GetOwner();
		if (!owner || owner.IsDeleted())
			return;

		float now = owner.GetWorld().GetWorldTime() * 0.001;
		float dt = Math.Clamp(now - m_fProxyLastTime, 0, 0.1);
		m_fProxyLastTime = now;
		m_iProxyTicks++;

		// the local driver's keys, every frame: the input context is activated per frame and the engine start/stop
		// actions are per-frame edges, so a slower poll would miss them
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

		// a new state from the server: new dead-reckoning base
		if (m_vRplPos != m_vBasePos || m_vRplVel != m_vBaseVel || m_vRplAngles != m_vBaseAngles)
		{
			m_vBasePos = m_vRplPos;
			m_vBaseVel = m_vRplVel;
			m_vBaseAngles = m_vRplAngles;
			m_fBaseTime = now;
		}

		if (m_vBasePos == vector.Zero)
			return;   // nothing received yet

		float age = Math.Clamp(now - m_fBaseTime, 0, 0.5);
		vector targetPos = m_vBasePos + m_vBaseVel * age;
		if (!m_bProxyInit)
		{
			m_bProxyInit = true;
			m_vProxyPos = targetPos;
			m_vProxyAngles = m_vBaseAngles;
		}
		else
		{
			float blend = 1 - Math.Pow(2.718282, -dt / Math.Max(m_fProxySmoothing, 0.001));
			m_vProxyPos = m_vProxyPos + (targetPos - m_vProxyPos) * blend;
			for (int a = 0; a < 3; a++)
			{
				float delta = m_vBaseAngles[a] - m_vProxyAngles[a];
				while (delta > 180)
					delta -= 360;
				while (delta < -180)
					delta += 360;
				m_vProxyAngles[a] = m_vProxyAngles[a] + delta * blend;
			}
		}

		vector mat[4];
		Math3D.AnglesToMatrix(m_vProxyAngles, mat);
		mat[3] = m_vProxyPos;
		owner.SetTransform(mat);

		SuspensionVisual(owner, mat);
		if (m_Gear)
			m_Gear.UpdateVisuals(owner, dt);
	}

	//------------------------------------------------------------------------------------------------
	//! Client timer (20 Hz): sends the driver's inputs (read every frame by ProxyTick)
	protected void ClientTick()
	{
		IEntity owner = GetOwner();
		if (!owner || owner.IsDeleted() || !m_bLocalDriver)
			return;

		SendInputs(0.05);
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

		super.OnDelete(owner);
	}

	//------------------------------------------------------------------------------------------------
	protected void ProxyHeartbeat()
	{
		IEntity owner = GetOwner();
		if (!owner || owner.IsDeleted())
			return;

		int frames = m_iFrameCalls;
		m_iFrameCalls = 0;
		string events = " ticks/s=" + m_iProxyTicks.ToString();
		m_iProxyTicks = 0;
		vector camera[4];
		owner.GetWorld().GetCurrentCamera(camera);
		events += " cam=" + camera[3].ToString() + " rplPos=" + m_vRplPos.ToString() + " rplKmh=" + (m_vRplVel.Length() * 3.6).ToString();
		bool active = (owner.GetFlags() & EntityFlags.ACTIVE) != 0;
		Print("LEOSIM proxy heartbeat frames/s=" + frames.ToString() + events + " eventMask=" + owner.GetEventMask().ToString() + " active=" + active.ToString() + " owner=" + m_Rpl.IsOwner().ToString() + " localDriver=" + m_bLocalDriver.ToString() + " driveTime=" + m_fDriveTime.ToString() + " packets=" + m_iSentPackets.ToString() + " engine=" + m_bEngineOn.ToString() + " rplBelt L=" + (m_fRplBeltL * 3.6).ToString() + " pos=" + owner.GetOrigin().ToString());
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsAuthority()
	{
		return !m_Rpl || !m_Rpl.IsProxy();
	}

	// ---- read access for the running gear visuals and the sound (valid on every machine) ----
	int GetRoadWheels() { return m_iRoadWheels; }
	float GetWheelRadius() { return m_fWheelRadius; }
	bool IsEngineOn() { return m_bEngineOn; }
	float GetCenterSpeed() { return m_fCenterSpeed; }

	float GetRpm()
	{
		if (IsAuthority())
			return m_fRpm;
		return m_fRplRpm;
	}

	int GetGear()
	{
		if (IsAuthority())
			return m_iGear;
		return m_iRplGear;
	}

	bool WheelContact(int track, int k)
	{
		int i = track * m_iRoadWheels + k;
		if (!m_aContact.IsIndexValid(i))
			return false;
		return m_aContact[i];
	}

	//! + = compressed (wheel pushed up from its modelled position), - = drooped [m]
	float WheelTravel(int track, int k)
	{
		int i = track * m_iRoadWheels + k;
		if (!m_aTravel.IsIndexValid(i))
			return 0;
		return m_aTravel[i];
	}

	//! Speed of the belt's ground face relative to the hull, + = running forward [m/s]
	float GetBeltSpeed(int track)
	{
		if (!IsAuthority())
		{
			if (track == 0)
				return m_fRplBeltL;
			return m_fRplBeltR;
		}

		if (!m_aBeltSpeed.IsIndexValid(track))
			return 0;
		return m_aBeltSpeed[track];
	}

	//------------------------------------------------------------------------------------------------
	IEntity GetDriver()
	{
		if (!m_Controller)
			return null;

		BaseCompartmentSlot slot = m_Controller.GetPilotCompartmentSlot();
		if (!slot)
			return null;

		return slot.GetOccupant();
	}

	//------------------------------------------------------------------------------------------------
	//! Every frame, everywhere: the driver's machine reads and (on a client) sends the inputs; proxies run the
	//! suspension in visual mode; the authority keeps the rigid body awake (without a native vehicle simulation the
	//! hull is born asleep and would never fall)
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		m_iFrameCalls++;
		if (IsAuthority())
		{
			IEntity driver = GetDriver();
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

		if (!IsAuthority())
		{
			// proxy: wheels follow the ground under the replicated hull, no forces
			vector hull[4];
			owner.GetTransform(hull);
			SuspensionVisual(owner, hull);
			if (m_bDebug)
				DebugProxy(owner, timeSlice);
			return;
		}

		// the engine hands vehicle ownership to the driver's client when it sits in (client-side prediction of the
		// native simulation). Our simulation is server-only, so the server takes the vehicle back at once
		if (m_Rpl && !m_Rpl.IsOwner() && m_fTime - m_fOwnershipTakebackTime > 0.5)
		{
			m_fOwnershipTakebackTime = m_fTime;
			m_Rpl.GiveExt(RplIdentity.Local(), false);
			m_iOwnershipTakebacks++;
			if (m_iOwnershipTakebacks <= 5)
				Print("LEOSIM vehicle ownership taken back by the server (" + m_iOwnershipTakebacks.ToString() + ")");
		}

		Physics physics = owner.GetPhysics();
		if (!physics)
			return;

		if (!physics.IsActive())
			physics.SetActive(ActiveState.ALWAYS_ACTIVE);
	}

	//------------------------------------------------------------------------------------------------
	//! The tracked input context has to be kept active by us: the inherited controller has no simulation to drive
	protected void ReadDriverInputs(float timeSlice)
	{
		InputManager input = GetGame().GetInputManager();
		if (!input)
			return;

		input.ActivateContext("TrackedContext");

		bool start = input.GetActionTriggered("TrackedEngineStart") || input.GetActionTriggered("VehicleEngineStart");
		bool stop = input.GetActionTriggered("TrackedEngineStop") || input.GetActionTriggered("VehicleEngineStop");
		if (start || stop)
			RequestEngine(start);

		m_fDriverThrottle = input.GetActionValue("TrackedThrottle");
		m_fDriverBrake = input.GetActionValue("TrackedBrake");
		m_fDriverSteer = input.GetActionValue("TrackedSteeringRight") - input.GetActionValue("TrackedSteeringLeft");
		m_bDriverHandbrake = input.GetActionValue("TrackedHandBrake") > 0.5;

		if (m_bTestDriverSchedule)
		{
			// automated test: the schedule plays the driver, engine started half a second after sitting in
			bool pivot;
			TestSchedule(m_fDriveTime, m_fDriverThrottle, m_fDriverSteer, m_fDriverBrake, pivot);
			m_bDriverHandbrake = pivot;
		}

		// pressing the throttle or the brake starts the engine, as vanilla vehicles do (rate limited)
		m_fAutoStartTimer -= timeSlice;
		if (!m_bEngineOn && !stop && (m_fDriverThrottle > 0.1 || m_fDriverBrake > 0.1) && m_fAutoStartTimer <= 0)
		{
			m_fAutoStartTimer = 2;
			RequestEngine(true);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Client driver -> server: on every change and at least every send interval
	protected void SendInputs(float timeSlice)
	{
		m_fSendTimer += timeSlice;
		bool changed = Math.AbsFloat(m_fDriverThrottle - m_fSentThrottle) > 0.02 || Math.AbsFloat(m_fDriverBrake - m_fSentBrake) > 0.02 || Math.AbsFloat(m_fDriverSteer - m_fSentSteer) > 0.02 || m_bDriverHandbrake != m_bSentHandbrake;
		if (!changed && m_fSendTimer < m_fNetSendInterval)
			return;

		m_fSendTimer = 0;
		m_fSentThrottle = m_fDriverThrottle;
		m_fSentBrake = m_fDriverBrake;
		m_fSentSteer = m_fDriverSteer;
		m_bSentHandbrake = m_bDriverHandbrake;
		// through the player controller: the one replicated item this client owns (see LEO_PlayerController.c)
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
		m_bNetFresh = true;
	}

	//------------------------------------------------------------------------------------------------
	//! Driver's machine: engine start/stop request, local on the authority, via the player controller on a client
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
	//! Server: engine state, replicated to everyone
	void SetEngine(bool on)
	{
		if (m_bEngineOn == on)
			return;

		m_bEngineOn = on;
		Replication.BumpMe();
	}

	//------------------------------------------------------------------------------------------------
	//! Driver inputs -> simulation commands: brake while standing is reverse, handbrake + steer is a pivot
	protected void CommandsFromInputs(float throttleIn, float brakeIn, float steerIn, bool handbrake)
	{
		float throttle = throttleIn;
		float brake = brakeIn;
		float steer = steerIn;
		float reverse = 0;

		// brake input without throttle: brakes while rolling forward, reverses once (nearly) stopped and keeps
		// reversing while moving backwards
		if (brake > 0.1 && throttle < 0.1 && m_fCenterSpeed < 0.5)
		{
			reverse = brake;
			brake = 0;
		}

		if (!m_bEngineOn)
		{
			throttle = 0;
			reverse = 0;
		}

		bool pivot = handbrake && Math.AbsFloat(steer) > 0.1;
		if (handbrake && !pivot)
			brake = 1;

		m_fThrottle = throttle;
		m_fReverse = reverse;
		m_fBrake = brake;
		m_fSteer = steer;
		m_bPivot = pivot;
	}

	//------------------------------------------------------------------------------------------------
	//! Server: which inputs drive the simulation this step
	protected void SelectInputs(float timeSlice)
	{
		if (m_bLocalDriver)
		{
			CommandsFromInputs(m_fDriverThrottle, m_fDriverBrake, m_fDriverSteer, m_bDriverHandbrake);
			return;
		}

		IEntity driver = GetDriver();
		if (driver)
		{
			// a remote driver: use the last packet, fade it out if the packets stop
			m_fNetAge += timeSlice;
			if (m_fNetAge > 1)
			{
				m_fNetThrottle = 0;
				m_fNetSteer = 0;
				m_fNetBrake = 0;
			}
			CommandsFromInputs(m_fNetThrottle, m_fNetBrake, m_fNetSteer, m_bNetHandbrake);
			return;
		}

		// nobody inside: the test inputs (headless experiments) or nothing
		m_bNetFresh = false;
		if (m_bTestDriverSchedule)
		{
			m_fThrottle = 0;
			m_fReverse = 0;
			m_fSteer = 0;
			m_fBrake = 0;
			m_bPivot = false;
			return;
		}
		UpdateTestInputs();
	}

	//------------------------------------------------------------------------------------------------
	override void EOnSimulate(IEntity owner, float timeSlice)
	{
		if (!IsAuthority())
			return;

		Physics physics = owner.GetPhysics();
		if (!physics || !physics.IsDynamic())
			return;

		m_fTime += timeSlice;
		SelectInputs(timeSlice);

		vector hull[4];
		owner.GetTransform(hull);
		vector right = hull[0];
		vector up = hull[1];
		vector forward = hull[2];

		float loadL = Suspension(owner, physics, hull, 0, up);
		float loadR = Suspension(owner, physics, hull, 1, up);

		// ground speed at every contact, this step
		for (int i = 0; i < m_aWheelRest.Count(); i++)
		{
			if (m_aContact[i])
				m_aGroundSpeed[i] = vector.Dot(physics.GetVelocityAt(ContactPoint(owner, i)), forward);
			else
				m_aGroundSpeed[i] = 0;
		}
		float vL = TrackGroundSpeed(0, loadL);
		float vR = TrackGroundSpeed(1, loadR);
		m_fCenterSpeed = 0.5 * (vL + vR);

		Powertrain(timeSlice);

		// each belt against engine, brake and ground; then the steering drive sets their difference near standstill
		float newL = BeltSolve(0, loadL, EngineForce(0), timeSlice);
		float newR = BeltSolve(1, loadR, EngineForce(1), timeSlice);
		Steering(newL, newR, timeSlice);
		BeltApply(owner, physics, 0, loadL, newL, forward, right);
		BeltApply(owner, physics, 1, loadR, newR, forward, right);

		ReplicateState(timeSlice);

		if (m_bDebug)
			Debug(owner, physics, timeSlice, loadL, loadR);
	}

	//------------------------------------------------------------------------------------------------
	//! Server: belt speeds, rpm and gear for the clients' visuals and sound, a few times per second
	protected void ReplicateState(float timeSlice)
	{
		m_fStateTimer += timeSlice;
		if (m_fStateTimer < m_fNetStateInterval)
			return;

		m_fStateTimer = 0;
		IEntity owner = GetOwner();
		vector pos = owner.GetOrigin();
		vector angles = owner.GetYawPitchRoll();
		vector vel = owner.GetPhysics().GetVelocity();
		bool moved = vector.DistanceSq(pos, m_vRplPos) > 0.0001 || vector.DistanceSq(angles, m_vRplAngles) > 0.0025 || vector.DistanceSq(vel, m_vRplVel) > 0.0025;
		bool changed = Math.AbsFloat(m_aBeltSpeed[0] - m_fRplBeltL) > 0.05 || Math.AbsFloat(m_aBeltSpeed[1] - m_fRplBeltR) > 0.05 || Math.AbsFloat(m_fRpm - m_fRplRpm) > 20 || m_iGear != m_iRplGear;
		if (!moved && !changed)
			return;

		m_vRplPos = pos;
		m_vRplAngles = angles;
		m_vRplVel = vel;
		m_fRplBeltL = m_aBeltSpeed[0];
		m_fRplBeltR = m_aBeltSpeed[1];
		m_fRplRpm = m_fRpm;
		m_iRplGear = m_iGear;
		Replication.BumpMe();
	}

	//------------------------------------------------------------------------------------------------
	//! Raycast of one road wheel: fills contact, travel (and the ray hit) without any force. Returns the contact flag
	protected bool WheelRay(IEntity owner, int i, out float travel)
	{
		BaseWorld world = owner.GetWorld();
		float restLength = m_fWheelRadius + m_fTravelDown;   // ray length below the modelled wheel centre
		vector rest = m_aWheelRest[i];

		// cast from above the most compressed position down to full droop, so the hull never starts under the ray
		vector start = owner.CoordToParent(rest + Vector(0, m_fTravelUp, 0));
		vector end = owner.CoordToParent(rest - Vector(0, restLength, 0));

		autoptr TraceParam param = new TraceParam();
		param.Start = start;
		param.End = end;
		param.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
		param.Exclude = owner;
		param.LayerMask = EPhysicsLayerPresets.VehicleCast;
		float fraction = world.TraceMove(param, null);

		if (fraction >= 1)
		{
			travel = -m_fTravelDown;
			return false;
		}

		// distance from the modelled wheel centre down to the contact, positive below it
		float rayLength = m_fTravelUp + restLength;
		float hitBelowTop = fraction * rayLength;
		float contactBelowCentre = hitBelowTop - m_fTravelUp;
		// the wheel sits on the contact: compression = how far the contact is above the modelled wheel bottom
		travel = Math.Clamp(m_fWheelRadius - contactBelowCentre, -m_fTravelDown, m_fTravelUp);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Spring + damper per road wheel of one track; returns the track's total normal load [N]
	protected float Suspension(IEntity owner, Physics physics, vector hull[4], int track, vector up)
	{
		float load = 0;
		for (int k = 0; k < m_iRoadWheels; k++)
		{
			int i = track * m_iRoadWheels + k;
			float travel;
			if (!WheelRay(owner, i, travel))
			{
				m_aContact[i] = false;
				m_aTravel[i] = travel;
				m_aLoad[i] = 0;
				continue;
			}

			vector wheelPos = owner.CoordToParent(m_aWheelRest[i] + Vector(0, travel, 0));
			vector velocity = physics.GetVelocityAt(wheelPos);
			float compressionSpeed = -vector.Dot(velocity, up);   // hull moving down at the wheel = compressing

			float damper = m_fRelaxationDamper;
			if (compressionSpeed > 0)
				damper = m_fCompressionDamper;

			float force = m_fSpringRate * (travel + m_fPreload) + damper * compressionSpeed;
			if (force < 0)
				force = 0;

			physics.ApplyForceAt(wheelPos, up * force);

			m_aContact[i] = true;
			m_aTravel[i] = travel;
			m_aLoad[i] = force;
			load += force;
		}

		return load;
	}

	//------------------------------------------------------------------------------------------------
	//! Proxies: the same raycasts, only to animate the wheels under the replicated hull
	protected void SuspensionVisual(IEntity owner, vector hull[4])
	{
		for (int i = 0; i < m_aWheelRest.Count(); i++)
		{
			float travel;
			m_aContact[i] = WheelRay(owner, i, travel);
			m_aTravel[i] = travel;
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Ground contact point of a road wheel in world space
	protected vector ContactPoint(IEntity owner, int i)
	{
		return owner.CoordToParent(m_aWheelRest[i] + Vector(0, m_aTravel[i] - m_fWheelRadius, 0));
	}

	//------------------------------------------------------------------------------------------------
	//! Load-weighted mean ground speed of a track's contacts along the hull's forward axis
	protected float TrackGroundSpeed(int track, float load)
	{
		if (load <= 0)
			return 0;

		float v = 0;
		for (int k = 0; k < m_iRoadWheels; k++)
		{
			int i = track * m_iRoadWheels + k;
			if (m_aContact[i])
				v += m_aGroundSpeed[i] * m_aLoad[i];
		}

		return v / load;
	}

	//------------------------------------------------------------------------------------------------
	//! Overall ratio from engine to belt for the current gear (0 in neutral)
	protected float GearRatio()
	{
		if (m_iGear > 0 && m_aGears.IsIndexValid(m_iGear - 1))
			return m_aGears[m_iGear - 1] * m_fFinalDrive;
		if (m_iGear < 0)
			return -m_fReverseGear * m_fFinalDrive;
		return 0;
	}

	//------------------------------------------------------------------------------------------------
	//! Engine torque available at an rpm [N.m]: rises to the peak torque, holds the peak power, dies at max rpm
	protected float EngineTorque(float rpm)
	{
		if (rpm >= m_fRpmMax)
			return 0;

		float peakPower = m_fMaxPowerKw * 1000;
		if (rpm <= m_fRpmMaxTorque)
			return m_fMaxTorque * (0.6 + 0.4 * rpm / m_fRpmMaxTorque);

		float omega = rpm * Math.PI2 / 60;
		float powerTorque = peakPower / omega;
		if (rpm <= m_fRpmMaxPower)
			return Math.Min(m_fMaxTorque, Math.Max(powerTorque, m_fMaxTorque * 0.8));

		// beyond peak power the torque falls to zero at max rpm
		return powerTorque * (m_fRpmMax - rpm) / (m_fRpmMax - m_fRpmMaxPower);
	}

	//------------------------------------------------------------------------------------------------
	//! Gear selection and engine rpm. The engine is tied to the belts through the gearbox: rpm = belt speed *
	//! ratio / sprocket circumference, never below idle (the clutch slips there)
	protected void Powertrain(float timeSlice)
	{
		// forward / reverse / neutral from the inputs
		bool wantsReverse = m_fReverse > 0.01;
		bool wantsForward = m_fThrottle > 0.01;
		if (wantsReverse && m_fCenterSpeed < 0.5)
			m_iGear = -1;
		else if (wantsForward && m_iGear <= 0)
			m_iGear = 1;

		float beltSpeed = 0.5 * (m_aBeltSpeed[0] + m_aBeltSpeed[1]);
		float ratio = GearRatio();
		float rpmFromBelts = 0;
		if (ratio != 0)
			rpmFromBelts = Math.AbsFloat(beltSpeed * ratio) / (Math.PI2 * m_fSprocketRadius) * 60;

		m_fRpm = Math.Max(m_fRpmIdle, rpmFromBelts);

		// automatic gearbox, with a short lockout so it does not hunt
		m_fShiftTimer -= timeSlice;
		if (m_iGear > 0 && m_fShiftTimer <= 0)
		{
			if (m_fRpm > m_fRpmShiftUp && m_iGear < m_aGears.Count())
			{
				m_iGear++;
				m_fShiftTimer = 1;
			}
			else if (m_fRpm < m_fRpmShiftDown && m_iGear > 1)
			{
				m_iGear--;
				m_fShiftTimer = 1;
			}
		}

		// equivalent mass of each belt as seen from its ground face: the belt plus the engine and transmission
		// through the gear ratio (the faster the gear, the lighter the engine feels)
		float absRatio = Math.AbsFloat(ratio);
		float mass = m_fBeltMass + 0.5 * m_fEngineInertia * absRatio * absRatio / (m_fSprocketRadius * m_fSprocketRadius);
		m_aBeltMass[0] = mass;
		m_aBeltMass[1] = mass;
	}

	//------------------------------------------------------------------------------------------------
	//! Force the engine puts on one belt through the gearbox [N], including engine braking
	protected float EngineForce(int track)
	{
		float ratio = GearRatio();
		if (ratio == 0)
			return 0;

		float absRatio = Math.AbsFloat(ratio);
		float demand = m_fThrottle;
		if (m_iGear < 0)
			demand = m_fReverse;

		// drive torque at the sprocket, split between the two belts: equal when driving straight, shifted towards
		// the outer belt when steering (steer > 0 = right turn, the left belt is the outer one)
		float share = 0.5;
		float steer = Math.AbsFloat(m_fSteer);
		if (steer > 0.01)
		{
			bool outer = (m_fSteer > 0) == (track == 0);
			if (outer)
				share = 0.5 + 0.5 * steer * m_fSteerDriveShare;
			else
				share = 0.5 - 0.5 * steer * m_fSteerDriveShare;
		}
		float torque = demand * EngineTorque(m_fRpm) * m_fEfficiency;
		float force = share * torque * absRatio / m_fSprocketRadius;
		if (m_iGear < 0)
			force = -force;

		// engine braking when the throttle is released: friction torque against the belt's direction
		if (demand < 0.01)
		{
			float braking = 0.5 * m_fEngineFriction * absRatio / m_fSprocketRadius;
			float v = m_aBeltSpeed[track];
			force -= Math.Clamp(v * 20, -1, 1) * braking;
		}

		return force;
	}

	//------------------------------------------------------------------------------------------------
	//! New speed of one belt under the engine force, the brake drag and the ground coupling, solved implicitly.
	//! Wheels whose shear would exceed their friction limit are taken out of the implicit set and push with their
	//! limit instead (a slipping track), so the belt can never be yanked by an unbounded spring force.
	protected float BeltSolve(int track, float load, float engineForce, float timeSlice)
	{
		float v = m_aBeltSpeed[track];
		float mass = m_aBeltMass[track];

		// brakes on the belt, as a drag coefficient so a braked belt locks near zero and skids over the ground
		float brake = m_fBrake * m_fMaxBrakeForce * 0.5;

		// clutch-brake steering while moving: the inner belt is braked in proportion to the steer (the drive goes
		// to the outer belt in EngineForce); fades with speed and blends out at pivot speeds
		float steer = Math.AbsFloat(m_fSteer);
		bool inner = (m_fSteer > 0) == (track == 1);
		float speed = Math.AbsFloat(m_fCenterSpeed);
		if (steer > 0.01 && inner && speed > 0.1)
		{
			float moving = Math.Clamp(speed / m_fPivotBlendSpeed, 0, 1);
			float fade = 1 / (1 + speed / (m_fSteerBrakeHalfSpeedKmh / 3.6));
			float steerBrake = m_fSteerBrakeForce;
			if (m_fThrottle > 0.01 || m_fReverse > 0.01)
				steerBrake = m_fSteerBrakeWithThrottle;
			brake += steer * steerBrake * moving * fade;
		}

		float brakeCoeff = brake / Math.Max(Math.AbsFloat(v), 0.5);

		// pass 1: every contact linear
		float stiffnessTotal = 0;
		float stiffnessTimesGround = 0;
		for (int k = 0; k < m_iRoadWheels; k++)
		{
			int i = track * m_iRoadWheels + k;
			if (!m_aContact[i] || load <= 0)
				continue;

			float stiffness = m_fSlipStiffness * m_aLoad[i] / load;
			stiffnessTotal += stiffness;
			stiffnessTimesGround += stiffness * m_aGroundSpeed[i];
		}

		float vNew = (mass * v + timeSlice * engineForce + timeSlice * stiffnessTimesGround) / (mass + timeSlice * (stiffnessTotal + brakeCoeff));

		// pass 2: contacts that saturate push with their friction limit as a constant force on the belt
		float linearTotal = 0;
		float linearTimesGround = 0;
		float saturatedForce = 0;
		for (int k = 0; k < m_iRoadWheels; k++)
		{
			int i = track * m_iRoadWheels + k;
			if (!m_aContact[i] || load <= 0)
				continue;

			float stiffness = m_fSlipStiffness * m_aLoad[i] / load;
			float shear = stiffness * (vNew - m_aGroundSpeed[i]);
			float limit = m_fFrictionLng * m_aLoad[i];
			if (Math.AbsFloat(shear) > limit)
			{
				// the ground pushes the hull with +-limit; the reaction on the belt is the opposite
				if (shear > 0)
					saturatedForce -= limit;
				else
					saturatedForce += limit;
			}
			else
			{
				linearTotal += stiffness;
				linearTimesGround += stiffness * m_aGroundSpeed[i];
			}
		}

		vNew = (mass * v + timeSlice * (engineForce + saturatedForce) + timeSlice * linearTimesGround) / (mass + timeSlice * (linearTotal + brakeCoeff));
		return vNew;
	}

	//------------------------------------------------------------------------------------------------
	//! Near standstill the steering drive moves the two belts towards the wanted speed difference (right - left),
	//! as much as its force can change their speeds in one step. Steer > 0 = right turn: the right belt slower
	protected void Steering(inout float newL, inout float newR, float timeSlice)
	{
		float speed = Math.AbsFloat(m_fCenterSpeed);
		float pivotWeight = Math.Clamp(1 - speed / m_fPivotBlendSpeed, 0, 1);
		if (pivotWeight <= 0)
			return;

		float halfSpeed = m_fSteerHalfSpeedKmh / 3.6;
		float wanted = -m_fSteer * m_fSteerSpeedDiffMax / (1 + speed / halfSpeed);
		float current = newR - newL;
		float correction = 0.5 * (wanted - current) * pivotWeight;

		float capL = m_fSteerForce * timeSlice / m_aBeltMass[0];
		float capR = m_fSteerForce * timeSlice / m_aBeltMass[1];
		float cap = Math.Min(capL, capR);
		correction = Math.Clamp(correction, -cap, cap);

		newL -= correction;
		newR += correction;
	}

	//------------------------------------------------------------------------------------------------
	//! Ground forces of one belt at its final speed, applied wheel by wheel at the contact points, with rolling
	//! resistance and the lateral grip of each contact. Stores the belt speed for the next step and the visuals
	protected void BeltApply(IEntity owner, Physics physics, int track, float load, float vNew, vector forward, vector right)
	{
		m_aBeltSpeed[track] = vNew;
		if (load <= 0)
			return;

		// turning resistance falls with speed: a moving track slides sideways more easily than a standing one
		float latFriction = m_fFrictionLat * (0.45 + 0.55 / (1 + Math.AbsFloat(m_fCenterSpeed) / m_fLateralFrictionHalfSpeed));

		for (int k = 0; k < m_iRoadWheels; k++)
		{
			int i = track * m_iRoadWheels + k;
			if (!m_aContact[i])
				continue;

			float wheelLoad = m_aLoad[i];
			float stiffness = m_fSlipStiffness * wheelLoad / load;
			vector contactPos = ContactPoint(owner, i);
			vector velocity = physics.GetVelocityAt(contactPos);
			float vLng = m_aGroundSpeed[i];
			float vLat = vector.Dot(velocity, right);

			float maxLng = m_fFrictionLng * wheelLoad;
			float fLng = Math.Clamp(stiffness * (vNew - vLng), -maxLng, maxLng);

			// rolling resistance opposes the wheel's own motion
			float rolling = m_fRollingResistance * wheelLoad;
			fLng -= Math.Clamp(vLng * 20, -1, 1) * rolling;

			// lateral grip of this contact against its own sideways slip
			float fLat = -vLat * m_fLateralStiffness * wheelLoad;
			float maxLat = latFriction * wheelLoad;
			fLat = Math.Clamp(fLat, -maxLat, maxLat);

			physics.ApplyForceAt(contactPos, forward * fLng + right * fLat);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! The test schedule at time t: the window (throttle/steer/brake), then the after-window (steer/brake, no throttle)
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
	protected void UpdateTestInputs()
	{
		bool pivot;
		TestSchedule(m_fTime, m_fThrottle, m_fSteer, m_fBrake, pivot);
		m_bPivot = pivot;
		m_fReverse = 0;
	}

	//------------------------------------------------------------------------------------------------
	//! Proxy: what this machine sees of the replicated vehicle, once per second
	protected void DebugProxy(IEntity owner, float timeSlice)
	{
		m_fProxyTimer += timeSlice;
		if (m_fProxyTimer < 1)
			return;

		vector pos = owner.GetOrigin();
		float kmh = vector.Distance(pos, m_vProxyLastPos) / m_fProxyTimer * 3.6;
		m_vProxyLastPos = pos;
		m_fProxyTimer = 0;
		int contacts = 0;
		for (int i = 0; i < m_aContact.Count(); i++)
		{
			if (m_aContact[i])
				contacts++;
		}
		string owning = "n/a";
		if (m_Rpl)
			owning = m_Rpl.IsOwner().ToString();
		Print("LEOSIM proxy pos=" + pos.ToString() + " kmh=" + kmh.ToString() + " rpl beltKmh L/R=" + (m_fRplBeltL * 3.6).ToString() + "/" + (m_fRplBeltR * 3.6).ToString() + " rpm=" + m_fRplRpm.ToString() + " gear=" + m_iRplGear.ToString() + " engine=" + m_bEngineOn.ToString() + " contacts=" + contacts.ToString() + " localDriver=" + m_bLocalDriver.ToString() + " driveTime=" + m_fDriveTime.ToString() + " sent thr=" + m_fSentThrottle.ToString() + " steer=" + m_fSentSteer.ToString() + " brk=" + m_fSentBrake.ToString() + " owner=" + owning);
	}

	//------------------------------------------------------------------------------------------------
	protected void Debug(IEntity owner, Physics physics, float timeSlice, float loadL, float loadR)
	{
		m_fDebugTimer += timeSlice;
		if (m_fDebugTimer < 1)
			return;

		m_fDebugTimer = 0;
		vector pos = owner.GetOrigin();
		vector hull[4];
		owner.GetTransform(hull);
		vector velocity = physics.GetVelocity();
		float kmh = vector.Dot(velocity, hull[2]) * 3.6;
		int contacts = 0;
		for (int k = 0; k < m_iRoadWheels; k++)
		{
			if (m_aContact[k])
				contacts++;
			if (m_aContact[m_iRoadWheels + k])
				contacts++;
		}
		float weight = physics.GetMass() * 9.81;
		float support = (loadL + loadR) / weight;
		float beltL = m_aBeltSpeed[0] * 3.6;
		float beltR = m_aBeltSpeed[1] * 3.6;
		string source = "none";
		if (m_bLocalDriver)
			source = "local";
		else if (GetDriver())
			source = "net(age " + m_fNetAge.ToString() + ")";
		Print("LEOSIM t=" + m_fTime.ToString() + " pos=" + pos.ToString() + " kmh=" + kmh.ToString() + " yawRate=" + physics.GetAngularVelocity()[1].ToString() + " in thr=" + m_fThrottle.ToString() + " rev=" + m_fReverse.ToString() + " steer=" + m_fSteer.ToString() + " brk=" + m_fBrake.ToString() + " driver=" + source + " engine=" + m_bEngineOn.ToString() + " | rpm=" + m_fRpm.ToString() + " gear=" + m_iGear.ToString() + " beltKmh L/R=" + beltL.ToString() + "/" + beltR.ToString() + " contacts=" + contacts.ToString() + " load L/R=" + loadL.ToString() + "/" + loadR.ToString() + " support=" + support.ToString());
	}
}
