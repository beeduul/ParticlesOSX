//
//  Particle.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef __ParticlesOSX__Particle__
#define __ParticlesOSX__Particle__

#include "Params.h"

#import "MyGLUtilities.h"

#include <vector>

class ParticleController;

class Particle {
public:

    Particle(const Vec2 &loc, const Vec2 &vec, const ParticleController &pc);
    virtual ~Particle();

    virtual void kill();
    virtual bool update(const ParticleController &pc);
    virtual void update_behavior(const ParticleController &pc);
//    virtual void draw();

    enum Stage { birth, alive, dying, dead };

    const Stage stage() { return m_stage; }
    const Stage stage(const Stage new_stage) { return m_stage = new_stage; }

    float stage_time() { return Clock::getElapsedSeconds() - m_stage_started_at; }
    float stage_duration() { return m_stage_durations[m_stage]; }
    float stage_duration(Stage stage, float stage_duration) { return m_stage_durations[stage] = stage_duration; }

    const float& stage_started_at() { return m_stage_started_at; }
    const float& stage_started_at(const float& new_stage_started_at) { return m_stage_started_at = new_stage_started_at; }

    Color getColor();

    const float radius() { return m_current_radius; }
//    const float& radius(const float& new_radius) { return m_radius = new_radius; }
    
    const Vec2& loc() { return m_loc; }
    const Vec2& loc(const Vec2& new_loc) { return m_loc = new_loc; }
    
    const Vec2& vec() { return m_vec; }
    const Vec2& vec(const Vec2& new_vec) { return m_vec = new_vec; }

private:
    // lifecycle
    Stage m_stage;
    float m_stage_durations[4];
    float m_stage_started_at;

    // methods
    void initialize(const Vec2 &location, const Vec2 &direction, float velocity, const ParticleController &pc);
    void update_lifecycle();
    Color getColor(Stage stage, float t);

    // characteristics
    Vec2 m_loc;
    Vec2 m_vec;
    float m_vel;
    float m_initial_radius;
    float m_current_radius;
    float m_decay;
    Color m_birth_color;
    Color m_death_color;
    float m_amp;
    float m_freq;
    float m_start;
    int m_drawStyle;
    Vec2 m_gravity;

    // optimizing draw calls
    static int radiusToSegments(float radius);

    // precomputed gl arrays
    static std::vector<GLfloat *> m_vec_circle_verts;
    static GLfloat *getCircleVerts(float radius);
//    void drawSolidCircle(const Vec2 &center, float radius);
//    void drawStrokedCircle(const Vec2 &center, float radius);

    
};


#endif /* defined(__ParticlesOSX__Particle__) */
