// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#include <mutex>

#include <yarp/os/Log.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/Math.h>

#include "hapticdeviceWrapper.h"
#include "common.h"

#define HAPTICDEVICE_WRAPPER_DEFAULT_NAME       "hapticdevice"
#define HAPTICDEVICE_WRAPPER_DEFAULT_PERIOD     20          // [ms]

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;

/*********************************************************************/
HapticDeviceWrapper::HapticDeviceWrapper() :
                     PeriodicThread(HAPTICDEVICE_WRAPPER_DEFAULT_PERIOD),
                     device(NULL), applyFdbck(false), fdbck(3,0.0)
{
}


/*********************************************************************/
HapticDeviceWrapper::~HapticDeviceWrapper()
{
    threadRelease();
    device=NULL;
}


/*********************************************************************/
bool HapticDeviceWrapper::open(Searchable &config)
{
    portStemName=config.check("name",
                              Value(HAPTICDEVICE_WRAPPER_DEFAULT_NAME)).asString().c_str();
    verbosity=config.check("verbosity",Value(0)).asInt32();
    int period=config.check("period",
                            Value(HAPTICDEVICE_WRAPPER_DEFAULT_PERIOD)).asInt32();
    setPeriod(period);

    if (verbosity>0)
        yInfo("*** Haptic Device Wrapper: opened");

    return true;
}


/*********************************************************************/
bool HapticDeviceWrapper::close()
{
    std::lock_guard lg(mutex);
    if (isRunning())
    {
        askToStop();
        if (verbosity>0)
            yInfo("*** Haptic Device Wrapper: stopped");
    }

    detachAll();

    if (driver.isValid())
        driver.close();

    if (verbosity>0)
        yInfo("*** Haptic Device Wrapper: closed");

    return true;
}


/*********************************************************************/
bool HapticDeviceWrapper::attach(PolyDriver *dev)
{
    if (!dev || !dev->isValid() || !dev->view(device)) 
    {
        yError("Cannot view IHapticDevice");
        return false;
    }

    start();
    if (verbosity>0)
        yInfo("*** Haptic Device Wrapper: started");

    return true;
}


/*********************************************************************/
bool HapticDeviceWrapper::detach()
{
    device=nullptr;
    return true;
}

/*********************************************************************/
bool HapticDeviceWrapper::read(ConnectionReader &connection)
{
    Bottle cmd;
    if (!cmd.read(connection))
        return false;
    int tag=cmd.get(0).asVocab32();

    Bottle rep;
    if (device!=NULL)
    {
        std::lock_guard lg(mutex);

        if (tag==hapticdevice::set_transformation)
        {
            if (cmd.size()>=2)
            {
                if (Bottle *payload=cmd.get(1).asList())
                {
                    Matrix T(payload->get(0).asInt32(),
                             payload->get(1).asInt32());

                    if (Bottle *vals=payload->get(2).asList())
                    {
                        for (int r=0; r<T.rows(); r++)
                            for (int c=0; c<T.cols(); c++)
                                T(r,c)=vals->get(T.rows()*r+c).asFloat64();

                        if (device->setTransformation(T))
                            rep.addVocab32(hapticdevice::ack);
                        else
                            rep.addVocab32(hapticdevice::nack);
                    }
                }
            }
        }
        else if (tag==hapticdevice::get_transformation)
        {
            Matrix T;
            if (device->getTransformation(T))
            {
                rep.addVocab32(hapticdevice::ack);
                rep.addList().read(T);
            }
            else
                rep.addVocab32(hapticdevice::nack);
        }
        else if (tag==hapticdevice::stop_feedback)
        {
            fdbck=0.0;
            device->stopFeedback();
            applyFdbck=false;
            rep.addVocab32(hapticdevice::ack);
        }
        else if (tag==hapticdevice::is_cartesian)
        {
            bool ret;
            if (device->isCartesianForceModeEnabled(ret))
            {
                rep.addVocab32(hapticdevice::ack);
                rep.addInt32(ret?1:0);
            }
            else
                rep.addVocab32(hapticdevice::nack);
        }
        else if (tag==hapticdevice::set_cartesian)
        {
            rep.addVocab32(device->setCartesianForceMode()?
                         hapticdevice::ack:hapticdevice::nack);
        }
        else if (tag==hapticdevice::set_joint)
        {
            rep.addVocab32(device->setJointTorqueMode()?
                         hapticdevice::ack:hapticdevice::nack);
        }
        else if (tag==hapticdevice::get_max)
        {
            Vector max;
            if (device->getMaxFeedback(max))
            {
                rep.addVocab32(hapticdevice::ack);
                rep.addList().read(max);
            }
            else
                rep.addVocab32(hapticdevice::nack);
        }
    }

    if (rep.size()==0)
        rep.addVocab32(hapticdevice::nack);

    ConnectionWriter *writer=connection.getWriter();
    if (writer!=NULL)
        rep.write(*writer);

    return true;
}


/*********************************************************************/
bool HapticDeviceWrapper::threadInit()
{
    statePort.open(("/"+portStemName+"/state:o").c_str());
    feedbackPort.open(("/"+portStemName+"/feedback:i").c_str());
    rpcPort.open(("/"+portStemName+"/rpc").c_str());
    rpcPort.setReader(*this);

    return true;
}


/*********************************************************************/
void HapticDeviceWrapper::threadRelease()
{
    statePort.interrupt();
    feedbackPort.interrupt();
    rpcPort.interrupt();

    statePort.close();
    feedbackPort.close();
    rpcPort.close();
}


/*********************************************************************/
void HapticDeviceWrapper::run()
{
    if (device!=NULL)
    {
        std::lock_guard lg(mutex);

        Vector pos,rpy,buttons;
        device->getPosition(pos);
        device->getOrientation(rpy);
        device->getButtons(buttons);

        Vector output=cat(cat(pos,rpy),buttons);
        statePort.prepare().read(output);

        stamp.update();
        statePort.setEnvelope(stamp);
        statePort.writeStrict();

        if (Bottle *fdbck=feedbackPort.read(false))
        {
            if (fdbck->size()>=3)
            {
                this->fdbck[0]=fdbck->get(0).asFloat64();
                this->fdbck[1]=fdbck->get(1).asFloat64();
                this->fdbck[2]=fdbck->get(2).asFloat64();
            }

            applyFdbck=true;
        }

        if (applyFdbck)
            device->setFeedback(fdbck);
    }
}
