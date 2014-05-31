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
    setf("size", 20);
    setf("lifespan", 1);
    seti("density", 1);
    setf("pulse_rate", 0);
    setf("pulse_amplitude", 1.0);
    seti("symmetry", 1);
    seti("draw_style", 0);
    setf("gravity", 0);
    setf("noise", 0.1);
    setf("jitter", 0);

    setColor("birthColor", Color::White);
    setColor("deathColor", Color::Black);

    // kinect params
    seti("kthreshold", 0);
    seti("kdensity", -10);
    setf("klifespan", 1.0);
    setColor("kinectBirthColor", Color::Red);
    setColor("kinectDeathColor", Color(1, .7, .7));
}

ParamsPtr &Params::get()
{
    return ParticleApp::params();
}
