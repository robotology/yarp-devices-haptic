// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#include <string>
#include <cmath>
#include <algorithm>
#include <map>

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <yarp/math/Math.h>

#include <hapticdevice/IHapticDevice.h>

#define DEG2RAD     (M_PI/180.0)
#define RAD2DEG     (180.0/M_PI)

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;
using namespace hapticdevice;


/**********************************************************/
class TeleOp: public RFModule
{
protected:
    PolyDriver         drvCart;
    PolyDriver         drvHand;
    PolyDriver         drvGaze;
    PolyDriver         drvGeomagic;
    ICartesianControl *iarm;
    IControlMode2     *imod;
    IPositionControl2 *ipos;
    IVelocityControl2 *ivel;
    IGazeControl      *igaze;
    IHapticDevice     *igeo;
    
    BufferedPort<Bottle> forceFbPort;
    RpcClient simPort;

    string part;
    int startup_context;

    enum {
        idle,
        triggered,
        running
    };

    int s0,s1;
    int c0,c1;
    bool simulator;
    bool gaze;
    bool onlyXYZ;
    map<int,string> stateStr;

    Matrix Tsim;
    Vector pos0,rpy0;
    Vector x0,o0;

    VectorOf<int> joints,modes;
    Vector vels;

    Vector maxFeedback;
    Vector feedback;
    double minForce;
    double maxForce;

public:
    /**********************************************************/
    bool configure(ResourceFinder &rf)
    {
        string name=rf.check("name",Value("teleop-icub")).asString().c_str();
        string robot=rf.check("robot",Value("icub")).asString().c_str();
        string geomagic=rf.check("geomagic",Value("geomagic")).asString().c_str();
        double Tp2p=rf.check("Tp2p",Value(1.0)).asDouble();
        part=rf.check("part",Value("right_arm")).asString().c_str();
        simulator=rf.check("simulator",Value("off")).asString()=="on";
        gaze=rf.check("gaze",Value("off")).asString()=="on";
        minForce=fabs(rf.check("min-force-feedback",Value(3.0)).asDouble());
        maxForce=fabs(rf.check("max-force-feedback",Value(15.0)).asDouble());
        bool torso=rf.check("torso",Value("on")).asString()=="on";

        Property optGeo("(device hapticdeviceclient)");
        optGeo.put("remote",("/"+geomagic).c_str());
        optGeo.put("local",("/"+name+"/geomagic").c_str());
        if (!drvGeomagic.open(optGeo))
            return false;
        drvGeomagic.view(igeo);

        if (simulator)
        {
            simPort.open(("/"+name+"/simulator:rpc").c_str());
            if (!Network::connect(simPort.getName().c_str(),"/icubSim/world"))
            {
                yError("iCub simulator is not running!");
                drvGeomagic.close();
                simPort.close();
                return false;
            }
        }

        if (gaze)
        {
            Property optGaze("(device gazecontrollerclient)");
            optGaze.put("remote","/iKinGazeCtrl");
            optGaze.put("local",("/"+name+"/gaze").c_str());
            if (!drvGaze.open(optGaze))
            {
                drvGeomagic.close();
                simPort.close();
                return false;
            }
            drvGaze.view(igaze);
        }

        Property optCart("(device cartesiancontrollerclient)");
        optCart.put("remote",("/"+robot+"/cartesianController/"+part).c_str());
        optCart.put("local",("/"+name+"/cartesianController/"+part).c_str());
        if (!drvCart.open(optCart))
        {
            drvGeomagic.close();
            if (simulator)
                simPort.close();
            if (gaze)
                drvGaze.close();            
            return false;
        }
        drvCart.view(iarm);

        Property optHand("(device remote_controlboard)");
        optHand.put("remote",("/"+robot+"/"+part).c_str());
        optHand.put("local",("/"+name+"/"+part).c_str());
        if (!drvHand.open(optHand))
        {
            drvGeomagic.close();
            if (simulator)
                simPort.close();
            if (gaze)
                drvGaze.close();
            drvCart.close();
            return false;
        }
        drvHand.view(imod);
        drvHand.view(ipos);
        drvHand.view(ivel);

        iarm->storeContext(&startup_context);
        iarm->restoreContext(0);

        Vector dof(10,1.0);
        if (!torso)
            dof[0]=dof[1]=dof[2]=0.0;
        else
            dof[1]=0.0;
        iarm->setDOF(dof,dof);
        iarm->setTrajTime(Tp2p);
        
        Vector accs,poss;
        for (int i=0; i<9; i++)
        {
            joints.push_back(7+i);
            modes.push_back(VOCAB_CM_POSITION);
            accs.push_back(1e9);
            vels.push_back(100.0);
            poss.push_back(0.0);
        }
        poss[0]=20.0;
        poss[1]=70.0;
        
        imod->setControlModes(joints.size(),joints.getFirst(),modes.getFirst());
        ipos->setRefAccelerations(joints.size(),joints.getFirst(),accs.data());
        ipos->setRefSpeeds(joints.size(),joints.getFirst(),vels.data());
        ipos->positionMove(joints.size(),joints.getFirst(),poss.data());

        joints.clear();
        modes.clear();
        vels.clear();
        for (int i=2; i<9; i++)
        {
            joints.push_back(7+i);
            modes.push_back(VOCAB_CM_VELOCITY);
            vels.push_back(40.0);
        }
        vels[vels.length()-1]=100.0;
        
        s0=s1=idle;
        c0=c1=0;
        onlyXYZ=true;
        
        stateStr[idle]="idle";
        stateStr[triggered]="triggered";
        stateStr[running]="running";

        Matrix T=zeros(4,4);
        T(0,1)=1.0;
        T(1,2)=1.0;
        T(2,0)=1.0;
        T(3,3)=1.0;
        igeo->setTransformation(SE3inv(T));
        igeo->setCartesianForceMode();
        igeo->getMaxFeedback(maxFeedback);
        
        Tsim=zeros(4,4);
        Tsim(0,1)=-1.0;
        Tsim(1,2)=1.0;  Tsim(1,3)=0.5976;
        Tsim(2,0)=-1.0; Tsim(2,3)=-0.026;
        Tsim(3,3)=1.0;

        pos0.resize(3,0.0);
        rpy0.resize(3,0.0);

        x0.resize(3,0.0);
        o0.resize(4,0.0);

        if (simulator)
        {
            Bottle cmd,reply;
            cmd.addString("world");
            cmd.addString("mk");
            cmd.addString("ssph");
                
            // radius
            cmd.addDouble(0.02);

            // position
            cmd.addDouble(0.0);
            cmd.addDouble(0.0);
            cmd.addDouble(0.0);
                
            // color
            cmd.addInt(1);
            cmd.addInt(0);
            cmd.addInt(0);

            // collision
            cmd.addString("FALSE");

            simPort.write(cmd,reply);
        }

        forceFbPort.open(("/"+name+"/force-feedback:i").c_str());
        feedback.resize(3,0.0);

        return true;
    }

    /**********************************************************/
    bool close()
    {
        iarm->stopControl();
        iarm->restoreContext(startup_context);
        drvCart.close();

        ivel->stop(joints.size(),joints.getFirst());
        for (size_t i=0; i<modes.size(); i++)
            modes[i]=VOCAB_CM_POSITION;
        imod->setControlModes(joints.size(),joints.getFirst(),modes.getFirst());
        drvHand.close();

        if (simulator)
        {
            Bottle cmd,reply;
            cmd.addString("world");
            cmd.addString("del");
            cmd.addString("all");
            simPort.write(cmd,reply);

            simPort.close();
        }

        if (gaze)
        {
            igaze->stopControl();
            drvGaze.close();
        }

        igeo->stopFeedback();
        igeo->setTransformation(eye(4,4));
        drvGeomagic.close();
        forceFbPort.close();

        return true;
    }

    /**********************************************************/
    void updateSim(const Vector &c_)
    {
        if ((c_.length()!=3) && (c_.length()!=4))
            return;

        Vector c=c_;        
        if (c.length()==3)
            c.push_back(1.0);
        c[3]=1.0;

        c=Tsim*c;

        Bottle cmd,reply;
        cmd.addString("world");
        cmd.addString("set");
        cmd.addString("ssph");

        // obj #
        cmd.addInt(1);

        // position
        cmd.addDouble(c[0]);
        cmd.addDouble(c[1]);
        cmd.addDouble(c[2]);

        simPort.write(cmd,reply);
    }

    /**********************************************************/
    void reachingHandler(const bool b, const Vector &pos,
                         const Vector &rpy)
    {
        if (b)
        {
            if (s0==idle)
                s0=triggered;
            else if (s0==triggered)
            {
                if (++c0*getPeriod()>0.1)
                {
                    pos0[0]=pos[0];
                    pos0[1]=pos[1];
                    pos0[2]=pos[2];

                    rpy0[0]=rpy[0];
                    rpy0[1]=rpy[1];
                    rpy0[2]=rpy[2];

                    iarm->getPose(x0,o0);
                    s0=running;
                }
            }
            else
            {
                Vector xd(4,0.0);
                xd[0]=pos[0]-pos0[0];
                xd[1]=pos[1]-pos0[1];
                xd[2]=pos[2]-pos0[2];
                xd[3]=1.0;

                Matrix H0=eye(4,4);
                H0(0,3)=x0[0];
                H0(1,3)=x0[1];
                H0(2,3)=x0[2];

                xd=H0*xd;

                Matrix Rd;
                if (onlyXYZ)
                    Rd=axis2dcm(o0);
                else
                {
                    Vector drpy(3);
                    drpy[0]=rpy[0]-rpy0[0];
                    drpy[1]=rpy[1]-rpy0[1];
                    drpy[2]=rpy[2]-rpy0[2];

                    Vector ax(4,0.0),ay(4,0.0),az(4,0.0);
                    ax[0]=1.0; ax[3]=drpy[2];
                    ay[1]=1.0; ay[3]=drpy[1]*((part=="left_arm")?1.0:-1.0);
                    az[2]=1.0; az[3]=drpy[0]*((part=="left_arm")?1.0:-1.0);

                    Rd=axis2dcm(o0)*axis2dcm(ax)*axis2dcm(ay)*axis2dcm(az);
                }

                Vector od=dcm2axis(Rd);
                iarm->goToPose(xd,od);

                if (gaze)
                    igaze->lookAtFixationPoint(xd);

                yInfo("going to (%s) (%s)",
                      xd.toString(3,3).c_str(),od.toString(3,3).c_str());

                if (simulator)
                    updateSim(xd);
            }
        }
        else
        {
            if (s0==triggered)
                onlyXYZ=!onlyXYZ;

            if (c0!=0)
            {
                iarm->stopControl();
                if (simulator)
                {
                    Vector x,o;
                    iarm->getPose(x,o);
                    updateSim(x);
                }
                if (gaze)
                    igaze->stopControl();
            }

            s0=idle;
            c0=0;
        }
    }

    /**********************************************************/
    void handHandler(const bool b)
    {
        if (b)
        {
            if (s1==idle)
                s1=triggered;
            else if (s1==triggered)
            {
                if (++c1*getPeriod()>0.1)
                {
                    imod->setControlModes(joints.size(),joints.getFirst(),modes.getFirst());
                    s1=running;
                }
            }
            else
                ivel->velocityMove(joints.size(),joints.getFirst(),vels.data());
        }
        else
        {
            if (s1==triggered)
                vels=-1.0*vels;

            if (c1!=0)
                ivel->stop(joints.size(),joints.getFirst());

            s1=idle;
            c1=0;
        }
    }

    /**********************************************************/
    double getPeriod()
    {
        return 0.01;
    }

    /**********************************************************/
    bool updateModule()
    {
        Vector buttons,pos,rpy;
        igeo->getButtons(buttons);
        igeo->getPosition(pos);
        igeo->getOrientation(rpy);

        bool b0=(buttons[0]!=0.0);
        bool b1=(buttons[1]!=0.0);

        reachingHandler(b0,pos,rpy);
        handHandler(b1);
        
        if (!b0 && !b1)
        {
            if (norm(feedback)>0.0)
            {
                igeo->stopFeedback();
                feedback=0.0;
            }
        }
        else if (Bottle *bForce=forceFbPort.read(false))
        {
            size_t sz=std::min(feedback.length(),(size_t)bForce->size());
            for (size_t i=0; i<sz; i++)
            {
                feedback[i]=bForce->get(i).asDouble();
                if ((feedback[i]>=-minForce) && (feedback[i]<=minForce))
                    feedback[i]=0.0;
                else if (feedback[i]<=-maxForce)
                    feedback[i]=-maxForce;
                else if (feedback[i]>=maxForce)
                    feedback[i]=maxForce;

                feedback[i]*=maxFeedback[i]/maxForce;
            }

            igeo->setFeedback(feedback);
            yInfo("feedback = (%s)",feedback.toString(3,3).c_str());
        }

        yInfo("[reaching=%s; pose=%s;] [hand=%s; movement=%s;]",
              stateStr[s0].c_str(),onlyXYZ?"xyz":"full",
              stateStr[s1].c_str(),vels[0]>0.0?"closing":"opening");

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

    TeleOp teleop;
    return teleop.runModule(rf);
}


