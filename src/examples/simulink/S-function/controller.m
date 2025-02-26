function [throttle,yaw_angle,torque,steering] = controller(throttle,...
    speed_threshold,speed_sim,rad_angle,...
    throttle_input_for_steering_controller,steering_threshold,steering)
% Controller for S-function examples.

% Steering
steering_clearance=1; % in degrees
% here's converting the angle from radiant to degree
yaw_angle=rad_angle*(180/pi);
if yaw_angle <= (steering_threshold-steering_clearance)
    steering=-throttle_input_for_steering_controller;
end

if  yaw_angle > (steering_threshold)
    steering=throttle_input_for_steering_controller;
end

% Throttle and Torque
speed_clearance=10;
actual_speed=speed_sim*3.6;
if actual_speed <= 5
    torque = 270 ;
    throttle=0.5;
elseif actual_speed <= (speed_threshold-speed_clearance)
    throttle=0.5;
    torque = 200 ;
elseif  actual_speed > (speed_threshold)
    throttle=-0.05;
    torque = 65;
else
    torque = 170;
end
end
