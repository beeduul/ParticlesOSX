//
//  Particle.cpp
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#include "Rand.h"
#include "Color.h"
#include "Particle.h"

#include <cmath>

std::vector<GLfloat *> Particle::m_vec_circle_verts;

Particle::Particle(const Vec2 &location, const Vec2 &direction, ParamsPtr ptrParams)
{
    initialize(
               location,
               direction,
               Rand::randFloat(1),
               ptrParams
               );
}
Particle::~Particle() {
}

void Particle::kill()
{
    m_stage = Stage::dying;
    m_stage_started_at = Clock::getElapsedSeconds();
}

void Particle::update_lifecycle()
{
    if (m_stage != dead && stage_time() > m_stage_durations[m_stage]) {
        m_stage_started_at += m_stage_durations[m_stage];
        m_stage = (Stage) (m_stage + 1);
    }
}

bool Particle::update(const ParticleController &pc)
{
    update_lifecycle();
    update_behavior(pc);
    return m_stage != dead;
}

void Particle::initialize(const Vec2 &location, const Vec2 &direction, float velocity, ParamsPtr ptrParams)
{
    m_stage = Stage::birth;
    m_stage_started_at = Clock::getElapsedSeconds();
    
    m_loc = location;
    m_vec = direction;
    m_vel = velocity;
    
    m_initial_radius = ptrParams->getf("size");
    
    Color birth_color = ptrParams->getColor("birthColor") + Color(Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1));
    Color death_color = ptrParams->getColor("deathColor") + Color(Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1), Rand::randFloat(-.1, .1));
    
    m_birth_color = birth_color;
    m_death_color = death_color;
    
    stage_duration(birth, ptrParams->getf("lifespan") * .2);
    stage_duration(alive, ptrParams->getf("lifespan") * .6);
    stage_duration(dying, ptrParams->getf("lifespan") * .2);
    
    m_decay = Rand::randFloat(0.95, 0.99);
    
    m_amp = Rand::randFloat(0.9, 1.1) * ptrParams->getf("pulse_amplitude");
    m_freq = ptrParams->getf("pulse_rate"); // Rand::randFloat(0, 10);
    m_start = Clock::getElapsedSeconds(); // Rand::randFloat(0, M_PI);
    
    m_drawStyle = ptrParams->geti("draw_style");
    
    if (ptrParams->geti("symmetry") == 1) {
        m_gravity = Vec2(0, 1);
    } else {
//        Vec2 center(ci::app::getWindowWidth()/2, ci::app::getWindowHeight()/2);
//        m_gravity = Vec2(center - m_loc).normalized();
        m_gravity = Vec2(0, 1);
    }
    float gravity = ptrParams->getf("gravity");
    m_gravity *= gravity;
    
}

void Particle::update_behavior(const ParticleController &pc)
{
    m_loc += m_vec * m_vel;
    m_vel *= m_decay;
    m_vec += m_gravity;

    float t = stage_time() / stage_duration();
    
    float t2 = m_freq * (Clock::getElapsedSeconds() - m_start);
    float swell = (1 + sin(t2)) * m_amp;
    
    m_current_radius = m_initial_radius * swell;
    switch(stage()) {
        case birth: {
            m_current_radius = m_current_radius * t; // easeOutBack(t); // easeInElastic(t, 5, 3);
            break;
        }
        case alive: {
            //            radius = radius;
            break;
        }
        case dying: {
            m_current_radius = m_current_radius * (1 - t); // (1 - easeOutBack(t)); // easeInElastic(t, 5, 3);
            break;
        }
        case dead: ; // no-op
    }
    

    
}

Color Particle::getColor()
{
    float t = stage_time() / stage_duration();
    return Color(stage(), t);
}

Color Particle::getColor(Stage stage, float t)
{
    switch(stage) {
        case birth: {
            Color c = m_birth_color;
            //            c.a = t;
            return c;
        }
        case alive: {
            return m_birth_color * (1-t) + m_death_color * t;
        }
        case dying: {
            return m_death_color * (1 - t);
        }
        case dead:
            return Color(0);
    }
    
}

//void Particle::draw()
//{
//    if (stage() == dead) return;
//    
//    float t = stage_time() / stage_duration();
//    
////    glColor4f(Color(stage(), t));
//// TODO    gl::color(Color(stage(), t));
//    
//    float t2 = m_freq * (Clock::getElapsedSeconds() - m_start);
//    float swell = (1 + sin(t2)) * m_amp;
//    
//    float radius = m_radius * swell;
//    switch(stage()) {
//        case birth: {
//            radius = radius * t; // easeOutBack(t); // easeInElastic(t, 5, 3);
//            break;
//        }
//        case alive: {
//            //            radius = radius;
//            break;
//        }
//        case dying: {
//            radius = radius * (1 - t); // (1 - easeOutBack(t)); // easeInElastic(t, 5, 3);
//            break;
//        }
//        case dead: ; // no-op
//    }
//    
//    switch (m_drawStyle) {
//        drawSolidCircle(loc(), radius);
//        case 0:
//            //drawStrokedCircle( loc(), radius );
////            drawShaderCircle(loc(), radius);
//            break;
//        case 1:
////            drawSolidCircleVBO( loc(), radius );
//            break;
//        case 2:
////            gl::drawSolidCircle(loc(), radius);
//            break;
//        case 3:
////            drawSolidCircle(loc(), radius);
//            break;
//        case 4:
////            gl::drawStrokedRect(Rectf(loc().x - radius, loc().y - radius, loc().x + radius, loc().y + radius));
//            break;
//        case 5:
//        default:
////            gl::drawSolidRect(Rectf(loc().x - radius, loc().y - radius, loc().x + radius, loc().y + radius));
//            break;
//    }
//}
//
//void Particle::drawStrokedCircle(const Vec2 &center, float radius)
//{
//    int numSegments = radiusToSegments(radius);
//    
//    GLfloat *verts = getCircleVerts(radius);
//    
//}
//
//void Particle::drawSolidCircle(const Vec2 &center, float radius)
//{
//    int numSegments = radiusToSegments(radius);
//    
//    GLfloat *verts = getCircleVerts(radius);
//    
////    glPushMatrix();
//    
////    glTranslatef(center.x, center.y, 0);
////    glScalef(radius, radius, 1);
//    
////	glEnableClientState( GL_VERTEX_ARRAY );
////	glVertexPointer( 2, GL_FLOAT, 0, verts );
////	glDrawArrays( GL_TRIANGLE_FAN, 0, numSegments + 2 );
////	glDisableClientState( GL_VERTEX_ARRAY );
//    
////    glPopMatrix();
//}

int Particle::radiusToSegments(float radius)
{
    return (int)floor( radius * M_PI * 2 );
}

GLfloat * Particle::getCircleVerts(float radius)
{
    int numSegments = radiusToSegments(radius);
    if( numSegments < 2 ) numSegments = 2;
    if( numSegments > 250 ) numSegments = 250;
    
    if (numSegments >= m_vec_circle_verts.size()) {
        m_vec_circle_verts.resize(numSegments, 0);
    }
    
    int index = numSegments-1;
    if (m_vec_circle_verts[index] == 0)
    {
        GLfloat *verts = new float[(numSegments+2)*2];
        verts[0] = 0;
        verts[1] = 0;
        for( int s = 0; s <= numSegments; s++ ) {
            float t = s / (float)numSegments * 2.0f * 3.14159f;
            verts[(s+1)*2+0] = cos( t );
            verts[(s+1)*2+1] = sin( t );
        }
        m_vec_circle_verts[index] = verts;
    }
    
    return m_vec_circle_verts[index];
}
