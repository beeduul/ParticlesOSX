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

#include <sstream>
#include <boost/foreach.hpp>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreFoundation/CFString.h>

#ifdef USE_KINECT
#import "libfreenect.h"

std::vector<uint16_t> ParticleApp::m_depthBuffer(640*480);
std::vector<uint8_t> ParticleApp::m_videoBuffer(640*480*4);
#endif

std::vector<ShaderProgramPtr> ParticleApp::m_shader_programs;

ParamsPtr ParticleApp::m_params;

ParticleApp::ParticleApp() :
    m_initialized(false),
    m_lastFrameTime(0)
{
}

ParticleApp::~ParticleApp()
{
    if (m_initialized) {

#ifdef USE_LEAP
        // Remove the listener when done
        m_leapController.removeListener(m_leapListener);
#endif

        delete m_graphics;

#ifdef USE_KINECT
        shutdownKinect();
#endif
        
    }
}

#ifdef USE_KINECT
void ParticleApp::shutdownKinect()
{
    if (!m_freenectDevice) {
        return;
    }

    m_freenectDevice->stopDepth();
    m_freenectDevice->stopDepth();
}
#endif

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

        m_graphics = new PUI::PGraphics;
        
        bool kinect =
            #ifdef USE_KINECT
            initializeKinect() ||
            #endif
            false;

        #ifdef USE_LEAP
        // Have the listener receive events from the controller
        m_leapController.addListener(m_leapListener);
        #endif
        
        PtrParticleController controller;
        for (int controllerIndex = 0; controllerIndex < m_particleControllers.size(); controllerIndex++) {
            controller = PtrParticleController(new ParticleController(this));
            controller->initialize(kinect && controllerIndex == 0);
//            ptrParticleController->setParams(m_params);
            m_particleControllers[controllerIndex] = controller;
#ifdef USE_LEAP
            if (controllerIndex == 9) {
                m_leapListener.addParticleController(controller);
            }
#endif
        }
        setActiveController(1);

        cout << "ParticleApp " << step++ << endl;
        GetGLError();

        cout << "DONE ParticleApp " << step++ << endl;
        GetGLError();
        
        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GetGLError();

        
        
    }
    cout << "ParticleApp.initialized " << endl;
    
    GetGLError();

    return m_initialized;
}

#ifdef USE_KINECT
bool ParticleApp::initializeKinect()
{
    m_freenectDevice = 0;
    try {
        m_freenectDevice = &m_freenect.createDevice<MyFreenectDevice>(0);
        m_freenectDevice->startVideo();
        m_freenectDevice->startDepth();
    } catch (std::runtime_error e) {
        cout << "WARNING could not initialize Kinect";
    }
    return m_freenectDevice != 0;
}
#endif


void ParticleApp::initializeShaders()
{
    bool ok;
    string vertex_shader_source;
    string fragment_shader_source;
    
    m_shader_programs.reserve(2);
    
    m_shader_programs[kShaderParticles] = ShaderProgramPtr(new ShaderProgram);
    vertex_shader_source = getFileContents("p01.vert");
    fragment_shader_source = getFileContents("p01.frag");
    ok = m_shader_programs[kShaderParticles]->initialize(vertex_shader_source, fragment_shader_source);
    if (!ok) {
        cerr << "PROGRAM LINK ERROR kShaderParticles" << endl;
    }

    m_shader_programs[kShaderSimple] = ShaderProgramPtr(new ShaderProgram);
    vertex_shader_source = getFileContents("rect.vert");
    fragment_shader_source = getFileContents("rect.frag");
    ok = m_shader_programs[kShaderSimple]->initialize(vertex_shader_source, fragment_shader_source);
    if (!ok) {
        cerr << "PROGRAM LINK ERROR kShaderSimple" << endl;
    }
    
    GetGLError();
}

void ParticleApp::update() {

    if (!m_initialized) {
        return;
    }

#ifdef USE_KINECT
    updateKinect();
#endif
    
    g_buffer_mutex.lock();
    int numParticles = 0;
    BOOST_FOREACH(PtrParticleController particleController, m_particleControllers) {
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

#ifdef USE_KINECT
void ParticleApp::updateKinect()
{
    if (!m_freenectDevice) {
        return;
    }
    
    m_freenectDevice->updateState();
    m_freenectDevice->getRGB(m_videoBuffer);
    m_freenectDevice->getDepth(m_depthBuffer);
    
    PtrParticleController kinectController = m_particleControllers.front();
    
    int oldDensity = m_params->geti("density");
    m_params->seti("density", m_params->geti("kdensity"));
    
    float oldLifespan = m_params->getf("lifespan");
    m_params->setf("lifespan", m_params->getf("klifespan"));
    
    Color oldBirthColor = m_params->getColor("birthColor");
    Color oldDeathColor = m_params->getColor("deathColor");
    
    Color kinectBirthColor = m_params->getColor("kinectBirthColor");
    Color kinectDeathColor = m_params->getColor("kinectDeathColor");
    
    int step = 4; // Rand::randInt(4, 32);
    
    int skipRate = 2;
    
    int threshold = m_params->geti("kthreshold");
    
    for (int j = 0; j < 480; j += step) {
        for (int i = 0; i < 640; i += step) {
            int offset = j * 640 + (640 - i);
            int depth = m_depthBuffer[offset];
            if (depth < threshold) {
                float x = (i - 320) * (windowSize().x() / (float) 640);
                float y = (j - 240) * (windowSize().y() / (float) 480);
                Vec2 position(x, y);
                int crazy = 5;
                Vec2 direction(Rand::randFloat(-crazy, crazy), Rand::randFloat(-crazy, crazy));
                int videoBufferOffset = offset * 4;
                float colorMagnitude = 1 - (float)depth / threshold;
                //                Color videoColor(m_videoBuffer[videoBufferOffset], m_videoBuffer[videoBufferOffset + 1], m_videoBuffer[videoBufferOffset + 2]);
                //                Color pixelColor = (depthColor + videoColor) / 2;
                m_params->setColor("birthColor", kinectBirthColor * colorMagnitude);
                m_params->setColor("deathColor", kinectDeathColor * colorMagnitude);
                kinectController->emitParticle(position, direction);
            }
        }
    }
    
    m_params->seti("density", oldDensity);
    m_params->setf("lifespan", oldLifespan);

    m_params->setColor("birthColor", oldBirthColor);
    m_params->setColor("deathColor", oldDeathColor);
    

}
#endif

void ParticleApp::draw() {

    if (!m_initialized) {
        return;
    }

    ShaderProgramPtr shader_program = ParticleApp::getShaderProgram(ParticleApp::kShaderSimple);

    Color clearColor(Color::Black);
    glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a());
    GetGLError();

    glClear(GL_COLOR_BUFFER_BIT);
    GetGLError();

    g_buffer_mutex.lock();
    BOOST_FOREACH(PtrParticleController particleController, m_particleControllers) {
        particleController->draw(*m_graphics);
    }
    getActiveController()->drawUI(*m_graphics);
    
    g_buffer_mutex.unlock();
    
    
    GetGLError();

}

void ParticleApp::resize(int w, int h) {
    m_window_size = Vec2(w, h);
    m_graphics->resize(m_window_size);
    glViewport(0, 0, w, h);
    cout << "resize " << w << " x " << h << endl;
}

void ParticleApp::keyDown(int keyCode, PKeyModifier modifiers)
{
    cout << "keyCode " << keyCode << ", modifiers: " << modifiers << endl;
    switch (keyCode) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            setActiveController(keyCode - '0');
        } break;
            
        case 'C': {
            BOOST_FOREACH(PtrParticleController particleController, m_particleControllers) {
                particleController->removeParticles(particleController->numParticles());
            }
        } break;
        
        default: {
            getActiveController()->keyDown(keyCode, modifiers);
        }
    }
}

void ParticleApp::keyUp(int keyCode, PKeyModifier modifiers)
{
    switch (keyCode) {
        case 'r': {
            getActiveController()->stopRecording();
        } break;
    }
}

void ParticleApp::mouseDownAt(int x, int y) {

    Vec2 mouse_pos(x, y);
    
    cout << "mouseDownAt " << x << ", " << y << endl;
    
    PUI::MouseEvent event(mouse_pos);

    if (!getActiveController()->mouseDown(event))
    {
        m_recentCursorPositions.clear();
        m_recentCursorPositions.push_front(mouse_pos);
    }
}

void ParticleApp::mouseDraggedAt(int x, int y) {

    Vec2 mouse_pos(x, y);
    PUI::MouseEvent event(mouse_pos);
    Vec2 mouse_vec(0, 0);
    if (!getActiveController()->mouseDrag(event, mouse_vec)) {
        //cout << "mouseDragAt " << x << ", " << y << endl;
        
        m_recentCursorPositions.push_front(mouse_pos);
        if (m_recentCursorPositions.size() > kCursorPositionVectorMaxLength) {
            m_recentCursorPositions.pop_back();
        }
    }
}

void ParticleApp::mouseUpAt(int x, int y)
{
    PUI::MouseEvent event(Vec2(x, y));
    getActiveController()->mouseUp(event);
}

void ParticleApp::mouseMovedAt(int x, int y)
{
    Vec2 mouse_pos(x, y);
    PUI::MouseEvent event(mouse_pos);
//    const Vec2& lastCursorPosition = m_recentCursorPositions.back();
//    Vec2 mouse_vec = (mouse_pos - lastCursorPosition) / m_recentCursorPositions.size();
    getActiveController()->mouseMove(event);
}

const Vec2 ParticleApp::mousePosition() const
{
    if (m_recentCursorPositions.size() > 0) {
        return m_recentCursorPositions.front();
    } else {
        return Vec2(0, 0);
    }
}

const Vec2 ParticleApp::canvasPosition() const
{
    return worldToView(mousePosition());
}

const Vec2 ParticleApp::worldToView(const Vec2 &vec) const
{
    return Vec2(vec - windowSize() / 2);
}

void ParticleApp::addParticleAt(Vec2 position, Vec2 vector, ParticleController::ControlType type)
{
    getActiveController()->addParticleAt(position, vector, type);
}

PtrParticleController ParticleApp::getActiveController()
{
    return m_activeController;
}

void ParticleApp::setActiveController(int controllerIndex)
{
    if (controllerIndex < 0 || controllerIndex >= m_particleControllers.size()) {
        std::stringstream ss;
        ss.str ("Invalid controller Index ");
        ss << controllerIndex << endl;
        throw ss.str();
    }
    m_activeController = m_particleControllers[controllerIndex];
}


PtrParticleController ParticleApp::getKinectController()
{
    PtrParticleController kinectController;
    BOOST_FOREACH(PtrParticleController particleController, m_particleControllers) {
        if (particleController->isKinect()) {
            return particleController;
        }
    }
    return nullptr;
}
