/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * email:  ugo.pattacini@iit.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#include <geomagic/geomagic.h>
#include <yarp/os/all.h>

using namespace yarp::os;


/****************************************************************/
class Driver : public RFModule
{
    BufferedPort<Bottle> outPort;
    double period;
    GeoMagic *geomagic;

public:
    
    /**
     * Function called once at start-up; useful to 
     * retrieve parameters from the command-line.
     */
    bool configure(ResourceFinder &rf)
    {
        // retrieve the thread periodicity (in seconds) from the command-line:
        // try out something like: "geomagic-driver --period 0.05"
        // default is 0.01 [s]
        period=rf.check("period",Value(0.01)).asDouble();

        // open up the yarp port
        outPort.open("/geomagic:o");
        
        return true;
    }

    /**
     * Function called once CTRL+C sequence is detected; useful to 
     * close everything gracefully. 
     */
    bool close()
    {
        // close the port
        outPort.close();
        return true;
    }

    /**
     * Establish the periodicity in seconds.
     */
    double getPeriod()
    {        
        return period;
    }

    /**
     * The body of thread, called each <period> seconds.
     */
    bool updateModule()
    {
        // cartesian position
        double pos[3];

        // orientation: roll, pitch, yaw (or any other representation)
        double rpy[3];

        // button status
        int b1,b2;

        // here comes the part where to call Geomagic API
        // and populate pos,rpy,b1,b2
        // ...
        geomagic->getDeviceData(pos, rpy, &b1, &b2);
        //pos[0]=1.0;
        //pos[1]=2.0;
        //pos[2]=3.0;

        //rpy[0]=0.1;
        //rpy[1]=0.2;
        //rpy[2]=0.3;

        //b1=0;
        //b2=1;

        // here we transmit these information over yarp 

        // we take a refrence to the internal buffer of the port
        Bottle &content=outPort.prepare();
        // we clean it up
        content.clear();

        content.addDouble(pos[0]);
        content.addDouble(pos[1]);
        content.addDouble(pos[2]);
        content.addDouble(rpy[0]);
        content.addDouble(rpy[1]);
        content.addDouble(rpy[2]);
        
        content.addInt(b1);
        content.addInt(b2);

        yDebug()<<content.toString().c_str();
        
        outPort.write();

        return true;
    }

    /**
     * Set GeoMagic Device Object Reference.
     */
    void setGeoMagic(GeoMagic *geomagic)
    {
        this->geomagic = geomagic;
    }
};


/****************************************************************/
int main(int argc,char *argv[])
{
    // always check if yarp network is up
    Network yarp;
    if (!yarp.checkNetwork())
    {
        yError("YARP server not available!");
        return 1;
    }

    // start GeoMagic Device
    GeoMagic geomagic;
    if (!geomagic.isActive())
    {
        yError("GeoMagic device error!");
        return 2;
    }

    // instantiate a class to deal with
    // command-line options
    ResourceFinder rf;
    rf.configure(argc,argv);

    // create an instance of the driver
    // and start it off
    Driver driver;
    driver.setGeoMagic(&geomagic);
    return driver.runModule(rf);
}


