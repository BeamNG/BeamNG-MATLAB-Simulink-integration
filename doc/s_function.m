%% BeamNG s-function
%
% Let us now look at the Simulink S-Function in some detail. 
% This is the part of Simulink which handles communication with BeamNG and controls 
% execution in Simulink appropriately. 
% The following figure gives an overview of how the S-function has been implemented.
%
% <<pictures/5_The_Simulink_S_Function.png>>
%

%% Inputs and Outputs
%
% The inputs are controlled with a message of fixed format, 
% and the outputs are controlled with another message with a different fixed format. 
% These are both described below in detail.
%
% The input section is split into a core block 
% (containing the core powertrain/vehicle system properties) 
% and a custom block, which can be used by the user to bring any other desired data from BeamNG to Simulink.
%
% The output section contains eight blocks:
%
% The driver controls section has signals for; throttle, brake, clutch, parking brake, and steering.
%
% The body state section has signals for; position, velocity, acceleration, ground speed, roll, 
% pitch, yaw, and altitude.
%
% The status section has signals for; ignition level, gear, fuel, engine load, high beam, low beam, 
% maximum RPM, reverse, RPM, signal L, signal R, and wheel speed.
%
% The wheel sections have signals for each wheel of the vehicle; including angular velocity, 
% wheel speed, braking torque, propulsion torque, friction torque, and downforce.
%
% Finally, the custom section (on the input and output sides) has up to 50 user-defined signals.
%
% The S-function is designed to transfer data between Simulink and BeamNG using these fixed messaged, 
% where every variable always exists at the same position in the message for every send/receive. 
% These signals are contiguous arrays of double-precision numbers.

%% Instructions
%
% The Lua controller must be loaded in order to start the tight coupling. 
% We can do this with the following three steps:
%
% # First, bring up the Lua console debug window with the ' key.
% # The vehicle should then be selected at the bottom-left of this screen 
%   (usually this will be "BeamNG - Current Vehicle").
% # The following command should be typed into the command bar, to load the controller: 
%   |"controller.loadControllerExternal('tech/vehicleSystemsCoupling',
%   'vehicleSystemsCoupling', {})"|
%
% The Simulink process should also be started. 
% If BeamNG is not running, Simulink will block its execution until it receives a message from BeamNG. 
% The reverse is also true; if Simulink is not executing, BeamNG will block execution.
%
% When communication has been established over the UDP send and recieve sockets 
% (after both ends of the communication have start executing), 
% the tight coupling process will commence.