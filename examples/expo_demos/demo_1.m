clc
mode = 1;


vehicle.teleport(py.tuple([-920, 870, 75]), py.tuple([0, 0, 1, -1]));

close_scopes();
open_system("beamng_simulink_interface_demos/Demos' switch/Demo 1/Scope")
pause(0.5)

vehicle.queue_lua_command("controller.loadControllerExternal" + ...
                          "('tech/vehicleSystemsCoupling', " + ...
                          "'vehicleSystemsCoupling', {})")
out = sim('beamng_simulink_interface_demos.slx', 'FastRestart', 'on');

beamng.restart_scenario();

