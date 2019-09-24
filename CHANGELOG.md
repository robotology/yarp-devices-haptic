# Changelog
All notable changes to this project will be documented in this file.

The format of this document is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]
The minimum version of YARP required to use `haptic-devices` is now 3.2 .

### Changed
- In `geomagicdriver`, the `get` and `set` methods are not blocking anymore (see https://github.com/robotology/haptic-devices/issues/10 and https://github.com/robotology/haptic-devices/pull/11).
- Compilation of `hapticdevicewrapper` and `hapticdeviceclient` is now ON by default.
- CMake options for compilation of devices changed from `ENABLE_hapticdevicemod_<devicename>` to `ENABLE_<devicename>`.

### Removed
- The compilation of the custom `hapticdevicemod` executable to launch `haptic-devices`'s YARP devices has been removed. The devices can be launched using `yarpdev` or `yarprobotinterface` deployers.

## [1.0.0] - 2017-06-22
First release of haptic-devices, containing the `geomagicdriver`, `hapticdevicewrapper` and `hapticdeviceclient`. 
This release is compatible with YARP from 2.3.70 to 3.2 .
