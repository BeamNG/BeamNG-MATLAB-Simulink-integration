clc
mode = 3;

vehicle.teleport(py.tuple([-318, 371, 101.5]), py.tuple([0, 0, 1, 0.4]));

close_scopes();
open_system("beamng_simulink_interface_demos/Demos' switch/Demo 3/Scope")
pause(0.5)

vehicle.queue_lua_command("controller.loadControllerExternal" + ...
                          "('tech/vehicleSystemsCoupling', " + ...
                          "'vehicleSystemsCoupling', {})")

set_param('beamng_simulink_interface_demos', 'SimulationCommand', 'start')

stopCommand = input('Press a button to stop the simulation');

set_param('beamng_simulink_interface_demos', 'SimulationCommand', 'stop')

beamng.restart_scenario();