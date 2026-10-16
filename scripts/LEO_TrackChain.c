//! One track's link chain, moved by script (owned by LEO_RunningGearComponent). Every update the belt path is rebuilt in
//! the track's plane (hull z = along, y = up) around the sprocket, return rollers, idler and road wheels at their
//! current suspension travel, and the links are spread evenly along it, shifted by the distance the track has run.
//! Unlike the engine's rope chain (VehicleTrackedSimulation TrackSegment), which has gravity but no ground collision,
//! it cannot sag through the ground or come off the sprocket: it only depends on the wheel positions.
//! Path: the circles in belt order (add them so: sprocket, return rollers rear to front, idler, road wheels front to
//! rear), joined by their outer common tangents with the belt outside - seen from the right the loop runs clockwise,
//! top run forward, bottom run back. The belt wraps each circle between its incoming and outgoing tangent; where the
//! turn is concave (a road wheel pushed up above its neighbours) it does not wrap but goes straight between them.
//! Circle radii are the belt's ground face (the simulation radii), as the link model's origin is its ground face.
class LEO_TrackChain
{
	protected static const int ARC = 0;
	protected static const int LINE = 1;

	protected float m_fX;

	// circles in belt order: centre (z, y) and radius
	protected ref array<float> m_aZ = {};
	protected ref array<float> m_aY = {};
	protected ref array<float> m_aR = {};

	// path segments, rebuilt every update. ARC: centre z, y, radius, start angle (runs clockwise);
	// LINE: start z, y, unit direction z, y
	protected ref array<int> m_aKind = {};
	protected ref array<float> m_aLength = {};
	protected ref array<float> m_aA = {};
	protected ref array<float> m_aB = {};
	protected ref array<float> m_aC = {};
	protected ref array<float> m_aD = {};
	protected ref array<float> m_aNormals = {};
	protected float m_fLength;

	protected ref array<IEntity> m_aLinks = {};
	protected float m_fOffset;

	//------------------------------------------------------------------------------------------------
	//! \param[in] x hull x of the track's centre plane
	void LEO_TrackChain(float x)
	{
		m_fX = x;
	}

	//------------------------------------------------------------------------------------------------
	//! Adds the next circle in belt order
	//! \return its index, for SetCircleHeight
	int AddCircle(float z, float y, float radius)
	{
		m_aZ.Insert(z);
		m_aY.Insert(y);
		m_aNormals.Insert(0);
		return m_aR.Insert(radius);
	}

	//------------------------------------------------------------------------------------------------
	void SetCircleHeight(int index, float y)
	{
		m_aY[index] = y;
	}

	//------------------------------------------------------------------------------------------------
	//! Spawns the link entities as children of the owner (local only, not replicated)
	//! \return false if an entity could not be spawned
	bool SpawnLinks(notnull IEntity owner, notnull VObject model, int count)
	{
		for (int i = 0; i < count; i++)
		{
			IEntity link = GetGame().SpawnEntity(GenericEntity, owner.GetWorld());
			if (!link)
				return false;

			owner.AddChild(link, -1, EAddChildFlags.AUTO_TRANSFORM);
			link.SetObject(model, string.Empty);
			link.ClearFlags(EntityFlags.TRACEABLE | EntityFlags.ACTIVE, false);
			m_aLinks.Insert(link);
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool IsOwnLink(IEntity entity)
	{
		return m_aLinks.Find(entity) != -1;
	}

	//------------------------------------------------------------------------------------------------
	void DeleteLinks()
	{
		foreach (IEntity link : m_aLinks)
		{
			if (link)
				delete link;
		}

		m_aLinks.Clear();
	}

	//------------------------------------------------------------------------------------------------
	//! Rebuilds the path and places the links
	//! \param[in] distance how far the track ran since the last update [m], + = top run forward
	void Update(float distance)
	{
		BuildPath();
		int count = m_aLinks.Count();
		if (m_fLength <= 0 || count == 0)
			return;

		m_fOffset = m_fOffset + distance;
		m_fOffset = m_fOffset - m_fLength * Math.Floor(m_fOffset / m_fLength);

		float spacing = m_fLength / count;
		int segment;
		float segmentStart;
		float previous = -1;
		vector mat[4];
		for (int i = 0; i < count; i++)
		{
			float s = m_fOffset + i * spacing;
			if (s >= m_fLength)
				s -= m_fLength;

			// links come in increasing s with one wrap back to the start of the path
			if (s < previous)
			{
				segment = 0;
				segmentStart = 0;
			}

			previous = s;
			while (segment < m_aKind.Count() - 1 && s > segmentStart + m_aLength[segment])
			{
				segmentStart += m_aLength[segment];
				segment++;
			}

			LinkTransform(segment, s - segmentStart, mat);
			if (m_aLinks[i])
				m_aLinks[i].SetLocalTransform(mat);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void BuildPath()
	{
		m_aKind.Clear();
		m_aLength.Clear();
		m_aA.Clear();
		m_aB.Clear();
		m_aC.Clear();
		m_aD.Clear();
		m_fLength = 0;

		int n = m_aR.Count();
		if (n < 3)
			return;

		// outer common tangent from circle i to circle i + 1: angle of its outward normal, the same at both circles
		for (int i = 0; i < n; i++)
		{
			int next = (i + 1) % n;
			float dz = m_aZ[next] - m_aZ[i];
			float dy = m_aY[next] - m_aY[i];
			float distance = Math.Sqrt(dz * dz + dy * dy);
			float cosine = 0;
			if (distance > 0.0001)
				cosine = Math.Clamp((m_aR[i] - m_aR[next]) / distance, -1, 1);

			m_aNormals[i] = Math.Atan2(dy, dz) + Math.Acos(cosine);
		}

		for (int i = 0; i < n; i++)
		{
			float inAngle = m_aNormals[(i + n - 1) % n];
			float outAngle = m_aNormals[i];
			float startZ = m_aZ[i] + m_aR[i] * Math.Cos(inAngle);
			float startY = m_aY[i] + m_aR[i] * Math.Sin(inAngle);
			float endZ = m_aZ[i] + m_aR[i] * Math.Cos(outAngle);
			float endY = m_aY[i] + m_aR[i] * Math.Sin(outAngle);

			// clockwise wrap from the incoming to the outgoing tangent; more than half a turn = concave corner
			float sweep = inAngle - outAngle;
			sweep = sweep - Math.PI2 * Math.Floor(sweep / Math.PI2);
			if (sweep <= Math.PI)
				AddSegment(ARC, m_aR[i] * sweep, m_aZ[i], m_aY[i], m_aR[i], inAngle);
			else
				AddLine(startZ, startY, endZ, endY);

			int next = (i + 1) % n;
			AddLine(endZ, endY, m_aZ[next] + m_aR[next] * Math.Cos(outAngle), m_aY[next] + m_aR[next] * Math.Sin(outAngle));
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void AddLine(float z0, float y0, float z1, float y1)
	{
		float dz = z1 - z0;
		float dy = y1 - y0;
		float length = Math.Sqrt(dz * dz + dy * dy);
		if (length < 0.00001)
			return;

		AddSegment(LINE, length, z0, y0, dz / length, dy / length);
	}

	//------------------------------------------------------------------------------------------------
	protected void AddSegment(int kind, float length, float a, float b, float c, float d)
	{
		if (length < 0.00001)
			return;

		m_aKind.Insert(kind);
		m_aLength.Insert(length);
		m_aA.Insert(a);
		m_aB.Insert(b);
		m_aC.Insert(c);
		m_aD.Insert(d);
		m_fLength += length;
	}

	//------------------------------------------------------------------------------------------------
	//! Link transform in hull space at distance u along a segment: origin on the path (the link's ground face),
	//! Z along the path (direction of increasing s), Y towards the wheels (the link's inner face), X = Y x Z.
	protected void LinkTransform(int segment, float u, out vector mat[4])
	{
		float z, y, tz, ty;
		if (m_aKind[segment] == ARC)
		{
			float angle = m_aD[segment] - u / m_aC[segment];
			float c = Math.Cos(angle);
			float s = Math.Sin(angle);
			z = m_aA[segment] + m_aC[segment] * c;
			y = m_aB[segment] + m_aC[segment] * s;
			tz = s;
			ty = -c;
		}
		else
		{
			tz = m_aC[segment];
			ty = m_aD[segment];
			z = m_aA[segment] + u * tz;
			y = m_aB[segment] + u * ty;
		}

		mat[0] = Vector(-1, 0, 0);
		mat[1] = Vector(0, -tz, ty);
		mat[2] = Vector(0, ty, tz);
		mat[3] = Vector(m_fX, y, z);
	}
}
