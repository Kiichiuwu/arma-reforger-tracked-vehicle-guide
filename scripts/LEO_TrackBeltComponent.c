//! The hull model keeps its original static track belts (meshes "track_l" / "track_r") as a fallback. The moving belt is
//! made of link entities: the engine's own link chain (VehicleTrackedSimulation spawns one child entity per link when
//! TrackPositions / TrackLength / TrackSegment are set) and/or the scripted chain of LEO_RunningGearComponent, which
//! hides the engine's links and spawns its own. Both are children of the hull with the link model.
//! This component hides the static belts once at least 2 such link entities exist, so a broken or missing chain never
//! leaves the tank without tracks. Runs on every machine (mesh visibility is local).
[ComponentEditorProps(category: "Leopard2A7", description: "Hides the static track belt meshes once the native link chain has spawned its links")]
class LEO_TrackBeltComponentClass : ScriptComponentClass
{
}

class LEO_TrackBeltComponent : ScriptComponent
{
	[Attribute("track_l track_r", desc: "Names of the hull meshes that form the static belts, separated by spaces")]
	protected string m_sStaticBeltMeshes;

	[Attribute("Leopard2A6_track_link", desc: "Part of the link model file name, used to recognise the chain's link entities among the hull's children")]
	protected string m_sLinkModelName;

	[Attribute("5", desc: "Seconds to wait for the chain before giving up and keeping the static belts")]
	protected float m_fTimeout;

	protected float m_fTimer;

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.FRAME);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		m_fTimer += timeSlice;
		int links = CountLinks(owner);
		if (links >= 2)
		{
			int hidden = ShowStaticBelts(owner, false);
			PrintFormat("LEOTRACK link chain found (%1 links): %2 static belt meshes hidden", links, hidden);
			ClearEventMask(owner, EntityEvent.FRAME);
			return;
		}

		if (m_fTimer > m_fTimeout)
		{
			Print("LEOTRACK no link chain after " + m_fTimeout.ToString() + " s: static belts kept", LogLevel.WARNING);
			ClearEventMask(owner, EntityEvent.FRAME);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected int CountLinks(IEntity owner)
	{
		int count;
		IEntity child = owner.GetChildren();
		while (child)
		{
			VObject model = child.GetVObject();
			if (model && model.GetResourceName().Contains(m_sLinkModelName))
				count++;

			child = child.GetSibling();
		}

		return count;
	}

	//------------------------------------------------------------------------------------------------
	//! \return number of meshes found and switched
	protected int ShowStaticBelts(IEntity owner, bool show)
	{
		Animation anim = owner.GetAnimation();
		if (!anim)
			return 0;

		array<string> names = {};
		m_sStaticBeltMeshes.Split(" ", names, true);
		int switched;
		foreach (string name : names)
		{
			TMeshIndex mesh = anim.GetNamedMeshIndex(name);
			if (mesh == -1)
			{
				Print("LEOTRACK static belt mesh not found: " + name, LogLevel.WARNING);
				continue;
			}

			anim.ShowMesh(mesh, show);
			switched++;
		}

		return switched;
	}
}
