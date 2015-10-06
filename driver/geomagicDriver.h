// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#ifndef __GEOMAGIC_DRIVER__
#define __GEOMAGIC_DRIVER__

#include <yarp/os/Searchable.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

#include <HD/hd.h>
#include <HL/hl.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>

#include "IGeomagic.h"

/**
 * Data retrieved from HDAPI.
 */
typedef struct
{
    HDboolean m_button1State;      /* Has the device button has been pressed. */
    HDboolean m_button2State;      /* Has the device button has been pressed. */
    hduVector3Dd m_devicePosition; /* Current device coordinates in mm. */
    hduVector3Dd m_gimbalAngles;   /* Gimbal Angles in rad.*/
    bool m_isForce;                /* Force or Torque Mode */
    hduVector3Dd m_forceValues;    /* Current force as Cartesian
                                      coordinated vector in N. 
                                                OR
                                      mNm : milli newton meters torque 
                                      for first 3 joints */
    HDErrorInfo m_error;

} DeviceData;


/**
 * Geomagic driver
 */
class GeomagicDriver : public yarp::dev::DeviceDriver,
                       public geomagic::IGeomagic
{
protected:
    bool configured;
    int verbosity;
    std::string name;
    yarp::sig::Matrix T;

    // Geomagic Touch Device HD library variables
    HHD hHD;
    hduMatrix *hT;
    HDSchedulerHandle hUpdateHandle;
    DeviceData hDeviceData;
    DeviceData innerDeviceData;

    // Geomagic features: num motors and force limits
    int numMotors;
    HDdouble maxForceMagnitude;

    // Get Geomagic Touch position, gimbal and buttons state
    static HDCallbackCode HDCALLBACK updateDeviceCallback(void *);
    // Copy the last device info.
    static HDCallbackCode HDCALLBACK copyDeviceDataCallback(void *);
    // Update motor force data
    static HDCallbackCode HDCALLBACK updateMotorForceDataCallback(void *);

    bool getData();
    bool setData();
    HDdouble sat(HDdouble value,HDdouble max);

public:
    GeomagicDriver();

    // Device Driver
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
};

#endif

