// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#include <yarp/os/Log.h>
#include <yarp/os/LockGuard.h>
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

using namespace hapticdevice;

/*********************************************************************/
HapticDeviceWrapper::HapticDeviceWrapper() :
                     RateThread(HAPTICDEVICE_WRAPPER_DEFAULT_PERIOD),
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
    verbosity=config.check("verbosity",Value(0)).asInt();
    int period=config.check("period",
                            Value(HAPTICDEVICE_WRAPPER_DEFAULT_PERIOD)).asInt();
    setRate(period);

    if (config.check("subdevice"))
    {
        Property p(config.toString().c_str());
        p.setMonitor(config.getMonitor(),"subdevice");
        p.unput("device");
        p.put("device",config.find("subdevice").asString());

        if (driver.open(p))
        {
            IHapticDevice *d;
            driver.view(d);
            attach(d);
        }
        else
        {
            yError("*** Haptic Device Wrapper: failed to open the driver!");
            return false;
        }
    }    

    if (verbosity>0)
        yInfo("*** Haptic Device Wrapper: opened");

    return true;
}


/*********************************************************************/
bool HapticDeviceWrapper::close()
{
    LockGuard lg(mutex);
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
void HapticDeviceWrapper::attach(IHapticDevice *dev)
{
    device=dev;

    start();
    if (verbosity>0)
        yInfo("*** Haptic Device Wrapper: started");
}


/*********************************************************************/
void HapticDeviceWrapper::detach()
{
    device=NULL;
}


/*********************************************************************/
bool HapticDeviceWrapper::attachAll(const PolyDriverList &p)
{
    if (p.size()!=1)
    {
        yError("*** Haptic Device Wrapper: cannot attach more than one device");
        return false;
    }

    PolyDriver *dev=p[0]->poly;
    if (dev->isValid())
        dev->view(device);

    if (device==NULL)
    {
        yError("*** Haptic Device Wrapper: invalid device"); 
        return false;
    }

    start();
    if (verbosity>0)
        yInfo("*** Haptic Device Wrapper: started");
    
    return true;
}


/*********************************************************************/
bool HapticDeviceWrapper::detachAll()
{
    device=NULL;
    return true;
}


/*********************************************************************/
bool HapticDeviceWrapper::read(ConnectionReader &connection)
{
    Bottle cmd;
    if (!cmd.read(connection))
        return false;
    int tag=cmd.get(0).asVocab();

    Bottle rep;
    if (device!=NULL)
    {
        LockGuard lg(mutex);

        if (tag==hapticdevice::set_transformation)
        {
            if (cmd.size()>=2)
            {
                if (Bottle *payload=cmd.get(1).asList())
                {
                    Matrix T(payload->get(0).asInt(),
                             payload->get(1).asInt());
            
                    if (Bottle *vals=payload->get(2).asList())
                    {
                        for (int r=0; r<T.rows(); r++)
                            for (int c=0; c<T.cols(); c++)
                                T(r,c)=vals->get(T.rows()*r+c).asDouble();
            
                        if (device->setTransformation(T))
                            rep.addVocab(hapticdevice::ack);
                        else
                            rep.addVocab(hapticdevice::nack);
                    }
                }
            }
        }
        else if (tag==hapticdevice::get_transformation)
        {
            Matrix T;
            if (device->getTransformation(T))
            {
                rep.addVocab(hapticdevice::ack);
                rep.addList().read(T);
            }
            else
                rep.addVocab(hapticdevice::nack);
        }
        else if (tag==hapticdevice::stop_feedback)
        {
            fdbck=0.0;
            device->stopFeedback();
            applyFdbck=false;
            rep.addVocab(hapticdevice::ack);
        }
        else if (tag==hapticdevice::is_cartesian)
        {
            bool ret;
            if (device->isCartesianForceModeEnabled(ret))
            {
                rep.addVocab(hapticdevice::ack);
                rep.addInt(ret?1:0);
            }
            else
                rep.addVocab(hapticdevice::nack);
        }
        else if (tag==hapticdevice::set_cartesian)
        {
            rep.addVocab(device->setCartesianForceMode()?
                         hapticdevice::ack:hapticdevice::nack);
        }
        else if (tag==hapticdevice::set_joint)
        {
            rep.addVocab(device->setJointTorqueMode()?
                         hapticdevice::ack:hapticdevice::nack);
        }
        else if (tag==hapticdevice::get_max)
        {
            Vector max;
            if (device->getMaxFeedback(max))
            {               
                rep.addVocab(hapticdevice::ack);
                rep.addList().read(max);                
            }
            else
                rep.addVocab(hapticdevice::nack);
        }
    }

    if (rep.size()==0)
        rep.addVocab(hapticdevice::nack);

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
        LockGuard lg(mutex);

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
                this->fdbck[0]=fdbck->get(0).asDouble();
                this->fdbck[1]=fdbck->get(1).asDouble();
                this->fdbck[2]=fdbck->get(2).asDouble();
            }

            applyFdbck=true;
        }

        if (applyFdbck)
            device->setFeedback(fdbck);
    }
}

