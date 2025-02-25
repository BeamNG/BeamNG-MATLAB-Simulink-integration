# S-function Examples

We provide Simulink examples to demonstrate the BeamNG-Simulink co-simulation in action. To execute these examples effectively, it is crucial to configure the three control parameters described in this document: window width, send wait, and send offset. The examples are available in the models named *throttle_example.slx* and *torques_example.slx*, detailed below:

![Figure 1: The controller used in both examples](../../media/m_function.png)
    Figure 1: The controller used in both examples


Copy the examples folder to your current working directory, by running the following in the MATLAB command window:

```
beamng_copy_examples
```

Both examples are located in `examples/simulink/S-function`. Change your working directory to this folder.

## Example 1: Pedal Control to Maintain Speed Limit

The example *throttle_example.slx* maintains the vehicle's speed limit and orientation angle on the map. The block receives vehicle orientation information (yaw angle) and speed as input and controls the vehicle via throttle and steering angle. The speed limit can be adjusted using the `speed_input` constant block, and the orientation angle with the `Desired_steering_angle_input` constant block to the controller as shown in Figure 1.

![Figure 2: The controller function of the Simulink model](../../media/throttle.png)
    Figure 2: Throttle example

## Example 2: Wheel Torques to Maintain Speed Limit

The example *torques_example.slx* also maintains the vehicle's speed limit and orientation angle on the map. In this case, the vehicle is controlled through torques applied to each wheel. Adjustments to the speed limit and orientation angle are made in the same way as in the throttle example.


![Figure 3: The controller function of the Simulink model](../../media/torques.png)
    Figure 3: Torques example
