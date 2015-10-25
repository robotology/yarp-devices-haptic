// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#include <yarp/os/LogStream.h>
#include <yarp/math/Math.h>

#include "geomagicDriver.h"

#define GEOMAGIC_DRIVER_DEFAULT_NAME    "Default Device"
#define MAX_JOINT_TORQUE_0              350.0
#define MAX_JOINT_TORQUE_1              350.0
#define MAX_JOINT_TORQUE_2              350.0

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;



/*********************************************************************/
GeomagicDriver::GeomagicDriver() : configured(false), verbosity(0),
                                   name(GEOMAGIC_DRIVER_DEFAULT_NAME),
                                   T(eye(4,4))
{
}


/*********************************************************************/
bool GeomagicDriver::open(Searchable &config)
{
    HDErrorInfo error;

    if (!configured)
    {
        verbosity=config.check("verbosity",Value(0)).asInt();
        if (verbosity>0)
            yInfo("*** Geomagic Driver: opened");
        name=config.check("device-id",
                          Value(GEOMAGIC_DRIVER_DEFAULT_NAME)).asString();
        if (verbosity>0)
            yInfo("*** Geomagic Driver: name: %s", name.c_str());

        // Initialize the device,
        // must be done before attempting to call any hd function.
        hHD = hdInitDevice((HDstring)name.c_str());
        if (HD_DEVICE_ERROR(error = hdGetError())) {
            yError("*** Geomagic Driver: failed to initialize the device (%s)",
                   hdGetErrorString(error.errorCode));
            return false;
        }

        // Get the number of output degrees of freedom, i.e.
        // the number of independent actuation variable.
        // For Touch devices 3DOF means XYZ linear force
        // output whereas 6DOF means xyz linear forces
        // and roll, pitch, yaw, torques about gimbal.
        hdGetIntegerv(HD_OUTPUT_DOF, &numMotors);

        // Get the nominal maximum force, i.e. the amount
        // of force that the device can sustain when the
        // motors are at room temperature (optimal).
        hdGetDoublev(HD_NOMINAL_MAX_FORCE, &maxForceMagnitude);

        // Get the maximum workspace dimensions of the
        // device, i.e. the maximum mechanical limits of
        // the device, as (minX, minY, minZ, maxX, maxY, maxZ).
        HDdouble dimensions[6];
        hdGetDoublev(HD_MAX_WORKSPACE_DIMENSIONS, dimensions);
        if (verbosity>0)
            yInfo("*** Geomagic Driver: Max Workspace Dimensions "
                  "minX:%lf,minY:%lf,minZ:%lf,"
                  "maxX:%lf,maxY:%lf,maxZ:%lf",
                  dimensions[0], dimensions[1],
                  dimensions[2], dimensions[3],
                  dimensions[4], dimensions[5]);

        // Get the usable workspace dimensions of the
        // device, i.e. the practical limits for the device, as
        // (minX, minY, minZ, maxX, maxY, maxZ). It is
        // guaranteed that forces can be reliably rendered
        // within the usable workspace dimensions.
        hdGetDoublev(HD_USABLE_WORKSPACE_DIMENSIONS, dimensions);
        if (verbosity>0)
            yInfo("*** Geomagic Driver: Usable Workspace Dimensions "
                  "minX:%lf,minY:%lf,minZ:%lf,"
                  "maxX:%lf,maxY:%lf,maxZ:%lf",
                  dimensions[0], dimensions[1],
                  dimensions[2], dimensions[3],
                  dimensions[4], dimensions[5]);

        // Schedule the main scheduler callback that updates the device state.
        hUpdateHandle = hdScheduleAsynchronous(updateDeviceCallback, this,
                                               HD_MAX_SCHEDULER_PRIORITY);
        if (HD_DEVICE_ERROR(error = hdGetError())) {
            hdDisableDevice(hHD);
            yError("*** Geomagic Driver: failed to create scheduler (%s)",
                   hdGetErrorString(error.errorCode));
            return false;
        }

        hdEnable(HD_FORCE_OUTPUT);
        hDeviceData.m_isForce = true;
        if (verbosity>0)
            yInfo("*** Geomagic Driver: Cartesian Force mode enabled");

        // Start the servo loop scheduler.
        hdStartScheduler();
        if (HD_DEVICE_ERROR(error = hdGetError())) {
            hdStopScheduler();
            hdUnschedule(hUpdateHandle);
            hdDisableDevice(hHD);
            yError("*** Geomagic Driver: failed to start scheduler (%s)",
                   hdGetErrorString(error.errorCode));
            return false;
        }

        configured=true;
        return true;
    }
    else
    {
        yError("*** Geomagic Driver: device already opened!");
        return false;
    }
}


/*********************************************************************/
bool GeomagicDriver::close()
{
    if (configured)
    {
        configured=false;

        hdStopScheduler();
        hdUnschedule(hUpdateHandle);
        hdDisableDevice(hHD);

        if (verbosity>0)
            yInfo("*** Geomagic Driver: closed");
        return true;
    }
    else
    {
        yError("*** Geomagic Driver: trying to close a device which was not opened!");
        return false;
    }
}


/*********************************************************************/
bool GeomagicDriver::getPosition(Vector &pos)
{
    if (!getData())
        return false;

    pos.resize(4);
    pos[0]=0.001*hDeviceData.m_devicePosition[0];
    pos[1]=0.001*hDeviceData.m_devicePosition[1];
    pos[2]=0.001*hDeviceData.m_devicePosition[2];
    pos[3]=1.0;

    pos=T*pos;
    pos.pop_back();

    return true;
}


/*********************************************************************/
bool GeomagicDriver::getOrientation(Vector &rpy)
{
    if (!getData())
        return false;

    rpy.resize(3);
    rpy[0]=hDeviceData.m_gimbalAngles[0];
    rpy[1]=hDeviceData.m_gimbalAngles[1];
    rpy[2]=hDeviceData.m_gimbalAngles[2];

    return true;
}


/*********************************************************************/
bool GeomagicDriver::getButtons(Vector &buttons)
{
    if (!getData())
        return false;

    buttons.resize(2);
    buttons[0]=hDeviceData.m_button1State;
    buttons[1]=hDeviceData.m_button2State;

    return true;
}


/*********************************************************************/
bool GeomagicDriver::isCartesianForceModeEnabled(bool &ret)
{
    ret=hDeviceData.m_isForce;
    return true;
}


/*********************************************************************/
bool GeomagicDriver::setCartesianForceMode()
{
    if (verbosity>0)
        yInfo("*** Geomagic Driver: Cartesian Force mode enabled");
    hDeviceData.m_isForce=true;
    return true;
}


/*********************************************************************/
bool GeomagicDriver::setJointTorqueMode()
{
    if (verbosity>0)
        yInfo("*** Geomagic Driver: Joint Torque mode enabled");
    hDeviceData.m_isForce=false;
    return true;
}


/*********************************************************************/
bool GeomagicDriver::getMaxFeedback(Vector &max)
{
    max.resize(3);
    if (hDeviceData.m_isForce) {
        max=maxForceMagnitude;
    }
    else {
        max[0]=MAX_JOINT_TORQUE_0;
        max[1]=MAX_JOINT_TORQUE_1;
        max[2]=MAX_JOINT_TORQUE_2;
    }
    return true;
}


/*********************************************************************/
bool GeomagicDriver::setFeedback(const Vector &fdbck)
{
    if (fdbck.size()<3)
        return false;

    if (hDeviceData.m_isForce) {
        Vector fdbck_=fdbck;
        fdbck_.push_back(1.0);
        fdbck_=SE3inv(T)*fdbck_;

        hDeviceData.m_forceValues[0]=this->sat(fdbck_[0],maxForceMagnitude);
        hDeviceData.m_forceValues[1]=this->sat(fdbck_[1],maxForceMagnitude);
        hDeviceData.m_forceValues[2]=this->sat(fdbck_[2],maxForceMagnitude);
    }
    else {
        hDeviceData.m_forceValues[0]=this->sat(fdbck[0],MAX_JOINT_TORQUE_0);
        hDeviceData.m_forceValues[1]=this->sat(fdbck[1],MAX_JOINT_TORQUE_1);
        hDeviceData.m_forceValues[2]=this->sat(fdbck[2],MAX_JOINT_TORQUE_2);
    }
    if (!setData())
        return false;
    return true;
}


/*********************************************************************/
bool GeomagicDriver::stopFeedback()
{
    hDeviceData.m_forceValues[0]=0.0;
    hDeviceData.m_forceValues[1]=0.0;
    hDeviceData.m_forceValues[2]=0.0;
    
    if (!setData())
        return false;
    
    return true;
}


/*********************************************************************/
bool GeomagicDriver::setTransformation(const Matrix &T)
{
    if ((T.rows()<this->T.rows()) || (T.cols()<this->T.cols()))
    {
        yError("*** Geomagic Driver: requested to use the unsuitable transformation matrix %s",
               T.toString(5,5).c_str());
        return false;
    }

    this->T=T.submatrix(0,this->T.rows()-1,0,this->T.cols()-1);
    if (verbosity>0)
        yInfo("*** Geomagic Driver: transformation matrix set to %s",
              this->T.toString(5,5).c_str());

    return true;
}


/*********************************************************************/
bool GeomagicDriver::getTransformation(Matrix &T)
{
    T=this->T;
    return true;
}


/*********************************************************************/
bool GeomagicDriver::getData()
{
    HDErrorInfo error;

    // Perform a synchronous call to copy the most current device state.
    // This synchronous scheduler call ensures that the device state
    // is obtained in a thread-safe manner.
    hdScheduleSynchronous(copyDeviceDataCallback, this,
                          HD_MIN_SCHEDULER_PRIORITY);
    if (HD_DEVICE_ERROR(error = hdGetError())) {
        yError("*** Geomagic Driver: failed to copy device data (%s)",
               hdGetErrorString(error.errorCode));
        return false;
    }

    return true;
}


/*********************************************************************/
bool GeomagicDriver::setData()
{
    HDErrorInfo error;

    // Perform a synchronous call to copy the motor data force.
    // This synchronous scheduler call ensures that the device update
    // is done in a thread-safe manner.
    hdScheduleSynchronous(updateMotorForceDataCallback, this,
                          HD_MIN_SCHEDULER_PRIORITY);
    if (HD_DEVICE_ERROR(error = hdGetError())) {
        yError("*** Geomagic Driver: failed to update force device data (%s)",
               hdGetErrorString(error.errorCode));
        return false;
    }

    return true;
}


/*********************************************************************/
HDdouble GeomagicDriver::sat(HDdouble value, HDdouble max)
{
    if (value>max)
        value=max;
    else if (value<-max)
        value=-max;
    return value;
}


/*********************************************************************/
HDCallbackCode HDCALLBACK
GeomagicDriver::updateDeviceCallback(void *pUserData)
{
    int nButtons = 0;
    GeomagicDriver *pThis = static_cast<GeomagicDriver *>(pUserData);
    DeviceData *pDeviceData = (DeviceData *)&(pThis->innerDeviceData);

    hdBeginFrame(hdGetCurrentDevice());

    /* Retrieve the current button(s). */
    hdGetIntegerv(HD_CURRENT_BUTTONS, &nButtons);
    
    /* In order to get the specific button 1 state, we use a bitmask to
       test for the HD_DEVICE_BUTTON_1 bit. */
    pDeviceData->m_button1State = 
        (nButtons & HD_DEVICE_BUTTON_1) ? HD_TRUE : HD_FALSE;
 
    /* In order to get the specific button 2 state, we use a bitmask to
       test for the HD_DEVICE_BUTTON_2 bit. */
    pDeviceData->m_button2State = 
        (nButtons & HD_DEVICE_BUTTON_2) ? HD_TRUE : HD_FALSE;
 
    /* Get the current location of the device (HD_GET_CURRENT_POSITION)
       We declare a vector of three doubles since hdGetDoublev returns 
       the information in a vector of size 3. */
    hdGetDoublev(HD_CURRENT_POSITION, pDeviceData->m_devicePosition);

    /* Get the angles of the device gimbal. For Touch
       devices: From Neutral position Right is +, Up is -,
       CW is + . */
    hdGetDoublev(HD_CURRENT_GIMBAL_ANGLES, pDeviceData->m_gimbalAngles);

    if (pDeviceData->m_isForce)
        hdSetDoublev(HD_CURRENT_FORCE, pDeviceData->m_forceValues);
    else
        hdSetDoublev(HD_CURRENT_JOINT_TORQUE, pDeviceData->m_forceValues);

    /* Also check the error state of HDAPI. */
    pDeviceData->m_error = hdGetError();

    /* Copy the position into our device_data tructure. */
    hdEndFrame(hdGetCurrentDevice());

    return HD_CALLBACK_CONTINUE;    
}


/*********************************************************************/
HDCallbackCode HDCALLBACK
GeomagicDriver::copyDeviceDataCallback(void *pUserData)
{
    GeomagicDriver *pThis = static_cast<GeomagicDriver *>(pUserData);
    memcpy(&pThis->hDeviceData, &pThis->innerDeviceData, sizeof(DeviceData));
    return HD_CALLBACK_DONE;
}


/*********************************************************************/
HDCallbackCode HDCALLBACK
GeomagicDriver::updateMotorForceDataCallback(void *pUserData)
{
    GeomagicDriver *pThis = static_cast<GeomagicDriver *>(pUserData);
    pThis->innerDeviceData.m_isForce=pThis->hDeviceData.m_isForce;
    pThis->innerDeviceData.m_forceValues[0]=pThis->hDeviceData.m_forceValues[0];
    pThis->innerDeviceData.m_forceValues[1]=pThis->hDeviceData.m_forceValues[1];
    pThis->innerDeviceData.m_forceValues[2]=pThis->hDeviceData.m_forceValues[2];
    return HD_CALLBACK_DONE;
}

