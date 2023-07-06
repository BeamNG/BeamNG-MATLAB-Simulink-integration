close all
clear
clc

% Start the simulator.
beamng = py.beamngpy.BeamNGpy('localhost', int32(64256), ...
    pyargs('home', '<path/to/beamng>')); % change the path

beamng.open();

% Create a vehicle.
vehicle = py.beamngpy.Vehicle('etk800', ...
    pyargs('model', 'etkc', 'licence', 'BeamNG', 'color', 'Black'));

% Create a scenario.
scenario = py.beamngpy.Scenario('west_coast_usa', 'simulink', ...
    pyargs('description', 'Matlab/Simulink bridge demo'));

scenario.add_vehicle(vehicle)
scenario.make(beamng)
beamng.load_scenario(scenario)
beamng.start_scenario()

vehicle.teleport(py.tuple([-920, 870, 75]), py.tuple([0, 0, 1, -1]));

%% demo 1 data: double line change

demo(1).x1 = -920;
demo(1).x2 = -870;
demo(1).x3 = -820;
demo(1).x4 = -770;

demo(1).y1 = 870;
demo(1).y2 = 875;

demo(1).v_target = 50/3.6;

%% demo 2 data: brake test

demo(2).y = 767;
demo(2).v_target = 80/3.6;
demo(2).x1 = -920;
demo(2).x2 = -700;

%% demo 3 data: interactive

demo(3).torque_gain = 3000;
demo(3).torque_gain_brake = - 5000;
demo(3).torque_bias = 18000;
demo(3).torque_rate = 400;

