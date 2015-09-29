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

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;


/*********************************************************************/
GeomagicDriver::GeomagicDriver() : configured(false), verbosity(0),
                                   T(eye(4,4))
{
}


/*********************************************************************/
bool GeomagicDriver::open(Searchable &config)
{
    if (!configured)
    {
        verbosity=config.check("verbosity",Value(0)).asInt(); 
        configured=true;
        if (verbosity>0)
            yInfo("*** Geomagic Driver: opened");
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
    pos.resize(3);
    pos[0]=1.0;
    pos[1]=2.0;
    pos[2]=3.0;

    return true;
}


/*********************************************************************/
bool GeomagicDriver::getOrientation(Vector &rpy)
{
    rpy.resize(3);
    rpy[0]=-10.0;
    rpy[1]=-20.0;
    rpy[2]=-30.0;

    return true;
}


/*********************************************************************/
bool GeomagicDriver::getButtons(Vector &buttons)
{
    buttons.resize(2);
    buttons[0]=0.0;
    buttons[1]=1.0;

    return true;
}


/*********************************************************************/
bool GeomagicDriver::setForceFeedback(const Vector &force)
{
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

    this->T=T.submatrix(0,0,this->T.rows()-1,this->T.cols()-1);

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

