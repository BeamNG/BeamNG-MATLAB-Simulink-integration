function [throttle,torque,steering] = controller(...
    speed_desired,speed,yaw_angle_rad,...
    steering_control,yaw_angle_desired)
% Controller for S-function examples.

% Steering
yaw_clearance = 1; % in degrees
yaw_angle = yaw_angle_rad*(180/pi); % rad to degree
if yaw_angle <= (yaw_angle_desired-yaw_clearance)
    steering = -steering_control;
elseif yaw_angle > (yaw_angle_desired)
    steering = steering_control;
else
    steering = 0;
end

% Throttle and Torque
speed_clearance=10;
actual_speed=speed*3.6;
if actual_speed <= 5
    torque = 270 ;
    throttle = 0.5;
elseif actual_speed <= (speed_desired-speed_clearance)
    throttle = 0.5;
    torque = 200 ;
elseif  actual_speed > (speed_desired)
    throttle = -0.05;
    torque = 65;
else
    throttle = 0;
    torque = 170;
end
end
