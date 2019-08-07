// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#ifndef __HAPTICDEVICE_COMMON__
#define __HAPTICDEVICE_COMMON__

#include <yarp/os/Vocab.h>

namespace hapticdevice {
    enum {
        ack                = yarp::os::createVocab('a','c','k'),
        nack               = yarp::os::createVocab('n','a','c','k'),
        get_transformation = yarp::os::createVocab('g','t','r','a'),
        set_transformation = yarp::os::createVocab('s','t','r','a'),
        stop_feedback      = yarp::os::createVocab('s','t','o','p'),
        is_cartesian       = yarp::os::createVocab('i','s','f'),
        set_cartesian      = yarp::os::createVocab('s','c','a','r'),
        set_joint          = yarp::os::createVocab('s','j','n','t'),
        get_max            = yarp::os::createVocab('g','m','a','x')
    };
}

#endif


