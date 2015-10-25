Examples of YARP Client plugins
=================================

## iCub Teleoperation with Geomagic Haptic Device

##### Building
Cmake the **`teleop-icub`** project.

##### Prerequisities
- Geomagic YARP Device Wrapper is running.
- icub/icubSim is running with the Cartesian Controllers available.
- Gaze Controller is running too, but that's optional.

##### Command-line Options
- `name` "_port-stem-name_": a string specifying the ports stem-name (`teleop-icub` by default).
- `geomagic` _port-stem-name_: a string specifying the remote driver to connect to.
- `robot` _name_: a string specifying the robot name (`icub` by default).
- `part` _part_: a string specifying the robot part to teleoperate (`right_arm` by default).
- `simulator` _sw_: a string on/off to display the target within the simulator (`off` by default).
- `gaze` _sw_: a string on/off to control the gaze as well (`off` by default).
- `torso` _sw_: a string on/off to enable/disable the control of the torso during the teleoperation (`on` by default).
- `Tp2p` _time_: a number (double) accounting for point-to-point trajectory time expressed in seconds (`1.0 s` by default). Decrease it to go faster.
- `min-force-feedback` _val_: a number (double) accounting for the minimum force that can be transmitted as a feedback to the device (`5.0 N` by default).
- `max-force-feedback` _val_: a number (double) accounting for the maximum force that can be transmitted as a feedback to the device (`20.0 N` by default).

The port _/teleop-icub/force-feedback:i_ does accept three numbers to implement the 3D force feedback on the tip of the device.

##### How to Teleoperate the icub/icubSim
The first button is the greyest.

- By pressing the first button once, you will switch the control mode: from "`xyz`" (position only) to "`full`" (position+orientation).
- By pressing the second button once, you will select whether to open or to close the hand.
- By keeping the first button pressed, you will move/rotate the robot hand with respect to the present pose.
- By keeping the second button pressed, you will open/close the robot hand.
- As soon as you release any button, the ongoing teleoperation gets stopped.
