// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <yarp/math/Math.h>

#include <geomagic/IGeomagic.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;
using namespace geomagic;


/**********************************************************/
class TestModule: public RFModule
{
protected:
    PolyDriver driver;
    IGeomagic *igeo;

    Vector force;
    double t0;
    
public:
    /**********************************************************/
    bool configure(ResourceFinder &rf)
    {
        Property option("(device geomagicdriver)");        
        if (!driver.open(option))
            return false;

        driver.view(igeo);
        igeo->setCartesianForceMode();

        igeo->getMaxFeedback(force);

        force[0]/=3.0;
        force[1]=force[2]=0.0;

        t0=Time::now();
        return true;
    }

    /**********************************************************/
    bool close()
    {
        igeo->stopFeedback();
        driver.close();
        return true;
    }

    /**********************************************************/
    double getPeriod()
    {
        return 0.1;
    }

    /**********************************************************/
    bool updateModule()
    {
        double t=Time::now();
        if (t-t0>2.0)
        {
            force=-1.0*force;
            t0=Time::now();
        }

        igeo->setFeedback(force);
        return true; 
    }
};


/**********************************************************/
int main()
{
    TestModule test;
    ResourceFinder rf;
    return test.runModule(rf);
}


