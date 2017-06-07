// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#ifndef __HAPTICDEVICE_WRAPPER__
#define __HAPTICDEVICE_WRAPPER__

#include <string>

#include <yarp/os/RateThread.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Mutex.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/IHapticDevice.h>
#include <yarp/sig/Vector.h>

/**
 * Haptic Device wrapper
 */
class HapticDeviceWrapper : public yarp::dev::DeviceDriver,
                            public yarp::dev::IMultipleWrapper,
                            public yarp::os::RateThread,
                            public yarp::os::PortReader
{
protected:
    std::string portStemName;
    int verbosity;

    yarp::os::BufferedPort<yarp::os::Bottle> statePort;
    yarp::os::BufferedPort<yarp::os::Bottle> feedbackPort;
    yarp::os::RpcServer                      rpcPort;
    
    yarp::os::Mutex mutex;
    yarp::os::Stamp stamp;

    yarp::dev::PolyDriver driver;
    yarp::dev::IHapticDevice *device;

    yarp::sig::Vector fdbck;
    bool applyFdbck;

    bool read(yarp::os::ConnectionReader &connection);
    bool threadInit();
    void threadRelease();
    void run();

public:
    HapticDeviceWrapper();
    ~HapticDeviceWrapper();

    bool open(yarp::os::Searchable &config);
    bool close();

    void attach(yarp::dev::IHapticDevice *dev);
    void detach();

    bool attachAll(const yarp::dev::PolyDriverList &p);
    bool detachAll();
};

#endif


