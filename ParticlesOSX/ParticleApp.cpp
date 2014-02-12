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

ParticleApp::~ParticleApp()
{
    if (m_initialized) {
        delete m_ui_manager;
    }
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
        
        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GetGLError();
        
        m_ui_manager = new PUI::PUIManager;

        int slider_height = 15;
        int slider_width = 100;

        int control_x = 4;
        
        int control_spacing_y = 5;
        int control_y = control_spacing_y;
        
        {
            Color fillColor = Color(255/255.0, 99/255.0, 71/255.0);
            PUI::PSlider *pSlider = new PUI::PSlider("size", 1, 30, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
            pSlider->setValue(m_params->getf("size"));
            pSlider->setColors(fillColor, Color::DkGray);
            pSlider->setDelegate(ptrParticleController);
            m_ui_manager->addControl(pSlider);
            control_y += control_spacing_y + slider_height;
        }

        {
            Color fillColor = Color(152/255.0, 251/255.0, 152/255.0);
            PUI::PSlider *pSlider = new PUI::PSlider("lifespan", 1, 50, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
            pSlider->setValue(m_params->getf("lifespan"));
            pSlider->setColors(fillColor, Color::DkGray);
            pSlider->setDelegate(ptrParticleController);
            m_ui_manager->addControl(pSlider);
            control_y += control_spacing_y + slider_height;
        }

        {
            Color fillColor = Color(255/255.0, 218/255.0, 185/255.0);
            PUI::PSlider *pSlider = new PUI::PSlider("density", 1, 20, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
            pSlider->setValue(m_params->geti("density"));
            pSlider->setColors(fillColor, Color::DkGray);
            pSlider->setDelegate(ptrParticleController);
            m_ui_manager->addControl(pSlider);
            control_y += control_spacing_y + slider_height;
        }

        {
            Color fillColor = Color(255/255.0, 228/255.0, 225/255.0);
            PUI::PSlider *pSlider = new PUI::PSlider("pulse_rate", 0, 5, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
            pSlider->setValue(m_params->getf("pulse_rate"));
            pSlider->setColors(fillColor, Color::DkGray);
            pSlider->setDelegate(ptrParticleController);
            m_ui_manager->addControl(pSlider);
            control_y += control_spacing_y + slider_height;
        }
        
        {
            Color fillColor = Color(0/255.0, 206/255.0, 209/255.0);
            PUI::PSlider *pSlider = new PUI::PSlider("gravity", -.5, .5, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
            pSlider->setValue(m_params->getf("gravity"));
            pSlider->setColors(fillColor, Color::DkGray);
            pSlider->setDelegate(ptrParticleController);
            m_ui_manager->addControl(pSlider);
            control_y += control_spacing_y + slider_height;
        }
        
        {
            Color fillColor = Color(220/255.0, 20/255.0, 60/255.0);
            PUI::PSlider *pSlider = new PUI::PSlider("symmetry", 1, MAX_SYMMETRY, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
            pSlider->setValue(m_params->geti("symmetry"));
            pSlider->setColors(fillColor, Color::DkGray);
            pSlider->setDelegate(ptrParticleController);
            m_ui_manager->addControl(pSlider);
            control_y += control_spacing_y + slider_height;

            m_symmetry_slider = pSlider;
        }

        {
            Color fillColor = Color(255/255.0, 250/255.0, 250/255.0);
            PUI::PSlider *pSlider = new PUI::PSlider("noise", 0, 1, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
            pSlider->setValue(m_params->getf("noise"));
            pSlider->setColors(fillColor, Color::DkGray);
            pSlider->setDelegate(ptrParticleController);
            m_ui_manager->addControl(pSlider);
            control_y += control_spacing_y + slider_height;
        }

        {
            Color fillColor = Color(30/255.0, 150/255.0, 250/255.0);
            PUI::PSlider *pSlider = new PUI::PSlider("velocity", 0, 2, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
            pSlider->setValue(m_params->getf("velocity"));
            pSlider->setColors(fillColor, Color::DkGray);
            pSlider->setDelegate(ptrParticleController);
            m_ui_manager->addControl(pSlider);
            control_y += control_spacing_y + slider_height;
        }
        
        {
            Color fillColor = Color(255/255.0, 239/255.0, 213/255.0);
            PUI::PSlider *pSlider = new PUI::PSlider("draw_style", 0, 2, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
            pSlider->setValue(m_params->geti("draw_style"));
            pSlider->setColors(fillColor, Color::DkGray);
            pSlider->setDelegate(ptrParticleController);
            m_ui_manager->addControl(pSlider);
            control_y += control_spacing_y + slider_height;
        }

        
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
        //cout << "FPS: " << int(1 / frameTime) << ", #: " << numParticles << endl;
    }
    
    m_lastAppTime = elapsed;
    m_lastFrameTime = frameTime;

    GetGLError();

}

void ParticleApp::draw() {

    if (!m_initialized) {
        return;
    }

    m_shader_program->useProgram();
    
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
    
    
    m_ui_manager->draw();
    
    GetGLError();

}

void ParticleApp::resize(int w, int h) {
    m_window_size = Vec2(w, h);
    m_ui_manager->resize(m_window_size);
    glViewport(0, 0, w, h);
    cout << "resize " << w << " x " << h << endl;
}

void ParticleApp::keyDown(int keyCode, int modifiers)
{
    switch (keyCode) {
        case 'c': {
            BOOST_FOREACH(PtrParticleController particleController, particleControllers) {
                particleController->removeParticles(particleController->numParticles());
            }
        } break;
            
        case 'f': {
            //            setFullScreen(!isFullScreen());
        } break;
            
        case 'S': {
            int symmetry = m_params->geti("symmetry");
            symmetry--;
            if (symmetry == 0) {
                symmetry = MAX_SYMMETRY;
            }
            m_params->seti("symmetry", symmetry);
            m_symmetry_slider->setValue(symmetry);
        } break;
            
        case 's': {
            int symmetry = m_params->geti("symmetry");
            symmetry++;
            if (symmetry > MAX_SYMMETRY) {
                symmetry = 1;
            }
            m_params->seti("symmetry", symmetry);
            m_symmetry_slider->setValue(symmetry);
        } break;

        case 'r': {
            PtrParticleController activeController = particleControllers.front();
            activeController->startRecording();
        } break;
    }
}

void ParticleApp::keyUp(int keyCode, int modifiers)
{
    switch (keyCode) {
        case 'r': {
            PtrParticleController activeController = particleControllers.front();
            activeController->stopRecording();
        } break;
    }
}

void ParticleApp::mouseDownAt(int x, int y) {

    PUI::MouseEvent event(Vec2(x, y));
    if (!m_ui_manager->mouseDown(&event)) {
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
}

void ParticleApp::mouseDraggedAt(int x, int y) {

    PUI::MouseEvent event(Vec2(x, y));
    if (!m_ui_manager->mouseDrag(&event)) {

        x -= windowSize().x() / 2;
        y -= windowSize().y() / 2;
        
        //cout << "mouseDragAt " << x << ", " << y << endl;
        
        Vec2 mouse_pos = Vec2(x, y);
        Vec2 mouse_vec = (mouse_pos - m_lastMouseLoc) * m_params->getf("velocity");
        addParticleAt(mouse_pos, mouse_vec, ParticleController::eMouseDrag);
        
        m_lastMouseLoc = Vec2(x, y);
    }
}

void ParticleApp::mouseUpAt(int x, int y)
{
    PUI::MouseEvent event(Vec2(x, y));
    m_ui_manager->mouseUp(&event);
}

void ParticleApp::mouseMovedAt(int x, int y)
{
    PUI::MouseEvent event(Vec2(x, y));
    m_ui_manager->mouseMove(&event);
}

void ParticleApp::addParticleAt(Vec2 position, Vec2 vector, ParticleController::ControlType type)
{
    PtrParticleController activeController = particleControllers.front();
    activeController->addParticleAt(position, vector, type);
}

