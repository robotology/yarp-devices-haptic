// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#ifndef __GEOMAGIC_CLIENT__
#define __GEOMAGIC_CLIENT__

#include <yarp/os/PortReader.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Mutex.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

#include "IGeomagic.h"

/**
 * Geomagic client
 */
class GeomagicClient : public yarp::dev::DeviceDriver,
                       public yarp::os::PortReader,
                       public yarp::dev::IPreciselyTimed,
                       public geomagic::IGeomagic
{
protected:    
    int verbosity;

    yarp::os::BufferedPort<yarp::os::Bottle> statePort;
    yarp::os::BufferedPort<yarp::os::Bottle> feedbackPort;
    yarp::os::RpcClient                      rpcPort;    
    
    yarp::sig::Vector state;
    yarp::os::Stamp stamp;
    yarp::os::Mutex mutex;

    bool read(yarp::os::ConnectionReader &connection);

public:
    GeomagicClient();
    ~GeomagicClient() { }

    bool open(yarp::os::Searchable &config);
    bool close();

    // IGeomagic Interface
    bool getPosition(yarp::sig::Vector &pos);
    bool getOrientation(yarp::sig::Vector &rpy);
    bool getButtons(yarp::sig::Vector &buttons);
    bool isCartesianForceModeEnabled(bool &ret);
    bool setCartesianForceMode();
    bool setJointTorqueMode();
    bool getMaxFeedback(yarp::sig::Vector &max);
    bool setFeedback(const yarp::sig::Vector &fdbck);
    bool stopFeedback();
    bool getTransformation(yarp::sig::Matrix &T);
    bool setTransformation(const yarp::sig::Matrix &T);

    // IPreciselyTimed Interface
    yarp::os::Stamp getLastInputStamp();
};

#endif


