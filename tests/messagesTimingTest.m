close all
clear
clc

% Start the simulator.
beamng = py.beamngpy.BeamNGpy('localhost', int32(25252));

beamng.open();

% Create a vehicle.
vehicle = py.beamngpy.Vehicle('etk800', ...
    pyargs('model', 'etkc', 'licence', 'BeamNG', 'color', 'Black'));

% Create a scenario.
scenario = py.beamngpy.Scenario('smallgrid', 'simulink', ...
    pyargs('description', 'Message Consitency Test'));

scenario.add_vehicle(vehicle)
scenario.make(beamng)
beamng.load_scenario(scenario)
beamng.start_scenario()

pause(2)

load_lua_controller(vehicle, true);

set_param('testModel', 'SimulationCommand', 'start')

pause(60)

beamng.restart_scenario();
beamng.close()

pause(4)

simulinkData = out.simulinkData.Data;

path2csv = strcat(getenv('USERPROFILE'), '\AppData\Local\BeamNG.drive\0.30\simulinkSendLog.csv');
luaSendData = readmatrix(path2csv);
luaSendData = luaSendData(2:end, :);

path2csv = strcat(getenv('USERPROFILE'), '\AppData\Local\BeamNG.drive\0.30\simulinkReceiveLog.csv');
luaReceiveData = readmatrix(path2csv);
luaReceiveData = luaReceiveData(2:end, :);

path2csv = strcat(getenv('USERPROFILE'), '\AppData\Local\BeamNG.drive\0.30\simulinkPhysicsStepsLog.csv');
luaPhysicsStepsData = readmatrix(path2csv);
luaPhysicsStepsData = luaPhysicsStepsData(2:end, :);

%% Plots

startTime = 28.3002;
endTime = 82;
dt = 0.0005;

sendWithBlock = 46.2094;
zoom1 = [sendWithBlock-dt*5 sendWithBlock+dt*10];

middlePoint = 52.8691;
zoom2 = [middlePoint-dt*10 middlePoint+dt*20];

figure

% Total Plot
subplot(2, 2, 1:2)
grid on
hold on

plot(luaPhysicsStepsData(:, 1), zeros(size(luaPhysicsStepsData(:, 1))), 'g|', 'MarkerSize', 200)
plot(luaSendData(:, 1), ones(size(luaSendData(:, 1))), 'r*')
plot(luaReceiveData(:, 1), - ones(size(luaReceiveData(:, 1))), 'b*')

%xlim([startTime endTime])
ylim([-2 2])
legend('Physics Steps', 'Sent', 'Received')
title('Simulink interface message timing')
xlabel('Time [s]')

% Zoom on blocking
subplot(2, 2, 3)
grid on
hold on

physicsStepsElements = luaPhysicsStepsData(:, 1) > zoom1(1) & luaPhysicsStepsData(:, 1) < zoom1(2);
sendElements = luaSendData(:, 1) > zoom1(1) & luaSendData(:, 1) < zoom1(2);
receiveElements = luaReceiveData(:, 1) > zoom1(1) & luaReceiveData(:, 1) < zoom1(2);

plot(luaPhysicsStepsData(physicsStepsElements, 1), zeros(size(luaPhysicsStepsData(physicsStepsElements, 1))), 'g|', 'MarkerSize', 200, 'LineWidth', 2)
plot(luaSendData(sendElements, 1), ones(size(luaSendData(sendElements, 1))), 'r*')
text(luaSendData(sendElements, 1), 1.1 * ones(size(luaSendData(sendElements, 1))), num2str(luaSendData(sendElements, 2)), 'Color', 'red')
plot(luaReceiveData(receiveElements, 1), - ones(size(luaReceiveData(receiveElements, 1))), 'b*')
text(luaReceiveData(receiveElements, 1), - 1.1 * ones(size(luaReceiveData(receiveElements, 1))), num2str(luaReceiveData(receiveElements, 2)), 'Color', 'blue')

xlim(zoom1)
ylim([-2 2])
legend('Physics Steps', 'Sent')
title('Zoom on blocking receive case')
xlabel('Time [s]')

% Zoom on startup
subplot(2, 2, 4)
grid on
hold on

physicsStepsElements = luaPhysicsStepsData(:, 1) > zoom2(1) & luaPhysicsStepsData(:, 1) < zoom2(2);
sendElements = luaSendData(:, 1) > zoom2(1) & luaSendData(:, 1) < zoom2(2);
receiveElements = luaReceiveData(:, 1) > zoom2(1) & luaReceiveData(:, 1) < zoom2(2);

plot(luaPhysicsStepsData(physicsStepsElements, 1), zeros(size(luaPhysicsStepsData(physicsStepsElements, 1))), 'g|', 'MarkerSize', 200, 'LineWidth', 2)
plot(luaSendData(sendElements, 1), ones(size(luaSendData(sendElements, 1))), 'r*')
text(luaSendData(sendElements, 1), 1.1 * ones(size(luaSendData(sendElements, 1))), num2str(luaSendData(sendElements, 2)), 'Color', 'red')
plot(luaReceiveData(receiveElements, 1), - ones(size(luaReceiveData(receiveElements, 1))), 'b*')
text(luaReceiveData(receiveElements, 1), - 1.1 * ones(size(luaReceiveData(receiveElements, 1))), num2str(luaReceiveData(receiveElements, 2)), 'Color', 'blue')

xlim(zoom2)
ylim([-2 2])
legend('Physics Steps', 'Sent', 'Received')
title('Zoom on working connection')
xlabel('Time [s]')


%% Counter
dt = endTime - startTime;
steps = sum(luaPhysicsStepsData(:, 1) > startTime & luaPhysicsStepsData(:, 1) < endTime);
sends = sum(luaSendData(:, 1) > startTime & luaSendData(:, 1) < endTime);
recvs = sum(luaReceiveData(:, 1) > startTime & luaReceiveData(:, 1) < endTime);

disp(strcat("Physics steps frequency: ", num2str(steps/dt)))
disp(strcat("Sends frequency: ", num2str(sends/dt)))
disp(strcat("Recives frequency: ", num2str(recvs/dt)))