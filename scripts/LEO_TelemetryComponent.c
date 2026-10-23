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
				// visual bounds of the turret model around its own origin, and the model root, to locate the forward offset
				vector mins, maxs;
				child.GetBounds(mins, maxs);
				PrintFormat("LEOTEL turret bounds mins=%1 maxs=%2", mins, maxs);
				LogBones(child, {"Scene_Root", "v_body", "v_gun_01", "v_muzzle", "gunner_idle", "gunner_getIn", "v_gunner_sight", "commander_idle", "v_commander_sight", "v_loader_hatch"});
				LogBones(owner, {"v_turret_slot", "v_wheel_l01", "v_sprocket_l"});
				// weapons and sub-turrets hang under the turret: where do they really sit (turret space, turret at 0 yaw)?
				IEntity part = child.GetChildren();
				while (part)
				{
					vector partLocal = child.CoordToLocal(part.GetOrigin());
					part.GetBounds(mins, maxs);
					PrintFormat("LEOTEL turret child %1 local=%2 bounds=%3 .. %4", part, partLocal, mins, maxs);
					LogBonesIn(part, child, {"barrel_muzzle", "Scene_Root", "v_gun_01"});
					part = part.GetSibling();
				}

				m_bTurretLogged = true;
				return;
			}

			child = child.GetSibling();
		}
	}

	protected ref array<IEntity> m_aLoggedOccupants = {};
	protected float m_fCameraTimer;

	//------------------------------------------------------------------------------------------------
	//! Logs, whenever a seat changes occupant, where the character and its head sit in hull space, and every 2 s the
	//! camera position while the local player is inside (to place seats, hatches and optics).
	protected void LogOccupants(IEntity owner, float timeSlice)
	{
		BaseCompartmentManagerComponent manager = BaseCompartmentManagerComponent.Cast(owner.FindComponent(BaseCompartmentManagerComponent));
		if (!manager)
			return;

		array<BaseCompartmentSlot> slots = {};
		manager.GetCompartments(slots);
		while (m_aLoggedOccupants.Count() < slots.Count())
			m_aLoggedOccupants.Insert(null);

		IEntity player = SCR_PlayerController.GetLocalControlledEntity();
		bool playerInside;
		foreach (int i, BaseCompartmentSlot slot : slots)
		{
			IEntity occupant = slot.GetOccupant();
			if (occupant && occupant == player)
				playerInside = true;

			if (occupant == m_aLoggedOccupants[i])
				continue;

			m_aLoggedOccupants[i] = occupant;
			if (!occupant)
				continue;

			vector pos = owner.CoordToLocal(occupant.GetOrigin());
			vector head = pos;
			Animation anim = occupant.GetAnimation();
			if (anim)
			{
				TNodeId bone = anim.GetBoneIndex("Head");
				vector mat[4];
				if (bone != -1 && anim.GetBoneMatrix(bone, mat))
					head = owner.CoordToLocal(occupant.CoordToParent(mat[3]));
			}

			PrintFormat("LEOTEL occupant slot=%1 (%2) pos=%3 head=%4", i, slot.GetOwner(), pos, head);
		}

		if (!playerInside)
			return;

		m_fCameraTimer += timeSlice;
		if (m_fCameraTimer < 2)
			return;

		m_fCameraTimer = 0;
		CameraBase camera = GetGame().GetCameraManager().CurrentCamera();
		if (camera)
			PrintFormat("LEOTEL camera %1 local=%2", camera, owner.CoordToLocal(camera.GetOrigin()));
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
	//! Logs where bones of a child entity (weapon, sub-turret) sit in the space of another entity (the turret).
	//! Bones the model does not have are skipped silently.
	protected void LogBonesIn(IEntity entity, IEntity space, array<string> names)
	{
		Animation anim = entity.GetAnimation();
		if (!anim)
			return;

		vector world[4];
		entity.GetWorldTransform(world);
		foreach (string name : names)
		{
			TNodeId bone = anim.GetBoneIndex(name);
			vector mat[4];
			if (bone == -1 || !anim.GetBoneMatrix(bone, mat))
				continue;

			// bone matrices are in model space: model -> world -> turret space
			PrintFormat("LEOTEL child bone %1 of %2 in turret space: %3", name, entity.GetPrefabData().GetPrefabName(), space.CoordToLocal(mat[3].Multiply4(world)));
		}
	}

	[Attribute("0.4186", desc: "Modelled road wheel centre height (bone v_wheel_l01 at rest) [m], for the absolute suspension travel in LEOSUSP")]
	protected float m_fWheelRestHeight;

	protected float m_fSuspensionTimer;

	//------------------------------------------------------------------------------------------------
	//! Every 2 s with the engine on, per track: how far each animated road wheel bone (v_wheel_<s>0X, model space,
	//! includes the CarProcAnim suspension translation) sits from where the simulation has that wheel (ground
	//! contact + radius, hull space), in mm, the simulation's travel from the modelled height (mm, + = up), and the
	//! spin of each road wheel and the sprocket about the axle.
	//! With LEO_RunningGearComponent driving the bones all diffs are ~0 and a track's road wheel spins are equal.
	//! (2026-10-05 runs 1 and 2: CarProcAnim, whatever the param order, gave bone v_wheel_l0X the signals of another
	//! wheel - it looks a wheel up as 13 * track + part but reads a per-axle table: left 0, right 0, left 1, ...)
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
		if (!m_bTurretLogged)
			LogTurretPlacement(owner);

		LogOccupants(owner, m_fInterval);

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
