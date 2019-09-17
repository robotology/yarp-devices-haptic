Generic YARP Driver for Haptic Devices
======================================

## Installation of Physical Drivers

##### Geomagic Touch Drivers and API
Download the Geomagic Touch drivers and the SDK
for your platform following the links at https://3dssupport.microsoftcrmportals.com/knowledgebase/article/KA-01405/en-us and follow the enclosed instructions.

In _Windows_, the environment variable **`OH_SDK_BASE`** should be set by the installer
pointing to the location where the driver and the SDK are installed.

In _Linux_, remember to set **`LC_NUMERIC=en_US.UTF-8`** in the environment,
prior to pairing the device. This will make the driver work outside US.

##### Dependencies for the YARP device driver and the examples
- [YARP](https://github.com/robotology/yarp)
- [icub-contrib-common](https://github.com/robotology/icub-contrib-common) (only for examples and tests)

## Compiling and Installing the YARP plugins
1. Set up the building project by means of **cmake**. If you need to compile some physical drivers you need, remember
to enable the relative CMake variable, e.g. `ENABLE_geomagicdriver`.
2. Compile and install the project.
3. Set up the environment variable **`hapticdevice_DIR`** pointing where the project gets installed
(should be the same path used for `CMAKE_INSTALL_PREFIX`).
4. Append to the environment variable **`YARP_DATA_DIRS`** the path `$hapticdevice_DIR/share/hapticdevice`.

## Running the YARP plugins
First, check whether the YARP drivers got installed correctly.

Therefore, launch: `yarpdev --list`

and see if `hapticdevicewrapper`, `hapticdeviceclient`, `geomagicdriver` are listed down.

You can then run the driver in two ways. For example, for the `geomagicdriver` it holds:

1. `yarpdev --device geomagicdriver [option-list]`
2. `yarprobotinterface --context geomagic --config geomagic.xml`

The available options are:
- `device-id` "_id_": a string with the name of the physical device that has been instantiated.
- `name` "_port-stem-name_": a string specifying the ports stem-name (`hapticdevice` by default).
- `period` _period_: an integer that specifies the period in `ms` (`20 ms` by default).
- `verbosity` _level_: an integer accounting for the enabled verbosity level (`0` by default).

In case the `yarprobotinterface` deployer is chosen, then the options are all contained in the corresponding
`xml` files that are installed in `$hapticdevice_DIR/share/hapticdevice/context` path and possibly
customized using the `yarp-config` tool.

## Connecting to the YARP driver
A YARP module that wants to connect to an haptic device needs to contain the following instructions:

##### C++ code
```cpp
#include <yarp/os/Property.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IHapticDevice.h>

yarp::os::Property option;
option.put("device","hapticdeviceclient");  // device name
option.put("remote","/hapticdevice");       // or whatever wrapper stem-name
option.put("local","/local-port");          // any local ports stem-name

yarp::dev::PolyDriver driver;
driver.open(option);

yarp::dev::IHapticDevice *ihap;
driver.view(ihap);
```

Read [YARP documentation](http://www.yarp.it/index.html) to find out more about [**IHapticDevice**](http://www.yarp.it/classyarp_1_1dev_1_1IHapticDevice.html) interface.

## [Client Examples](/examples)

## [Guidelines for contributing](/.github/CONTRIBUTING.md)

## Authors
- [`Ugo Pattacini`](https://github.com/pattacini):
  - `hapticdevicewrapper`
  - `hapticdeviceclient`
- [`Manuelito Scola`](https://github.com/manuelitoscola):
  - `geomagicdriver`

## License

Material included here is Copyright of _iCub Facility - Istituto Italiano di
Tecnologia_ and is released under the terms of the GPL v2.0 or later.
See the file LICENSE for details.

