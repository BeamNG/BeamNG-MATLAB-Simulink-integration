# Usage guide

In order to run the examples in this folder the wollowing steps must be followed:
1. Change the path to the `BeamNG.tech` executable in the `start_beamng.m` file at line 7.
2. Run the script `start_beamng.m` and wait until BeamNG is fully loaded.
3. Open the Simulink model `beamng_simulink_interface_demos.slx`.
4. Run one of the `demo_X.m` files.

Each `demo_X.m` run a different simulation:
- `demo_0.m` run a simulation where the car is automatically driven by the BeamNG AI and Simulink is used only to display some data. The simulation can run infinitely and can be stopped pressing any key in the Matlab Command Window.
- `demo_1.m` run a simulation where Simulink control the car using the driver inputs (throttle, brake and steering) to perform a double lane change. The simulation stops automatically after performing the maneuver. 
- `demo_2.m` run a simulation where Simulink control the car using the driver inputs (throttle, brake and steering) to perform a braking test. The simulation stops automatically after performing the maneuver. 
- `demo_3.m` run a simulation where the car can be controlled manually from BeamNG and Simulink decides the amount of torques to apply on each wheel. The simulation can run infinitely and can be stopped pressing any key in the Matlab Command Window.