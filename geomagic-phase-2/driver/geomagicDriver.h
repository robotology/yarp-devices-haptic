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

public:
    GeomagicDriver();

    // Device Driver
    bool open(yarp::os::Searchable &config);
    bool close();

    // IGeomagic Interface
    bool getPosition(yarp::sig::Vector &pos);
    bool getOrientation(yarp::sig::Vector &rpy);
    bool getButtons(yarp::sig::Vector &buttons);

    // There are 2 possibilities to control forces on motors:
    // - set the current force as Cartesian coordinated vector.
    //   This is the primary method for sending forces to the device.
    //   Setting the current force causes that force to be commanded at end
    //   of the frame. In this case, the 3 element double vector expresses
    //   the force in Newton. The max positive and negative value for
    //   the forces are returned by 'getMaxForceFeedback' function.
    // - set the current torque as a joint coordinated vector for
    //   the first three joints of the Touch device.
    //   Setting the current joint torque causes that torque to be commanded
    //   at end of the frame. In this case, the 3 element double vector
    //   expresses the force in milliNewton for meter. The max force value
    //   is returned from 'getMaxForceFeedback' function.
    bool isCartesianForceModeEnabled();
    bool getMaxForceFeedback(yarp::sig::Vector &force);
    void setJointTorqueMode();
    void setCartesianForceMode();
    bool setForceFeedback(const yarp::sig::Vector &force);

    bool getTransformation(yarp::sig::Matrix &T);
    bool setTransformation(const yarp::sig::Matrix &T);

private:
    bool getData();
    bool setData();
    HDdouble min(HDdouble value, HDdouble max);
};

#endif

