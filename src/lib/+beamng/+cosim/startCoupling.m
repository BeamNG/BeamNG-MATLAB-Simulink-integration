function startCoupling(vehicle,debug)
%STARTCOUPLING  Start co-simulation coupling.
%   startCoupling(vehicle,debug) starts co-simulation coupling on the given
%   vehicle (beamngpy Vehicle class instance). The flag debug is a boolean
%   flag indicating if debug mode should be used (default false).
if nargin < 2
    debug = false;
end
if debug
    debug_arg = '{debugFile=true}';
else
    debug_arg = '{}';
end
cmd = "controller.loadControllerExternal" + ...
      "('tech/vehicleSystemsCoupling', " + ...
      sprintf("'vehicleSystemsCoupling', %s)",debug_arg);
vehicle.queue_lua_command(cmd);
end
