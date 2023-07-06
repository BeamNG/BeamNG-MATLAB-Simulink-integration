clc
mode = 0;

vehicle.teleport(py.tuple([-335, 611, 76]), py.tuple([0, 0, 1, 0.4]));

close_scopes();
open_system("beamng_simulink_interface_demos/Demos' switch/Demo 0/Scope")
pause(0.5)

vehicle.ai.set_mode("span")

vehicle.queue_lua_command("controller.loadControllerExternal" + ...
                          "('tech/vehicleSystemsCoupling', " + ...
                          "'vehicleSystemsCoupling', {})")

set_param('beamng_simulink_interface_demos', 'SimulationCommand', 'start')

stopCommand = input('Press a button to stop the simulation');

set_param('beamng_simulink_interface_demos', 'SimulationCommand', 'stop')

vehicle.ai.set_mode("disable")

pause(1)

beamng.restart_scenario();