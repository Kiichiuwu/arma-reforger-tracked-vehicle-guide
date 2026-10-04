//! LEO_TrackedReverseComponent v4 (based on reverse engineering of the Workbench 1.8.0.13 executable).
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
