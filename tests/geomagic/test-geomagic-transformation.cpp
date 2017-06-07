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

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;


/**********************************************************/
class TestModule: public RFModule
{
protected:
    PolyDriver driver;
    IHapticDevice *igeo;
    
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
            option.put("device","hapticdeviceclient"); 
            option.put("remote","/hapticdevice");
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

        if (buttons[0]!=0.0)
        {
            Matrix T1;
            igeo->getTransformation(T1);
            T1=SE3inv(T1);

            Matrix T2=eye(4,4);
            T2(0,3)=pos[0];
            T2(1,3)=pos[1];
            T2(2,3)=pos[2];

            igeo->setTransformation(SE3inv(T1*T2));
        }
        else if (buttons[1]!=0.0)
            igeo->setTransformation(eye(4,4));

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


