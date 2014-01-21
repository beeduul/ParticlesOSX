//
//  ParticleApp.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef __ParticlesOSX__ParticleApp__
#define __ParticlesOSX__ParticleApp__

#include "Params.h"
#include "ParticleController.h"

#include "Vec2.h"

#include <ctime>
#include <list>

class ParticleApp {
public:
    ParticleApp();

    bool initialize();
    void init_gl() {}
    void activate() {};
    void deactivate() {};
    
    void update();
    void draw();
    void resize(int w, int h);
    
    void mouseDownAt(int x, int y);
    void mouseDraggedAt(int x, int y);
    
    static ParamsPtr& params() {
        return m_params;
    }

    Vec2 windowSize() { return m_window_size; }
    
private:
    bool m_initialized;
    
    Vec2 m_lastMouseLoc;
    Vec2 m_window_size;
    
    void addParticleAt(Vec2 position, Vec2 vector, ParticleController::ControlType type);

    std::list<PtrParticleController> particleControllers;

    static ParamsPtr m_params;
};


#endif /* defined(__ParticlesOSX__ParticleApp__) */
