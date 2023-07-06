clc
mode = 2;

vehicle.teleport(py.tuple([-920, 767, 92.5]), py.tuple([0, 0, 1, -1]));

close_scopes();
open_system("beamng_simulink_interface_demos/Demos' switch/Demo 2/Scope")
pause(0.5)

vehicle.queue_lua_command("controller.loadControllerExternal" + ...
                          "('tech/vehicleSystemsCoupling', " + ...
                          "'vehicleSystemsCoupling', {})")
out = sim('beamng_simulink_interface_demos.slx', 'FastRestart', 'on');

beamng.restart_scenario();