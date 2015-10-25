// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#ifndef __HAPTICDEVICE_CLIENT__
#define __HAPTICDEVICE_CLIENT__

#include <yarp/os/RpcClient.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Mutex.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

#include "IHapticDevice.h"

class StatePort : public yarp::os::BufferedPort<yarp::os::Bottle>
{
    HapticDeviceClient *client;
    void onRead(yarp::os::Bottle &state);

public:
    StatePort() : client(NULL)
    {
        useCallback();
    }

    void setClient(HapticDeviceClient *client_)
    {
        this->client=client_;
    }
};

/**
 * Haptic Device client.
 */
class HapticDeviceClient : public yarp::dev::DeviceDriver,
                           public yarp::dev::IPreciselyTimed,
                           public hapticdevice::IHapticDevice
{
protected:    
    int verbosity;

    friend StatePort;
    StatePort                                statePort;
    yarp::os::BufferedPort<yarp::os::Bottle> feedbackPort;
    yarp::os::RpcClient                      rpcPort;    
    
    yarp::sig::Vector state;
    yarp::os::Stamp stamp;
    yarp::os::Mutex mutex;

public:
    HapticDeviceClient();
    ~HapticDeviceClient() { }

    bool open(yarp::os::Searchable &config);
    bool close();

    // IHapticDevice Interface
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


