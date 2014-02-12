//
//  Params.cpp
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#include "ParticleApp.h"
#include "Params.h"

Params::Params()
{
    // kinect params
    setb("kinect", false);
    setf("kdepthThresholdLo", .3);
    setf("kdepthThresholdHi", 1);
    
    // TODO deprecate
    setb("bounce", false);
    
    setf("size", 30);
    setf("lifespan", 10.0);
    seti("density", 10);
    setf("pulse_rate", 0);
    setf("pulse_amplitude", 1.0);
    seti("symmetry", 1);
    seti("draw_style", 0);
    setf("gravity", 0);
    setf("noise", .3);
    setf("velocity", .25);
}

ParamsPtr &Params::get()
{
    return ParticleApp::params();
}
