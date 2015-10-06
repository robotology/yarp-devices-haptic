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
    
public:
    /**********************************************************/
    bool configure(ResourceFinder &rf)
    {
        string mode=rf.check("mode",Value("physical")).asString().c_str();

        Property option;
        if (mode=="physical")
            option.put("device","geomagicdriver");
        else
        {
            option.put("device","geomagicclient"); 
            option.put("remote","/geomagic");
            option.put("local","/client");
        }

        if (!driver.open(option))
            return false;

        driver.view(igeo);
        return true;
    }

    /**********************************************************/
    bool close()
    {
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
        Vector pos,rpy,buttons;
        igeo->getPosition(pos);
        igeo->getOrientation(rpy);
        igeo->getButtons(buttons);

        if ((buttons[0]!=0.0) || (buttons[1]!=0.0))
        {
            Matrix T=eye(4,4);
            T(0,3)=pos[0];
            T(1,3)=pos[1];
            T(2,3)=pos[2];
            igeo->setTransformation(SE3inv(T));
        }

        yInfo("pos=(%s); rpy=(%s); buttons=(%s)",
              pos.toString(3,3).c_str(),
              rpy.toString(3,3).c_str(),
              buttons.toString(3,3).c_str());

        return true; 
    }
};


/**********************************************************/
int main(int argc,char *argv[])
{
    Network yarp;
    if (!yarp.checkNetwork())
    {
        yError("YARP server not found!");
        return 1;
    }
    
    ResourceFinder rf;
    rf.configure(argc,argv);

    TestModule test;
    return test.runModule(rf);
}


