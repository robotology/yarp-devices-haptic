Geomagic YARP Device Driver
===========================

## Installation

##### Geomagic Touch Drivers and API
Login at http://developer.geomagic.com, download the drivers and the SDK
for your platform and follow the enclosed instructions.

In _Windows_, the environment variable **`OH_SDK_BASE`** should be set pointing to the location where the driver and the SDK are installed.

In _Linux_, remember to set **`LC_NUMERIC=en_US.UTF-8`** in the environment, prior to pairing the device. This will make the driver work outside US.

##### Dependencies for the YARP device driver and the examples
- [YARP](https://github.com/robotology/yarp)
- [icub-contrib-common](https://github.com/robotology/icub-contrib-common)

## Compiling and Installing the YARP driver
1. Set up the building project by means of **cmake**.
2. Compile and install the project.
3. Set up the environment variable **`geomagic_DIR`** pointing where the project gets installed (should be the same content used for `CMAKE_INSTALL_PREFIX`).
4. Append to the environment variable **`YARP_DATA_DIRS`** the path `$geomagic_DIR/share/geomagic`.

## Running YARP driver
First, check whether the YARP drivers got installed correctly.

Therefore, launch: `yarpdev --list`

and see if `geomagicdriver`, `geomagicwrapper`, `geomagicclient` are listed down.

You can then run the driver in two ways:

1. `yarpdev --device geomagicdriver [option-list]`
2. `robotInterface --context geomagic --config geomagic.xml`

The available options are:
- `device-id` _id_: a string with the name of the Geomagic Touch device that has been paired.
- `name` _port-stem-name_: a string specifying the ports stem-name.
- `period` _period_: an integer that specifies the period in `ms` (`20 ms` by default).
- `verbosity` _level_: an integer accounting for the enabled verbosity level.

In case the `robotInterface` deployer is chosen, then the options are all contained in the corresponding `geomagic.xml` file.

## Interface Documentation

Online documentation is available here: [http://robotology.github.com/geomagic](http://robotology.github.com/geomagic).

## License

Material included here is Copyright of _iCub Facility - Istituto Italiano di
Tecnologia_ and is released under the terms of the GPL v2.0 or later.
See the file LICENSE for details.
