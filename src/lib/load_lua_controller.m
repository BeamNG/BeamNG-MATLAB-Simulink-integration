function load_lua_controller(vehicle, debug_flag)
    % load_lua_controller is a function that is used to load the lua
    % controller to use the simulink bridge. It takes as input the desired
    % instance of the beamngpy vehicle class and a boolean debug flag to
    % load the controller in debug mode (saves .CSV files).
    
    if debug_flag
        vehicle.queue_lua_command("controller.loadControllerExternal" + ...
                          "('tech/vehicleSystemsCoupling', " + ...
                          "'vehicleSystemsCoupling', {debugFile=true})")
    else
        vehicle.queue_lua_command("controller.loadControllerExternal" + ...
                          "('tech/vehicleSystemsCoupling', " + ...
                          "'vehicleSystemsCoupling', {})")
    end
end

