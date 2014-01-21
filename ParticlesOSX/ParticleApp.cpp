//
//  ParticleApp.cpp
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#include "ParticleApp.h"

#include "Color.h"
#include "Rand.h"

#include <boost/foreach.hpp>

#include <OpenGL/gl3.h>

ParamsPtr ParticleApp::m_params;

ParticleApp::ParticleApp() :
    m_initialized(false)
{
}

bool ParticleApp::initialize()
{
    if (!m_initialized) {
        const GLubyte *strVersion = glGetString (GL_VERSION);
        cout << "OpenGL Version: " << strVersion << endl;

        particleControllers.push_back(PtrParticleController(new ParticleController(this)));
        
        Clock::reset();
        
        m_params = ParamsPtr(new Params());
        m_initialized = true;
    }
    
    return m_initialized;
}


void ParticleApp::update() {
    int numParticles = 0;
    BOOST_FOREACH(PtrParticleController particleController, particleControllers) {
        particleController->update();
        numParticles += particleController->numParticles();
    }
}

void ParticleApp::draw() {
    Color clearColor(145/255.0, 129/255.0, 81/255.0);
    glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a());
    glClear(GL_COLOR_BUFFER_BIT);

    BOOST_FOREACH(PtrParticleController particleController, particleControllers) {
        particleController->draw();
    }

}

void ParticleApp::resize(int w, int h) {
    m_window_size = Vec2(w, h);
}

void ParticleApp::mouseDownAt(int x, int y) {
    Color birthColor(Rand::randFloat(), Rand::randFloat(), Rand::randFloat());
    Color deathColor = Color(1 - birthColor.r(), 1 - birthColor.g(), 1 - birthColor.b());
    m_params->setColor("birthColor", birthColor);
    m_params->setColor("deathColor", deathColor);
    
    PtrParticleController activeController = particleControllers.front();
    activeController->setParams(m_params);
    
    m_lastMouseLoc = Vec2(x, y);
    addParticleAt(m_lastMouseLoc, Vec2(0, 0), ParticleController::eMouseDown);

}

void ParticleApp::mouseDraggedAt(int x, int y) {

    Vec2 mouse_pos = Vec2(x, y);
    Vec2 mouse_vec = mouse_pos - m_lastMouseLoc;
    addParticleAt(mouse_pos, mouse_vec * .25, ParticleController::eMouseDrag);
    
    m_lastMouseLoc = Vec2(x, y);
    
}

void ParticleApp::addParticleAt(Vec2 position, Vec2 vector, ParticleController::ControlType type)
{
    PtrParticleController activeController = particleControllers.front();
    activeController->addParticleAt(position, vector, type);
}
