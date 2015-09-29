// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#ifndef __GEOMAGIC_INTERFACE__
#define __GEOMAGIC_INTERFACE__

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

/**
 * A generic Geomagic interface
 */
class IGeomagic
{
public:
    virtual ~IGeomagic() { }

    /**
     * Get the instantaneous position.
     * @param pos vector containing the returned x-y-z coordinates 
     *            expressed in [m].
     * @return true/false on success/failure.
     */
    virtual bool getPosition(yarp::sig::Vector &pos)=0;

    /**
     * Get the instantaneous orientation.
     * @param rpy vector containing the returned roll-pitch-yaw 
     *            coordinates expressed in [deg].
     * @return true/false on success/failure.
     */
    virtual bool getOrientation(yarp::sig::Vector &rpy)=0;

    /**
     * Get the status of the available buttons.
     * @param buttons vector containing the status of each available
     *                button expressed as a double in [0,1].
     * @return true/false on success/failure.
     */
    virtual bool getButtons(yarp::sig::Vector &buttons)=0;

    /**
     * Set the values for the force feedback.
     * @param force vector containing the force feedback values.
     * @return true/false on success/failure.
     */
    virtual bool setForceFeedback(const yarp::sig::Vector &force)=0;

    /**
     * Set the transformation matrix to be applied to position and 
     * orientation data. 
     * @param T the transformation matrix.
     * @return true/false on success/failure.
     */
    virtual bool setTransformation(const yarp::sig::Matrix &T)=0;

    /**
     * Get the current transformation matrix used to modify the 
     * position and orientation readings.
     * @param T the returned transformation matrix.
     * @return true/false on success/failure.
     */
    virtual bool getTransformation(yarp::sig::Matrix &T)=0;
};

#endif
//


