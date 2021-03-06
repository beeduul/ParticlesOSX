//
//  ParticleController.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef __ParticlesOSX__ParticleController__
#define __ParticlesOSX__ParticleController__

#include "Particle.h"
#include "Params.h"
#include "ShaderProgram.h"

#include "PUI.h"

#include <list>
#include <array>
#include <mutex>

class ParticleApp;

class ParticleController : public PUI::IDelegate {
    
public:
    
    enum ControlType { eMouseDown, eMouseDrag, eMouseUp };
    
    ParticleController(ParticleApp *appPtr);
    ~ParticleController();
    void initialize(bool kinect);
    void update();
    void draw(PUI::PGraphics &graphics);
    void drawUI(PUI::PGraphics &graphics);
    
    bool mouseDown(PUI::MouseEvent &event);
    bool mouseDrag(PUI::MouseEvent &event, Vec2 &mouse_vec);
    bool mouseUp(PUI::MouseEvent &event);
    void mouseMove(PUI::MouseEvent &event);

    void keyDown(int keyCode, int modifiers);
    
    void setParams(ParamsPtr ptrParams) { m_params = ptrParams; }
    ParamsPtr getParams() const { return m_useGlobalParams ? Params::get() : m_params; }
    
    const Vec2 getSymmetryPoint() const { return m_symmetry_center; }
    
    int numParticles() const { return (int) m_particles.size(); }
    
    void emitParticle(const Vec2 &position, const Vec2 &direction);
    void emitParticle(const Vec2 &position, const Vec2 &direction, ParamsPtr ptrParams);
    
    void addParticleAt(const Vec2 &position, const Vec2 &direction, ControlType type);
    //    void addParticles( int amt );
    void removeParticles( int amt );
    
    void moveParticles(const Vec2 &offset);
    
    void startRecording();
    void stopRecording();
    bool isRecording();
    
    bool isKinect() { return m_kinect; }
    
    // IDelegate implementation
    virtual void controlCallback(const PUI::PControl* control);
    
    
private:

    ParticleApp *m_appPtr;
    
    mutex m_mutex;
    
    struct Recording {
        Recording(Vec2 p, Vec2 d, ControlType ty, float t) {
            position = p;
            direction = d;
            type = ty;
            time = t;
        }
        Vec2 position;
        Vec2 direction;
        ControlType type;
        float time;
    };
    
    bool m_isRecording;
    float m_recordingBeganAt;
    float m_recordingLength;
    float m_lastPlaybackAt;
    std::list<Recording> m_recording;
    std::list<Recording>::iterator m_playbackHead;
    
    bool m_useGlobalParams;
    ParamsPtr m_params;
    
    std::list<Particle *> m_particles;
    
    bool updateRemove(Particle *p);
    
    bool m_kinect;
    
    PUI::PUIManager *m_ui_manager;
    PUI::PSlider *m_symmetry_slider;
    Vec2 m_symmetry_center;
    void initializeUI();
    
    // optimized buffer management for particles

    static const int kMaxParticles = 200000;
    static const int kNumVerticesPerParticle = 6; // 2 triangles, 3 vertices each
    static const int kNumVertexComponents = 2; // x, y
    static const int kNumColorComponents = 3;
    std::array<float, kMaxParticles * kNumVerticesPerParticle * kNumVertexComponents> m_gpuPositionsArray; // x, y
    std::array<float, kMaxParticles * kNumVerticesPerParticle * kNumColorComponents> m_gpuColorsArray; // r,g,b
//    std::array<float, kMaxParticles * kNumVerticesPerParticle * kNumVertexComponents> m_gpu_ParticleCentersArray;
    std::array<float, kMaxParticles * kNumVerticesPerParticle * kNumVertexComponents> m_gpu_ParticleUVsArray; // u, v
    GLuint m_vao_id;
    
    GLuint billboard_vertex_buffer;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;
//    GLuint particles_center_buffer;
    GLuint particles_uv_buffer;
    void createBuffers();
    void destroyBuffers();
    void drawBuffers();
    
    
    // testing opengl
    // This will identify our vertex buffer
    GLuint vertexbuffer;

    Vec2 m_p;
    
};

typedef std::shared_ptr<ParticleController> PtrParticleController;

#endif /* defined(__ParticlesOSX__ParticleController__) */
