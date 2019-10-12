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
#include <yarp/dev/IHapticDevice.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

#include <HD/hd.h>
#include <HL/hl.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>

#include <atomic>
#include <mutex>
#include <thread>

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
 * Data sent to the HDAPI.
 */
typedef struct {
    bool m_isForce;                /* Force or Torque Mode */
    hduVector3Dd m_forceValues;
} SentToDeviceData;


/**
 * Geomagic driver
 */
class GeomagicDriver : public yarp::dev::DeviceDriver,
                       public yarp::dev::IHapticDevice
{
protected:
    bool configured;
    int verbosity;
    std::string name;
    yarp::sig::Matrix T;

    // True if the close method has been called at least once
    std::atomic<bool> isDeviceClosing{false};

    // Thread for blocking data exchange with the Geomagic device
    std::thread dataExchangeThread;

    // Geomagic Touch Device HD library variables
    HHD hHD;
    HDSchedulerHandle hUpdateHandle;
    DeviceData hDeviceDataSensor;
    std::mutex hDeviceDataSensorMutex;
    std::mutex hDeviceDataForceMutex;
    SentToDeviceData hDeviceDataForce;
    DeviceData innerDeviceData;
    // False if there was an error in reading from Geomagic
    std::atomic<bool> readSuccessful{false};
    // False if there was an error in writing to the Geomagic
    std::atomic<bool> writeSuccessful{false};

    // Geomagic features: num motors and force limits
    int numMotors;
    HDdouble maxForceMagnitude;

    // Get Geomagic Touch position, gimbal and buttons state
    static HDCallbackCode HDCALLBACK updateDeviceCallback(void *);
    // Copy the last device info.
    static HDCallbackCode HDCALLBACK copyDeviceDataCallback(void *);
    // Update motor force data
    static HDCallbackCode HDCALLBACK updateMotorForceDataCallback(void *);

    HDdouble sat(HDdouble value,HDdouble max);

    // Functions  for blocking data exchange with the Geomagic
    void dataExchangeLoop();
    void getData();
    void setData();
public:
    GeomagicDriver();

    // Device Driver
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
};

#endif

