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

#define USE_LEAP

#ifdef USE_LEAP
#include "MyLeap.h"
#endif

#include <ctime>
#include <array>


#define MAX_SYMMETRY 16

typedef unsigned long PKeyModifier;
const PKeyModifier KeyModifiers_CapsLock  = 1 << 16;
const PKeyModifier KeyModifiers_Shift     = 1 << 17;
const PKeyModifier KeyModifiers_Control   = 1 << 18;
const PKeyModifier KeyModifiers_Alt       = 1 << 19;
const PKeyModifier KeyModifiers_Command   = 1 << 20;
//const PKeyModifier KeyModifiers_NumPad    = 1 << 21;

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
    
    void keyDown(int keyCode, PKeyModifier modifiers);
    void keyUp(int keyCode, PKeyModifier modifiers);

    void mouseDownAt(int x, int y);
    void mouseDraggedAt(int x, int y);
    void mouseUpAt(int x, int y);
    void mouseMovedAt(int x, int y);
    
    const Vec2 mousePosition() const;
    const Vec2 canvasPosition() const;
    const Vec2 worldToView(const Vec2 &vec) const;
    static std::string getFileContents(const char *filename);
    
    PtrParticleController getActiveController();
    void setActiveController(int controllerIndex);
    PtrParticleController getKinectController();
    
    static ParamsPtr& params() {
        return m_params;
    }
    
    enum { kShaderParticles, kShaderSimple };
    static ShaderProgramPtr getShaderProgram(int program_index) { return m_shader_programs[program_index]; }
    
    const Vec2 windowSize() const { return m_window_size; }

private:
    bool m_initialized;

    static std::vector<ShaderProgramPtr> m_shader_programs;
    
    // high sample rate of mouse movements combined with integer-precision mouse positions create sharp particle movement vectors.  sampling older mouse positions helps with this.
    const int kCursorPositionVectorMaxLength = 4;
    list<Vec2> m_recentCursorPositions;
    
    Vec2 m_window_size;
    
    void addParticleAt(Vec2 position, Vec2 vector, ParticleController::ControlType type);

    const static int kNumControllers = 10;
    std::array<PtrParticleController, kNumControllers> m_particleControllers;
    PtrParticleController m_activeController;
    
    float m_lastAppTime;
    float m_lastFrameTime;

    std::mutex g_buffer_mutex;
    
    static ParamsPtr m_params;

    PUI::PGraphics *m_graphics;
    
#ifdef USE_KINECT
    Freenect::Freenect m_freenect;
    MyFreenectDevice *m_freenectDevice;
    static std::vector<uint16_t> m_depthBuffer;
    static std::vector<uint8_t> m_videoBuffer;
#endif
    
#ifdef USE_LEAP
    MyLeapListener m_leapListener;
    Leap::Controller m_leapController;
#endif
    
};


#endif /* defined(__ParticlesOSX__ParticleApp__) */
