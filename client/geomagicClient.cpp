// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#include <string>

#include <yarp/os/Log.h>
#include <yarp/os/Network.h>
#include <yarp/os/LockGuard.h>

#include "geomagicClient.h"
#include "common.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

using namespace geomagic;


/*********************************************************************/
void StatePort::onRead(Bottle &state)
{
    if (client!=NULL)
    {
        LockGuard lg(client->mutex);
        state.write(client->state);
        getEnvelope(client->stamp);
    }
}


/*********************************************************************/
GeomagicClient::GeomagicClient() : state(8,0.0)
{
}


/*********************************************************************/
bool GeomagicClient::open(Searchable &config)
{
    if (!config.check("remote"))
    {
        yError("*** Geomagic Client: \"remote\" option missing, failed to open!");
        return false;
    }

    if (!config.check("local"))
    {
        yError("*** Geomagic Client: \"local\" option missing, failed to open!");
        return false;
    }

    string remote=config.find("remote").asString().c_str();
    string local=config.find("local").asString().c_str();
    verbosity=config.check("verbosity",Value(0)).asInt();

    statePort.open((local+"/state:i").c_str());
    feedbackPort.open((local+"/feedback:o").c_str());
    rpcPort.open((local+"/rpc").c_str());
    statePort.setClient(this);

    bool ok=true;
    ok|=Network::connect((remote+"/state:o").c_str(),statePort.getName().c_str(),"udp");
    ok|=Network::connect(feedbackPort.getName().c_str(),(remote+"/feedback:i").c_str(),"tcp");
    ok|=Network::connect(rpcPort.getName().c_str(),(remote+"/rpc").c_str(),"tcp");

    if (!ok)
    {
        statePort.close();
        feedbackPort.close();
        rpcPort.close();

        yError("*** Geomagic Client: unable to connect to Geomagic Server, failed to open!");
        return false;
    }

    if (verbosity>0)
        yInfo("*** Geomagic Client: opened");

    return true;
}


/*********************************************************************/
bool GeomagicClient::close()
{
    statePort.close();
    feedbackPort.close();
    rpcPort.close();

    if (verbosity>0)
        yInfo("*** Geomagic Client: closed");

    return true;
}


/*********************************************************************/
bool GeomagicClient::getPosition(Vector &pos)
{
    LockGuard lg(mutex);
    pos=state.subVector(0,2);
    return true;
}


/*********************************************************************/
bool GeomagicClient::getOrientation(Vector &rpy)
{
    LockGuard lg(mutex);
    rpy=state.subVector(3,5);
    return true;
}


/*********************************************************************/
bool GeomagicClient::getButtons(Vector &buttons)
{
    LockGuard lg(mutex);
    buttons=state.subVector(6,7);
    return true;
}


/*********************************************************************/
bool GeomagicClient::isCartesianForceModeEnabled(bool &ret)
{
    Bottle cmd,rep;
    cmd.addVocab(geomagic::is_cartesian);
    if (!rpcPort.write(cmd,rep))
    {
        yError("*** Geomagic Client: unable to get reply from Geomagic Wrapper!");
        return false;
    }

    if (rep.get(0).asVocab()==geomagic::ack)
    {
        ret=(rep.get(1).asInt()!=0);
        return true;
    }
    else
        return false;
}


/*********************************************************************/
bool GeomagicClient::setCartesianForceMode()
{
    Bottle cmd,rep;
    cmd.addVocab(geomagic::set_cartesian);
    if (!rpcPort.write(cmd,rep))
    {
        yError("*** Geomagic Client: unable to get reply from Geomagic Wrapper!");
        return false;
    }

    return (rep.get(0).asVocab()==geomagic::ack);
}


/*********************************************************************/
bool GeomagicClient::setJointTorqueMode()
{
    Bottle cmd,rep;
    cmd.addVocab(geomagic::set_joint);
    if (!rpcPort.write(cmd,rep))
    {
        yError("*** Geomagic Client: unable to get reply from Geomagic Wrapper!");
        return false;
    }

    return (rep.get(0).asVocab()==geomagic::ack);
}


/*********************************************************************/
bool GeomagicClient::getMaxFeedback(Vector &max)
{
    Bottle cmd,rep;
    cmd.addVocab(geomagic::get_max);
    if (!rpcPort.write(cmd,rep))
    {
        yError("*** Geomagic Client: unable to get reply from Geomagic Wrapper!");
        return false;
    }

    if (rep.get(0).asVocab()==geomagic::ack)
    {
		if (Bottle *payload=rep.get(1).asList())
		{
			max.resize(payload->size());
			for (size_t i=0; i<max.length(); i++)
				max[i]=payload->get(i).asDouble();
			
			return true;
		}
    }
    
	return false;
}


/*********************************************************************/
bool GeomagicClient::setFeedback(const Vector &fdbck)
{
    feedbackPort.prepare().read(const_cast<Vector&>(fdbck));
    feedbackPort.writeStrict();
    return true;
}


/*********************************************************************/
bool GeomagicClient::stopFeedback()
{
    Bottle cmd,rep;
    cmd.addVocab(geomagic::stop_feedback);
    if (!rpcPort.write(cmd,rep))
    {
        yError("*** Geomagic Client: unable to get reply from Geomagic Wrapper!");
        return false;
    }

    return (rep.get(0).asVocab()==geomagic::ack);
}


/*********************************************************************/
bool GeomagicClient::getTransformation(Matrix &T)
{
    Bottle cmd,rep;
    cmd.addVocab(geomagic::get_transformation);
    if (!rpcPort.write(cmd,rep))
    {
        yError("*** Geomagic Client: unable to get reply from Geomagic Wrapper!");
        return false;
    }

    if (rep.get(0).asVocab()==geomagic::ack)
    {
		if (Bottle *payload=rep.get(1).asList())
		{
			T.resize(payload->get(0).asInt(),
				     payload->get(1).asInt());
			
			if (Bottle *vals=payload->get(2).asList())
			{
				for (int r=0; r<T.rows(); r++)
					for (int c=0; c<T.cols(); c++)
						T(r,c)=vals->get(T.rows()*r+c).asDouble();
			
				return true;
			}
		}
    }
    
	return false;
}


/*********************************************************************/
bool GeomagicClient::setTransformation(const Matrix &T)
{
    Bottle cmd,rep;
    cmd.addVocab(geomagic::set_transformation);
    cmd.addList().read(const_cast<Matrix&>(T));
    if (!rpcPort.write(cmd,rep))
    {
        yError("*** Geomagic Client: unable to get reply from Geomagic Wrapper!");
        return false;
    }

    return (rep.get(0).asVocab()==geomagic::ack);
}


/*********************************************************************/
Stamp GeomagicClient::getLastInputStamp()
{
    LockGuard lg(mutex);
    return stamp;
}

