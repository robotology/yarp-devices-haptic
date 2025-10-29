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
        ack                = yarp::os::createVocab32('a','c','k'),
        nack               = yarp::os::createVocab32('n','a','c','k'),
        get_transformation = yarp::os::createVocab32('g','t','r','a'),
        set_transformation = yarp::os::createVocab32('s','t','r','a'),
        stop_feedback      = yarp::os::createVocab32('s','t','o','p'),
        is_cartesian       = yarp::os::createVocab32('i','s','f'),
        set_cartesian      = yarp::os::createVocab32('s','c','a','r'),
        set_joint          = yarp::os::createVocab32('s','j','n','t'),
        get_max            = yarp::os::createVocab32('g','m','a','x')
    };
}

#endif
