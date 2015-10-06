Examples of Geomagic YARP Clients
=================================

## iCub Teleoperation

##### Prerequisities
- Geomagic YARP Device Wrapper is running.
- icub/icubSim is running with the Cartesian Controllers available.
- Gaze Controller is running too, but that's optional.

## Command-line Options
- `name` "_port-stem-name_": a string specifying the ports stem-name (`teleop-icub` by default).
- `geomagic` _port-stem-name_: a string specifying the remote driver to connect to.
- `robot` _name_: a string specifying the robot name (`icub` by default).
- `part` _part_: a string specifying the robot part to teleoperate (`right_arm` by default).
- `simulator` _sw_: a string on/off to display the target within the simulator (`off` by default).
- `gaze` _sw_: a string on/off to control the gaze as well (`off` by default).
- `torso` _sw_: a string on/off to enable/disable the control of the torso during the teleoperation (`on` by default).
- `Tp2p` _time_: a number (double) accounting for point-to-point trajectory time expressed in seconds (`1.0 s` by default). Decrease it to go faster.

## How to Teleoperate the robot
