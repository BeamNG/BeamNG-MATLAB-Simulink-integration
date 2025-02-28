function stopCoupling(vehicle)
%STOPCOUPLING  Stop co-simulation coupling.
%   stopCoupling(vehicle) stops co-simulation coupling on the given
%   vehicle (beamngpy Vehicle class instance).
vehicle.queue_lua_command("extensions." + ...
       "tech_vehicleSystemsCoupling.stopCoupling()")
end
