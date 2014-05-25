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

#include "PUI.h"

#ifdef USE_KINECT
#include "MyFreenectDevice.h"
#endif

#include <ctime>
#include <list>


#define MAX_SYMMETRY 16

class ParticleApp {
public:
    ParticleApp();
    ~ParticleApp();

    bool initialize();
    bool initializeKinect();
    void initializeShaders();
    void init_gl() {}
    
    void shutdownKinect();
    
    void activate() {};
    void deactivate() {};
    
    void update();
    void updateKinect();
    
    void draw();
    void resize(int w, int h);
    
    void keyDown(int keyCode, int modifiers);
    void keyUp(int keyCode, int modifiers);
    
    void mouseDownAt(int x, int y);
    void mouseDraggedAt(int x, int y);
    void mouseUpAt(int x, int y);
    void mouseMovedAt(int x, int y);
    
    static std::string getFileContents(const char *filename);
    
    static ParamsPtr& params() {
        return m_params;
    }

    ShaderProgramPtr shaderProgram() { return m_shader_program; }
    
    Vec2 windowSize() { return m_window_size; }

private:
    bool m_initialized;

    ShaderProgramPtr m_shader_program;
    
    // high sample rate of mouse movements combined with integer-precision mouse positions create sharp particle movement vectors.  sampling older mouse positions helps with this.
    const int kCursorPositionVectorMaxLength = 4;
    list<Vec2> m_recentCursorPositions;
    
    Vec2 m_window_size;
    
    void addParticleAt(Vec2 position, Vec2 vector, ParticleController::ControlType type);

    std::list<PtrParticleController> particleControllers;

    float m_lastAppTime;
    float m_lastFrameTime;

    std::mutex g_buffer_mutex;
    
    static ParamsPtr m_params;

    PUI::PUIManager *m_ui_manager;

    PUI::PSlider *m_symmetry_slider;
    
#ifdef USE_KINECT
    Freenect::Freenect m_freenect;
    MyFreenectDevice *m_freenectDevice;
    static std::vector<uint16_t> m_depthBuffer;
    static std::vector<uint8_t> m_videoBuffer;
#endif
};


#endif /* defined(__ParticlesOSX__ParticleApp__) */
