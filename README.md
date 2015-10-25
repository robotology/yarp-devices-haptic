Generic YARP Driver for Haptic Devices
======================================

## Installation of Physical Drivers

##### Geomagic Touch Drivers and API
Login at http://developer.geomagic.com, download the drivers and the SDK
for your platform and follow the enclosed instructions.

In _Windows_, the environment variable **`OH_SDK_BASE`** should be set by the installer
pointing to the location where the driver and the SDK are installed.

In _Linux_, remember to set **`LC_NUMERIC=en_US.UTF-8`** in the environment,
prior to pairing the device. This will make the driver work outside US.

##### Dependencies for the YARP device driver and the examples
- [YARP](https://github.com/robotology/yarp)
- [icub-contrib-common](https://github.com/robotology/icub-contrib-common) (only for examples and tests)

## Compiling and Installing the YARP plugins
1. Set up the building project by means of **cmake**. Remember to tick on the drivers:
`hapticdevicewrapper`, `hapticdeviceclient` and the physical drivers you need, e.g. `geomagicdriver`.
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
2. `robotInterface --context geomagic --config geomagic.xml`

The available options are:
- `device-id` "_id_": a string with the name of the physical device that has been instantiated.
- `name` "_port-stem-name_": a string specifying the ports stem-name (`hapticdevice` by default).
- `period` _period_: an integer that specifies the period in `ms` (`20 ms` by default).
- `verbosity` _level_: an integer accounting for the enabled verbosity level (`0` by default).

In case the `robotInterface` deployer is chosen, then the options are all contained in the corresponding
`xml` files that are installed in `$hapticdevice_DIR/share/hapticdevice/context` path and possibly
customized using the `yarp-config` tool.

## Connecting to the YARP driver
A YARP module that wants to connect to an haptic device needs to contain the following instructions:

CMAKE directives:
```cmake
find_package(hapticdevice REQUIRED)
include_directories(${hapticdevice_INCLUDE_DIRS})
```

C++ code:
```cpp
#include <hapticdevice/IHapticDevice.h>

Property option("(device hapticdeviceclient)");
option.put("remote","/hapticdevice");   // or whatever wrapper stem-name
option.put("local","/local-port");  // any local ports stem-name

PolyDriver driver;
driver.open(option);

hapticdevice::IHapticDevice *ihap;
driver.view(ihap);
```

The **IHapticDevice** YARP interface is documented here: [**`http://robotology.github.com/haptic-devices`**](http://robotology.github.com/haptic-devices).

## Client Examples
Examples of modules connecting to the YARP driver can be found
[**`here`**](https://github.com/robotology/haptic-devices/tree/master/examples).

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
