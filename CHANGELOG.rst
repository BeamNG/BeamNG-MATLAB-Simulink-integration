=========
Changelog
=========

Version 1.3.1.1
===============

Minor updates:

- mltbx and prj files with the updated examples
- fix the object_placement example

Version 1.3.1
=============

- Port Update: The used port with BeamNG is now 25252 instead of 64256.
- Updated the code Code is updated with latest BeamNGpy v1.31 and BeamNG.tech v0.34.

Version 1.3.0
=============

- Added Simulink integration using FMI.
- Added Examples with FMI.
- Added CSV parser to the S-function.
- Added the integration with co-simulation tool in BeamNG.tech.
- Updated the code with latest BeamNGpy 1.29 and BeamNG.tech 0.32.

Version 1.2.0
=============

- fix windows send function.
- Added new example
- updated BeamNGpy syntaxes
- updated the code with latest BeamNGpy 1.28 and BeamNG.tech 0.31


Version 1.1.0
=============

- Add linux compatibility
- updated the code with latest BeamNGpy 1.26 and BeamNG.tech 0.28


Version 1.0.0
=============

New BeamNG Matlab and Simulink bridge.

- Added a Simulink subsystem that wraps the S-function returning busses instead of arrays as outputs.
- Changed the Simulink S-function that now takes as parameters the UDP ports and addresses.
- Added the Matlab function setup_beamngpy that allows to connect easily any python environment with BeamNGpy installed to Matlab.
- Added the Matlab function load_lua_controller to load the vehicleSystemCoupling.lua controller directly from Matlab through BeamNGpy.
- We have developed five fundamental scripts that showcase the versatility of using the MATLAB bridge
  to launch the Beamng.Tech simulator:
  - annotation_bounding_boxes.m
  - lidar_tour.m
  - lidar_tour.m
  - multishot_camera.m
  - object_placement.m
  - vehicle_state_plotting.m
- Basic prototype UDP tight-coupling system for Simulink model with the BeamNG simulator.
  This prototype solution demonstrates typical communication between both ends of the system.
