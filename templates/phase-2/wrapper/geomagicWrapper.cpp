// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#include <yarp/os/Log.h>
#include <yarp/os/Vocab.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/Math.h>

#include "geomagicWrapper.h"

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
                                     device(NULL), applyForce(false), force(3,0.0)
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

    statePort.open(("/"+portStemName+"/state:o").c_str());
    forcePort.open(("/"+portStemName+"/force:i").c_str());
    rpcPort.open(("/"+portStemName+"/rpc").c_str());
    rpcPort.setReader(*this);

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
    Bottle in;
    if (!in.read(connection))
        return false;

    Bottle out;
    if (device!=NULL)
    {
        if (in.get(0).asVocab()==Vocab::encode("sett"))
        {
            if (in.size()>=1+4*4)
            {
                Matrix T(4,4);
                for (int r=0; r<T.rows(); r++)
                    for (int c=0; c<T.cols(); c++)
                        T(r,c)=in.get(1+4*r+c).asDouble();

                if (device->setTransformation(T))
                    out.addVocab(Vocab::encode("ack"));
                else
                    out.addVocab(Vocab::encode("nack"));
            }
        }
        else if (in.get(0).asVocab()==Vocab::encode("gett"))
        {
            Matrix T;
            if (device->getTransformation(T))
            {
                out.addVocab(Vocab::encode("ack"));
                for (int r=0; r<T.rows(); r++)
                    for (int c=0; c<T.cols(); c++)
                        out.addDouble(T(r,c));
            }
            else
                out.addVocab(Vocab::encode("nack"));
        }
        else if (in.get(0).asVocab()==Vocab::encode("fon"))
        {
            applyForce=true;
            out.addVocab(Vocab::encode("ack"));
        }
        else if (in.get(0).asVocab()==Vocab::encode("foff"))
        {
            applyForce=false;
            out.addVocab(Vocab::encode("ack"));
        }
    }

    if (out.size()==0)
        out.addVocab(Vocab::encode("nack"));

    ConnectionWriter *writer=connection.getWriter();
    if (writer!=NULL)
        out.write(*writer);

    return true;
}


/*********************************************************************/
bool GeomagicWrapper::threadInit()
{
    return true;
}


/*********************************************************************/
void GeomagicWrapper::threadRelease()
{
    statePort.interrupt();
    forcePort.interrupt();
    rpcPort.interrupt();

    statePort.close();
    forcePort.close();
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

        if (Bottle *force=forcePort.read(false))
        {
            if (force->size()>=3)
            {
                this->force[0]=force->get(0).asDouble();
                this->force[1]=force->get(1).asDouble();
                this->force[2]=force->get(2).asDouble();
            }
        }

        if (applyForce)
            device->setForceFeedback(force);
    }
}


