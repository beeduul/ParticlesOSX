//
//  MyLeap.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 5/28/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef __ParticlesOSX__MyLeap__
#define __ParticlesOSX__MyLeap__

#include <iostream>
#include "Leap.h"

#include "ParticleController.h"

class MyLeapListener : public Leap::Listener {
public:
    void addParticleController(PtrParticleController);
    virtual void onInit(const Leap::Controller&);
    virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);
    virtual void onExit(const Leap::Controller&);
    virtual void onFrame(const Leap::Controller&);
    virtual void onFocusGained(const Leap::Controller&);
    virtual void onFocusLost(const Leap::Controller&);
    virtual void onDeviceChange(const Leap::Controller&);
    virtual void onServiceConnect(const Leap::Controller&);
    virtual void onServiceDisconnect(const Leap::Controller&);
private:
    PtrParticleController m_ptrParticleController;
};

#endif /* defined(__ParticlesOSX__MyLeap__) */
