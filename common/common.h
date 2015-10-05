// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later.
 *
 */

#ifndef __GEOMAGIC_COMMON__
#define __GEOMAGIC_COMMON__

#include <yarp/os/Vocab.h>

namespace geomagic {
    enum {
        ack                = VOCAB3('a','c','k'),
        nack               = VOCAB4('n','a','c','k'),
        get_transformation = VOCAB4('g','t','r','a'),
        set_transformation = VOCAB4('s','t','r','a'),
        stop_feedback      = VOCAB4('s','t','o','p'),
        is_cartesian       = VOCAB3('i','s','f'),
        set_cartesian      = VOCAB4('s','c','a','r'),
        set_joint          = VOCAB4('s','j','n','t'),
        get_max            = VOCAB4('g','m','a','x')
    };
}

#endif


