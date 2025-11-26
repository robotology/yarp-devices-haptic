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
#include <mutex>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Stamp.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/IHapticDevice.h>
#include <yarp/sig/Vector.h>

/**
 * Haptic Device wrapper
 */
class HapticDeviceWrapper : public yarp::dev::DeviceDriver,
                            public yarp::dev::WrapperSingle,
                            public yarp::os::PeriodicThread,
                            public yarp::os::PortReader
{
protected:
    std::string portStemName;
    int verbosity;

    yarp::os::BufferedPort<yarp::os::Bottle> statePort;
    yarp::os::BufferedPort<yarp::os::Bottle> feedbackPort;
    yarp::os::RpcServer                      rpcPort;

    std::mutex mutex;
    yarp::os::Stamp stamp;

    yarp::dev::PolyDriver driver;
    yarp::dev::IHapticDevice *device;

    yarp::sig::Vector fdbck;
    bool applyFdbck;

    bool read(yarp::os::ConnectionReader &connection) override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

public:
    HapticDeviceWrapper();
    ~HapticDeviceWrapper() override;

    bool open(yarp::os::Searchable &config) override;
    bool close() override;

    bool attach(yarp::dev::PolyDriver *dev) override;
    bool detach() override;
};

#endif
