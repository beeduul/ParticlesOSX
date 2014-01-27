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

#include <list>
#include <array>

class ParticleApp;

class ParticleController {
    
public:
    
    enum ControlType { eMouseDown, eMouseDrag, eMouseUp };
    
    ParticleController(ParticleApp *appPtr);
    void initialize();
    void update();
    void draw();
    
    void setParams(ParamsPtr ptrParams) { m_params = ptrParams; }
    ParamsPtr getParams() { return m_useGlobalParams ? Params::get() : m_params; }
    
    int numParticles() const { return (int) m_particles.size(); }
    
    void emitParticle(const Vec2 &position, const Vec2 &direction, ParamsPtr ptrParams);
    
    void addParticleAt(const Vec2 &position, const Vec2 &direction, ControlType type);
    //    void addParticles( int amt );
    void removeParticles( int amt );
    
    void moveParticles(const Vec2 &offset);
    
    void startRecording();
    void stopRecording();
    bool isRecording();
    
private:

    ParticleApp *m_appPtr;
    
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
    
    // optimized buffer management for particles

    static const int kMaxParticles = 200000;
    static const int kNumVerticesPerParticle = 6;
    static const int kNumVertexComponents = 2;
    static const int kNumColorComponents = 3;
    std::array<float, kMaxParticles * kNumVerticesPerParticle * kNumVertexComponents> m_gpuPositionsArray; // x, y

    std::array<float, kMaxParticles * kNumVerticesPerParticle * kNumColorComponents> m_gpuColorsArray;
    
    GLuint m_vaoID;
    
    GLuint billboard_vertex_buffer;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;
    void createBuffers();
    void drawBuffers();
    
    
    // testing opengl
    // This will identify our vertex buffer
    GLuint vertexbuffer;

    Vec2 m_p;
    
};

typedef std::shared_ptr<ParticleController> PtrParticleController;

#endif /* defined(__ParticlesOSX__ParticleController__) */
