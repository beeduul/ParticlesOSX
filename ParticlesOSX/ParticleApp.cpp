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
#import <CoreFoundation/CoreFoundation.h>
#import <CoreFoundation/CFString.h>

ParamsPtr ParticleApp::m_params;

ParticleApp::ParticleApp() :
    m_initialized(false),
    m_lastFrameTime(0)
{
}

// Function
void SetCWDToMainBundle()
{
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path,
                                          PATH_MAX))
    {
        // Error
    }
    CFRelease(resourcesURL);
    chdir(path);
}

#include <fstream>
#include <string>
#include <cerrno>

std::string ParticleApp::getFileContents(const char *filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);
}

bool ParticleApp::initialize()
{
    if (!m_initialized) {
        int step = 0;
        
        SetCWDToMainBundle();
        
        cout << "ParticleApp " << step++ << endl;
        GetGLError();

        const GLubyte* strRenderer = glGetString (GL_RENDERER); // get renderer string
        cout << "OpenGL Renderer: " << strRenderer << endl;
        const GLubyte *strVersion = glGetString (GL_VERSION);
        cout << "OpenGL Version: " << strVersion << endl;

        cout << "ParticleApp " << step++ << endl;
        GetGLError();
        
        Clock::reset();
        m_lastFrameTime = 0;
        m_lastAppTime = 0;
        
        m_params = ParamsPtr(new Params());
        m_initialized = true;

        initializeShaders();
        cout << "ParticleApp initialized shaders " << step++ << endl;
        GetGLError();
        
        PtrParticleController ptrParticleController = PtrParticleController(new ParticleController(this));
        ptrParticleController->initialize();
        ptrParticleController->setParams(m_params);
        particleControllers.push_back(ptrParticleController);

        cout << "ParticleApp " << step++ << endl;
        GetGLError();

        cout << "DONE ParticleApp " << step++ << endl;
        GetGLError();
        
    }
    cout << "ParticleApp.initialized " << endl;
    
    GetGLError();

    return m_initialized;
}


void ParticleApp::initializeShaders()
{
    m_shader_program = ShaderProgramPtr(new ShaderProgram);
    
    string vertex_shader_source = getFileContents("p01.vert");
    string fragment_shader_source = getFileContents("p01.frag");

    int step = 0;
    
    bool ok;
    ok = m_shader_program->initialize(vertex_shader_source, fragment_shader_source);
    if (!ok) {
        cerr << "PROGRAM LINK ERROR" << endl;
    }
    
    cout << "ParticleController.initializeShaders " << step++ << endl;;
    GetGLError();
    
    m_shader_program->useProgram();
    cout << "ParticleController.initializeShaders " << step++ << endl;;
    GetGLError();
    
}

void ParticleApp::update() {

    if (!m_initialized) {
        return;
    }

    g_buffer_mutex.lock();
    int numParticles = 0;
    BOOST_FOREACH(PtrParticleController particleController, particleControllers) {
        particleController->update();
        numParticles += particleController->numParticles();
    }
    g_buffer_mutex.unlock();
    
    float elapsed = Clock::getElapsedSeconds();
    float frameTime = elapsed - m_lastAppTime;
    frameTime = frameTime * 0.9 + m_lastFrameTime * 0.1;
    
    if (frameTime > 0) {
        cout << "FPS: " << int(1 / frameTime) << ", #: " << numParticles << endl;
    }
    
    m_lastAppTime = elapsed;
    m_lastFrameTime = frameTime;

    GetGLError();

}

void ParticleApp::draw() {

    if (!m_initialized) {
        return;
    }

    Color clearColor(145/255.0, 129/255.0, 81/255.0);
    glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a());
    GetGLError();

    glClear(GL_COLOR_BUFFER_BIT);
    GetGLError();

    g_buffer_mutex.lock();
    BOOST_FOREACH(PtrParticleController particleController, particleControllers) {
        particleController->draw();
    }
    g_buffer_mutex.unlock();
    GetGLError();

}

void ParticleApp::resize(int w, int h) {
    m_window_size = Vec2(w, h);
    glViewport(0, 0, w, h);
    cout << "resize " << w << " x " << h << endl;
}

void ParticleApp::mouseDownAt(int x, int y) {

    x -= windowSize().x() / 2;
    y -= windowSize().y() / 2;
    
    //cout << "mouseDownAt " << x << ", " << y << endl;
    
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

    x -= windowSize().x() / 2;
    y -= windowSize().y() / 2;

    //cout << "mouseDragAt " << x << ", " << y << endl;

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

