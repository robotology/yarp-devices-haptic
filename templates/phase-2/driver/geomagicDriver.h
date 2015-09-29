// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#ifndef __GEOMAGIC_DRIVER__
#define __GEOMAGIC_DRIVERL__

#include <yarp/os/Searchable.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

#include "IGeomagic.h"

/**
 * Geomagic driver
 */
class GeomagicDriver : public yarp::dev::DeviceDriver,
                       public IGeomagic
{
protected:
    bool configured;
    int verbosity;
    yarp::sig::Matrix T;

public:
    GeomagicDriver();

    // Device Driver
    bool open(yarp::os::Searchable &config);
    bool close();

    // IGeomagic Interface
    bool getPosition(yarp::sig::Vector &pos);
    bool getOrientation(yarp::sig::Vector &rpy);
    bool getButtons(yarp::sig::Vector &buttons);
    bool setForceFeedback(const yarp::sig::Vector &force);
    bool setTransformation(const yarp::sig::Matrix &T);
    bool getTransformation(yarp::sig::Matrix &T);
};

#endif

