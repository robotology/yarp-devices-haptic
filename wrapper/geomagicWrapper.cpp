// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#include <yarp/os/Log.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/Math.h>

#include "geomagicWrapper.h"
#include "common.h"

#define GEOMAGIC_WRAPPER_DEFAULT_NAME       "geomagic"
#define GEOMAGIC_WRAPPER_DEFAULT_PERIOD     20          // [ms]

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;

using namespace geomagic;

/*********************************************************************/
GeomagicWrapper::GeomagicWrapper() : RateThread(GEOMAGIC_WRAPPER_DEFAULT_PERIOD),
                                     device(NULL), applyFdbck(false), fdbck(3,0.0)
{
}


/*********************************************************************/
GeomagicWrapper::~GeomagicWrapper()
{
    threadRelease();
    device=NULL;
}


/*********************************************************************/
bool GeomagicWrapper::open(Searchable &config)
{
    portStemName=config.check("name",
                              Value(GEOMAGIC_WRAPPER_DEFAULT_NAME)).asString().c_str();
    verbosity=config.check("verbosity",Value(0)).asInt();
    int period=config.check("period",
                            Value(GEOMAGIC_WRAPPER_DEFAULT_PERIOD)).asInt();
    setRate(period);

    if (config.check("subdevice"))
    {
        Property p(config.toString().c_str());
        p.setMonitor(config.getMonitor(),"subdevice");
        p.unput("device");
        p.put("device",config.find("subdevice").asString());

        if (driver.open(p))
        {
            IGeomagic *g;
            driver.view(g);
            attach(g);
        }
        else
        {
            yError("*** Geomagic Wrapper: failed to open the driver!");
            return false;
        }
    }    

    if (verbosity>0)
        yInfo("*** Geomagic Wrapper: opened");

    return true;
}


/*********************************************************************/
bool GeomagicWrapper::close()
{
    if (isRunning())
    {
        askToStop();
        if (verbosity>0)
            yInfo("*** Geomagic Wrapper: stopped");
    }

    detachAll();

    if (driver.isValid())
        driver.close();
    
    if (verbosity>0)
        yInfo("*** Geomagic Wrapper: closed");

    return true;
}


/*********************************************************************/
void GeomagicWrapper::attach(IGeomagic *dev)
{
    device=dev;

    start();
    if (verbosity>0)
        yInfo("*** Geomagic Wrapper: started");
}


/*********************************************************************/
void GeomagicWrapper::detach()
{
    device=NULL;
}


/*********************************************************************/
bool GeomagicWrapper::attachAll(const PolyDriverList &p)
{
    if (p.size()!=1)
    {
        yError("*** Geomagic Wrapper: cannot attach more than one device");
        return false;
    }

    PolyDriver *dev=p[0]->poly;
    if (dev->isValid())
        dev->view(device);

    if (device==NULL)
    {
        yError("*** Geomagic Wrapper: invalid device"); 
        return false;
    }

    start();
    if (verbosity>0)
        yInfo("*** Geomagic Wrapper: started");
    
    return true;
}


/*********************************************************************/
bool GeomagicWrapper::detachAll()
{
    device=NULL;
    return true;
}


/*********************************************************************/
bool GeomagicWrapper::read(ConnectionReader &connection)
{
    Bottle cmd;
    if (!cmd.read(connection))
        return false;
    int tag=cmd.get(0).asVocab();

    Bottle rep;
    if (device!=NULL)
    {
        if (tag==geomagic::set_transformation)
        {
            if (cmd.size()>=2)
            {
                Matrix T;
                cmd.get(1).asList()->write(T);
                if (device->setTransformation(T))
                    rep.addVocab(geomagic::ack);
                else
                    rep.addVocab(geomagic::nack);
            }
        }
        else if (tag==geomagic::get_transformation)
        {
            Matrix T;
            if (device->getTransformation(T))
            {
                rep.addVocab(geomagic::ack);
                rep.addList().read(T);
            }
            else
                rep.addVocab(geomagic::nack);
        }
        else if (tag==geomagic::stop_feedback)
        {
            applyFdbck=false;
            rep.addVocab(geomagic::ack);
        }
        else if (tag==geomagic::is_cartesian)
        {
            bool ret;
            if (device->isCartesianForceModeEnabled(ret))
            {
                rep.addVocab(geomagic::ack);
                rep.addInt(ret?1:0);
            }
            else
                rep.addVocab(geomagic::nack);
        }
        else if (tag==geomagic::set_cartesian)
        {
            rep.addVocab(device->setCartesianForceMode()?
                         geomagic::ack:geomagic::nack);
        }
        else if (tag==geomagic::set_joint)
        {
            rep.addVocab(device->setJointTorqueMode()?
                         geomagic::ack:geomagic::nack);
        }
        else if (tag==geomagic::get_max)
        {
            Vector max;
            if (device->getMaxFeedback(max))
            {
                rep.addVocab(geomagic::ack);
                rep.addList().read(max);
            }
            else
                rep.addVocab(geomagic::nack);
        }
    }

    if (rep.size()==0)
        rep.addVocab(geomagic::nack);

    ConnectionWriter *writer=connection.getWriter();
    if (writer!=NULL)
        rep.write(*writer);

    return true;
}


/*********************************************************************/
bool GeomagicWrapper::threadInit()
{
    statePort.open(("/"+portStemName+"/state:o").c_str());
    feedbackPort.open(("/"+portStemName+"/feedback:i").c_str());
    rpcPort.open(("/"+portStemName+"/rpc").c_str());
    rpcPort.setReader(*this);

    return true;
}


/*********************************************************************/
void GeomagicWrapper::threadRelease()
{
    statePort.interrupt();
    feedbackPort.interrupt();
    rpcPort.interrupt();

    statePort.close();
    feedbackPort.close();
    rpcPort.close();
}


/*********************************************************************/
void GeomagicWrapper::run()
{
    if (device!=NULL)
    {
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

