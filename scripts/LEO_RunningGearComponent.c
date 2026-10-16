//! Animates the running gear from the VehicleTrackedSimulation every frame: road wheel suspension travel and spin, the
//! spin of idlers, sprockets and return rollers (bone matrices, as SCR_DestructionTireComponent does), and the track
//! links (LEO_TrackChain, one per track).
//! It replaces CarProcAnimComponent for these bones: on tracked vehicles (Workbench 1.8.0.13) CarProcAnim gave every
//! wheel bone except the first left one another wheel's spin and travel (LEOSUSP telemetry, 2026-10-05), whatever the
//! order of its params. The simulation getters take (track, part) and are unambiguous.
//! It replaces the engine's rope chain for the links: that rope has gravity but no ground collision, so it sagged
//! through the ground when slack and came off the sprocket when the tank was thrown (flip, mines). The rope still runs
//! (the prefab keeps TrackSegment, the wheel RPM might depend on it) but its links are hidden.
//! Bones per side s = l / r (identity rest orientation, so bone space = hull space):
//!   v_suspension_s0k (moved along Y by the travel) -> v_wheel_s0k_rotator -> v_wheel_s0k (spun about X), k = 1..n,
//!   v_idler_s, v_sprocket_s and v_roller_s0k (spun about X).
//! TrackParts of each track, in this order: road wheels 0..n-1 (front to rear), idler n, sprocket n+1, return rollers.
//! The idler must be at the front and the sprocket at the rear (LEO_TrackChain's belt order).
[ComponentEditorProps(category: "Leopard2A7", description: "Animates road wheels (suspension and spin), idlers, sprockets, return rollers and the track links from the tracked simulation")]
class LEO_RunningGearComponentClass : ScriptComponentClass
{
}

class LEO_RunningGearComponent : ScriptComponent
{
	[Attribute("7", desc: "Road wheels per track (TrackParts 0..n-1), bones v_suspension_<s>0<k> and v_wheel_<s>0<k>")]
	protected int m_iRoadWheels;

	[Attribute("4", desc: "Return rollers per track (TrackParts after the sprocket), bones v_roller_<s>0<k>")]
	protected int m_iRollers;

	[Attribute("0.345", desc: "Visual road wheel rim radius, where it rolls on the track's inner face [m]")]
	protected float m_fRoadWheelRadius;

	[Attribute("0.0847", desc: "Visual return roller rim radius, where it rolls on the track's inner face [m]")]
	protected float m_fRollerRadius;

	[Attribute("0.4186", desc: "Modelled road wheel centre height [m], used if a wheel bone cannot be read")]
	protected float m_fRestHeight;

	[Attribute("0.10", desc: "Travel down from the modelled wheel position [m], same as the simulation's Suspension MaxTravelDown")]
	protected float m_fMaxTravelDown;

	[Attribute("0.30", desc: "Travel up from the modelled wheel position [m], same as the simulation's Suspension MaxTravelUp")]
	protected float m_fMaxTravelUp;

	[Attribute("8", desc: "How fast a wheel drops to full travel when it loses ground contact, and comes back when it lands [1/s]. With contact the wheel follows the simulation exactly")]
	protected float m_fDropRate;

	[Attribute("1", desc: "Move the track links by script (and hide the engine's rope chain)")]
	protected bool m_bScriptedChain;

	[Attribute("{B50AEDC702735F50}Assets/Vehicles/Tracked/Leopard2A6/Leopard2A6_track_link.xob", UIWidgets.ResourcePickerThumbnail, desc: "Track link model", params: "xob")]
	protected ResourceName m_sLinkModel;

	[Attribute("73", desc: "Links per track")]
	protected int m_iLinks;

	[Attribute("Leopard2A6_track_link", desc: "Part of the engine chain's link model name, to find and hide its links among the hull's children")]
	protected string m_sNativeLinkName;

	[Attribute("300", desc: "Beyond this distance from the camera the running gear is not updated [m]")]
	protected float m_fMaxDistance;

	protected TrackedControllerComponent m_Controller;
	protected bool m_bChecked;

	// road wheels, index track * m_iRoadWheels + k
	protected ref array<TNodeId> m_aSuspensionBones = {};
	protected ref array<TNodeId> m_aWheelBones = {};
	protected ref array<vector> m_aWheelRest = {};
	protected ref array<float> m_aTravel = {};
	protected ref array<bool> m_aContact = {};
	protected ref array<int> m_aChainCircles = {};

	// index track (idler, sprocket) or track * m_iRollers + k (rollers)
	protected ref array<TNodeId> m_aIdlerBones = {};
	protected ref array<TNodeId> m_aSprocketBones = {};
	protected ref array<TNodeId> m_aRollerBones = {};
	protected ref array<vector> m_aIdlerRest = {};
	protected ref array<vector> m_aSprocketRest = {};
	protected ref array<vector> m_aRollerRest = {};

	// spin angles [rad], index track * 4 + (0 road wheels, 1 idler, 2 sprocket, 3 rollers)
	protected ref array<float> m_aAngles = {0, 0, 0, 0, 0, 0, 0, 0};

	protected ref array<ref LEO_TrackChain> m_aChains = {};
	protected float m_fNativeLinkTimer;
	protected float m_fNativeLinkAge;

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		if (System.IsConsoleApp())
			return;

		m_Controller = TrackedControllerComponent.Cast(owner.FindComponent(TrackedControllerComponent));
		Animation anim = owner.GetAnimation();
		if (!m_Controller || !anim)
		{
			Print("LEOGEAR no TrackedControllerComponent or no skeleton: running gear not animated", LogLevel.WARNING);
			return;
		}

		int missing;
		for (int track = 0; track < 2; track++)
		{
			string side = "l";
			if (track == 1)
				side = "r";

			for (int k = 1; k <= m_iRoadWheels; k++)
			{
				TNodeId suspension = anim.GetBoneIndex("v_suspension_" + side + "0" + k.ToString());
				TNodeId wheel = anim.GetBoneIndex("v_wheel_" + side + "0" + k.ToString());
				vector rest;
				if (!ReadRest(anim, wheel, rest) || rest[1] < 0.5 * m_fRestHeight || rest[1] > 2 * m_fRestHeight)
				{
					missing++;
					rest[1] = m_fRestHeight;
				}

				if (suspension == -1)
					missing++;

				m_aSuspensionBones.Insert(suspension);
				m_aWheelBones.Insert(wheel);
				m_aWheelRest.Insert(rest);
				m_aTravel.Insert(0);
				m_aContact.Insert(true);
			}

			vector idler, sprocket;
			m_aIdlerBones.Insert(anim.GetBoneIndex("v_idler_" + side));
			m_aSprocketBones.Insert(anim.GetBoneIndex("v_sprocket_" + side));
			if (!ReadRest(anim, m_aIdlerBones[track], idler))
				missing++;

			if (!ReadRest(anim, m_aSprocketBones[track], sprocket))
				missing++;

			m_aIdlerRest.Insert(idler);
			m_aSprocketRest.Insert(sprocket);
			for (int k = 1; k <= m_iRollers; k++)
			{
				TNodeId roller = anim.GetBoneIndex("v_roller_" + side + "0" + k.ToString());
				vector rollerRest;
				if (!ReadRest(anim, roller, rollerRest))
					missing++;

				m_aRollerBones.Insert(roller);
				m_aRollerRest.Insert(rollerRest);
			}
		}

		if (missing > 0)
		{
			Print("LEOGEAR " + missing.ToString() + " running gear bones missing or unreadable: the scripted chain is off", LogLevel.WARNING);
			m_bScriptedChain = false;
		}

		SetEventMask(owner, EntityEvent.FRAME);
	}

	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		foreach (LEO_TrackChain chain : m_aChains)
		{
			chain.DeleteLinks();
		}

		super.OnDelete(owner);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		VehicleTrackedSimulation simulation = m_Controller.GetSimulation();
		Animation anim = owner.GetAnimation();
		// TrackCount() is 0 until the simulation is loaded, while the wheel getters return nothing useful
		if (!simulation || !simulation.IsValid() || !anim || simulation.TrackCount() < 2)
			return;

		if (!m_bChecked && !CheckSimulation(owner, simulation))
			return;

		vector camera[4];
		owner.GetWorld().GetCurrentCamera(camera);
		bool visible = vector.DistanceSq(owner.GetOrigin(), camera[3]) < m_fMaxDistance * m_fMaxDistance;

		if (m_bScriptedChain && m_aChains.IsEmpty())
			CreateChains(owner, simulation);

		if (m_bScriptedChain)
			HideNativeLinks(owner, timeSlice);

		float drop = 1 - Math.Pow(2.718282, -m_fDropRate * timeSlice);
		vector mat[4];
		for (int track = 0; track < 2; track++)
		{
			// how far the track ran relative to the hull this frame (+ = top run forward), at its ground face: the
			// speed of road wheel 0's surface. Wrapped parts (idler, sprocket) turn with the links around them, at the
			// chain path radius (their simulation radius); the others roll on the track's inner face
			float distance = simulation.WheelGetRPM(track, 0) * Math.PI2 / 60 * simulation.WheelGetRadius(track, 0) * timeSlice;
			float roadAngle = Advance(track * 4, distance / m_fRoadWheelRadius);
			float idlerAngle = Advance(track * 4 + 1, distance / simulation.WheelGetRadius(track, m_iRoadWheels));
			float sprocketAngle = Advance(track * 4 + 2, distance / simulation.WheelGetRadius(track, m_iRoadWheels + 1));
			float rollerAngle = Advance(track * 4 + 3, distance / m_fRollerRadius);

			for (int k = 0; k < m_iRoadWheels; k++)
			{
				int i = track * m_iRoadWheels + k;

				// with contact: wheel centre = contact point + radius along the hull's up axis (the suspension ray is
				// cast along it), exactly as the simulation has it; without: hanging at full droop, eased in and out
				bool contact = simulation.WheelHasContact(track, k);
				if (contact)
				{
					vector point = owner.CoordToLocal(simulation.WheelGetContactPosition(track, k));
					float target = Math.Clamp(point[1] + simulation.WheelGetRadius(track, k) - m_aWheelRest[i][1], -m_fMaxTravelDown, m_fMaxTravelUp);
					if (m_aContact[i])
						m_aTravel[i] = target;
					else
						m_aTravel[i] = m_aTravel[i] + (target - m_aTravel[i]) * drop;

					// landed: keep easing until the wheel has caught up
					m_aContact[i] = Math.AbsFloat(target - m_aTravel[i]) < 0.005;
				}
				else
				{
					m_aTravel[i] = m_aTravel[i] + (-m_fMaxTravelDown - m_aTravel[i]) * drop;
					m_aContact[i] = false;
				}

				if (!visible)
					continue;

				if (m_aSuspensionBones[i] != -1)
				{
					Math3D.MatrixIdentity4(mat);
					mat[3] = Vector(0, m_aTravel[i], 0);
					anim.SetBoneMatrix(owner, m_aSuspensionBones[i], mat);
				}

				Spin(owner, anim, m_aWheelBones[i], roadAngle);
				if (m_aChains.IsIndexValid(track))
					m_aChains[track].SetCircleHeight(m_aChainCircles[i], m_aWheelRest[i][1] + m_aTravel[i]);
			}

			if (!visible)
				continue;

			Spin(owner, anim, m_aIdlerBones[track], idlerAngle);
			Spin(owner, anim, m_aSprocketBones[track], sprocketAngle);
			for (int k = 0; k < m_iRollers; k++)
			{
				Spin(owner, anim, m_aRollerBones[track * m_iRollers + k], rollerAngle);
			}

			if (m_aChains.IsIndexValid(track))
				m_aChains[track].Update(distance);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Once, on the first frame with a loaded simulation: are there as many TrackParts as this component expects?
	protected bool CheckSimulation(IEntity owner, VehicleTrackedSimulation simulation)
	{
		m_bChecked = true;
		// WheelCount() may be per track or for both tracks: only require one track's worth
		int parts = simulation.WheelCount();
		PrintFormat("LEOGEAR simulation: %1 tracks, WheelCount %2", simulation.TrackCount(), parts);
		if (parts >= m_iRoadWheels + 2 + m_iRollers)
			return true;

		PrintFormat("LEOGEAR the simulation has %1 wheels, expected %2 road wheels + idler + sprocket + %3 rollers per track: running gear not animated", parts, m_iRoadWheels, m_iRollers);
		ClearEventMask(owner, EntityEvent.FRAME);
		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected void CreateChains(IEntity owner, VehicleTrackedSimulation simulation)
	{
		Resource resource = Resource.Load(m_sLinkModel);
		VObject model;
		if (resource && resource.IsValid())
			model = resource.GetResource().ToVObject();

		if (!model)
		{
			Print("LEOGEAR link model not found: " + m_sLinkModel + " - the scripted chain is off", LogLevel.WARNING);
			m_bScriptedChain = false;
			return;
		}

		m_aChainCircles.Clear();
		for (int track = 0; track < 2; track++)
		{
			LEO_TrackChain chain = new LEO_TrackChain(m_aWheelRest[track * m_iRoadWheels][0]);

			// belt order: sprocket, return rollers rear to front, idler, road wheels front to rear. Circle radii are the
			// simulation radii: the ground face of the links, where the engine's own chain ran
			vector sprocket = m_aSprocketRest[track];
			chain.AddCircle(sprocket[2], sprocket[1], simulation.WheelGetRadius(track, m_iRoadWheels + 1));
			array<int> rollers = SortedByZ(m_aRollerRest, track * m_iRollers, m_iRollers, true);
			foreach (int k : rollers)
			{
				vector roller = m_aRollerRest[track * m_iRollers + k];
				chain.AddCircle(roller[2], roller[1], simulation.WheelGetRadius(track, m_iRoadWheels + 2 + k));
			}

			vector idler = m_aIdlerRest[track];
			chain.AddCircle(idler[2], idler[1], simulation.WheelGetRadius(track, m_iRoadWheels));

			array<int> wheels = SortedByZ(m_aWheelRest, track * m_iRoadWheels, m_iRoadWheels, false);
			array<int> circles = {};
			circles.Resize(m_iRoadWheels);
			foreach (int k : wheels)
			{
				vector wheel = m_aWheelRest[track * m_iRoadWheels + k];
				circles[k] = chain.AddCircle(wheel[2], wheel[1], simulation.WheelGetRadius(track, k));
			}

			m_aChainCircles.InsertAll(circles);
			if (!chain.SpawnLinks(owner, model, m_iLinks))
				Print("LEOGEAR could not spawn all track links", LogLevel.WARNING);

			m_aChains.Insert(chain);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Hides the engine rope chain's links (children with its link model that are not ours). Checked 4 times a second
	//! for the first 10 s, while the rope spawns them, then every 2 s
	protected void HideNativeLinks(IEntity owner, float timeSlice)
	{
		m_fNativeLinkAge += timeSlice;
		m_fNativeLinkTimer -= timeSlice;
		if (m_fNativeLinkTimer > 0)
			return;

		m_fNativeLinkTimer = 2;
		if (m_fNativeLinkAge < 10)
			m_fNativeLinkTimer = 0.25;

		IEntity child = owner.GetChildren();
		while (child)
		{
			VObject model = child.GetVObject();
			if (model && (child.GetFlags() & EntityFlags.VISIBLE) && model.GetResourceName().Contains(m_sNativeLinkName) && !IsOwnLink(child))
				child.ClearFlags(EntityFlags.VISIBLE, false);

			child = child.GetSibling();
		}
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsOwnLink(IEntity entity)
	{
		foreach (LEO_TrackChain chain : m_aChains)
		{
			if (chain.IsOwnLink(entity))
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	//! \return indices 0..count-1 of positions[start + i], sorted by z (ascending or descending)
	protected array<int> SortedByZ(array<vector> positions, int start, int count, bool ascending)
	{
		array<int> order = {};
		for (int i = 0; i < count; i++)
		{
			int at = order.Count();
			while (at > 0)
			{
				float previous = positions[start + order[at - 1]][2];
				float current = positions[start + i][2];
				if ((ascending && previous <= current) || (!ascending && previous >= current))
					break;

				at--;
			}

			order.InsertAt(i, at);
		}

		return order;
	}

	//------------------------------------------------------------------------------------------------
	//! Model-space rest position of a bone (bind pose: read before this component moves anything)
	protected bool ReadRest(Animation anim, TNodeId bone, out vector position)
	{
		vector mat[4];
		if (bone == -1 || !anim.GetBoneMatrix(bone, mat))
			return false;

		position = mat[3];
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Adds delta [rad] to the stored angle, wrapped to -pi..pi
	//! \return the new angle
	protected float Advance(int index, float delta)
	{
		float angle = m_aAngles[index] + delta;
		if (angle > Math.PI || angle < -Math.PI)
			angle = angle - Math.PI2 * Math.Round(angle / Math.PI2);

		m_aAngles[index] = angle;
		return angle;
	}

	//------------------------------------------------------------------------------------------------
	//! Rotates the bone about its X axis (the axle). A positive angle moves the top of the wheel forward (+Z), which
	//! is how a wheel turns when the track runs forward
	protected void Spin(IEntity owner, Animation anim, TNodeId bone, float angle)
	{
		if (bone == -1)
			return;

		float c = Math.Cos(angle);
		float s = Math.Sin(angle);
		vector mat[4];
		mat[0] = Vector(1, 0, 0);
		mat[1] = Vector(0, c, s);
		mat[2] = Vector(0, -s, c);
		mat[3] = vector.Zero;
		anim.SetBoneMatrix(owner, bone, mat);
	}
}
