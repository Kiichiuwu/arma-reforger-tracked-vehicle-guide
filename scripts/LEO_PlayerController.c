//! Driver inputs of the Leopard's tracked simulations travel through the player controller: it is the one replicated
//! item a client owns, and the server only accepts RplRcver.Server RPCs from an item's owner. The vehicle itself
//! stays owned by the server (LEO_TrackSimComponent / LEO_TrackedDriveComponent take it back when the engine hands it
//! to the driver's client), so a vehicle-level RPC from the driver would be dropped.
modded class SCR_PlayerController
{
	//------------------------------------------------------------------------------------------------
	//! Driver's client: send the inputs for the vehicle with this replication id
	void LEO_SendTrackedInputs(RplId vehicle, float throttle, float brake, float steer, bool handbrake)
	{
		Rpc(LEO_RpcAsk_TrackedInputs, vehicle, throttle, brake, steer, handbrake);
	}

	//------------------------------------------------------------------------------------------------
	void LEO_SendTrackedEngine(RplId vehicle, bool on)
	{
		Rpc(LEO_RpcAsk_TrackedEngine, vehicle, on);
	}

	//------------------------------------------------------------------------------------------------
	//! The vehicle entity behind a replication id, only if this player sits in its driver's seat
	protected IEntity LEO_FindDrivenVehicle(RplId vehicle, out LEO_TrackSimComponent sim, out LEO_TrackedDriveComponent drive)
	{
		sim = null;
		drive = null;
		RplComponent rpl = RplComponent.Cast(Replication.FindItem(vehicle));
		if (!rpl)
			return null;

		IEntity entity = rpl.GetEntity();
		if (!entity)
			return null;

		IEntity driver;
		sim = LEO_TrackSimComponent.Cast(entity.FindComponent(LEO_TrackSimComponent));
		if (sim)
		{
			driver = sim.GetDriver();
		}
		else
		{
			drive = LEO_TrackedDriveComponent.Cast(entity.FindComponent(LEO_TrackedDriveComponent));
			if (!drive)
				return null;

			driver = drive.GetDriver();
		}

		// only the player sitting in the driver's seat may drive
		if (!driver || driver != GetControlledEntity())
		{
			sim = null;
			drive = null;
			return null;
		}

		return entity;
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Unreliable, RplRcver.Server)]
	protected void LEO_RpcAsk_TrackedInputs(RplId vehicle, float throttle, float brake, float steer, bool handbrake)
	{
		LEO_TrackSimComponent sim;
		LEO_TrackedDriveComponent drive;
		if (!LEO_FindDrivenVehicle(vehicle, sim, drive))
			return;

		if (sim)
			sim.ReceiveNetInputs(throttle, brake, steer, handbrake);
		else if (drive)
			drive.ReceiveNetInputs(throttle, brake, steer, handbrake);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void LEO_RpcAsk_TrackedEngine(RplId vehicle, bool on)
	{
		LEO_TrackSimComponent sim;
		LEO_TrackedDriveComponent drive;
		if (!LEO_FindDrivenVehicle(vehicle, sim, drive))
			return;

		if (sim)
			sim.SetEngine(on);
		else if (drive)
			drive.SetEngine(on);
	}
}
