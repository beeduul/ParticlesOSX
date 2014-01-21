//
//  ParticleController.cpp
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#include "ParticleController.h"
#include "ParticleApp.h"

#include "Rand.h"
#include "Color.h"
#include "Vec2.h"

#include <cmath>
#include <iostream>

std::string g_vertex_shader_source("        \n\
#version 330                                \n\
varying vec2 uv;                            \n\
layout(location = 1) in vec2 position;      \n\
layout(location = 2) in vec4 colorIn;       \n\
out vec4 color;                            \n\
void main() {                               \n\
    gl_Position = position;                 \n\
    color = colorIn;                        \n\
    //gl_TexCoord[0] = gl_MultiTexCoord0;     \n\
    //uv = vec2(gl_MultiTexCoord0);           \n\
}                                           \n\
");

std::string g_fragment_shader_source("                                          \n\
#version 330                                                                    \n\
//varying vec2 uv;                                                                \n\
uniform vec4 color;                                                             \n\
//uniform float radius;                                                           \n\
void                                                                            \n\
main() {                                                                        \n\
    //float border = 0.1;                                                         \n\
    //vec4 color0 = vec4(0.0, 0.0, 0.0, 0.0); // border                           \n\
    //vec2 m = uv - vec2(radius, radius);                                         \n\
    //float dist = radius - sqrt(m.x * m.x + m.y * m.y);                          \n\
    //float t = 0.0;                                                              \n\
    //if (dist > border)                                                          \n\
    //    t = 1.0;                                                                \n\
    //else if (dist > 0.0)                                                        \n\
    //    t = dist / border;                                                      \n\
    gl_FragColor = color; // mix(color0, color, t);                                       \n\
}                                                                               \n\
");

ParticleController::ParticleController(ParticleApp *appPtr) :
    m_isRecording(false),
    m_useGlobalParams(false),
    m_appPtr(appPtr)
{
    bool ok;
    VertexShader vertex_shader;
    ok = vertex_shader.initialize(g_vertex_shader_source);
    if (!ok) {
        cerr << "VERTEX SHADER ERROR" << endl;
    }

    FragmentShader fragment_shader;
    ok = fragment_shader.initialize(g_fragment_shader_source);
    if (!ok) {
        cerr << "FRAGMENT SHADER ERROR" << endl;
    }

    ok = m_shader_program.initialize(vertex_shader, fragment_shader);
    if (!ok) {
        cerr << "PROGRAM LINK ERROR" << endl;
    }

    m_shader_program.useProgram();
    
}

bool ParticleController::isRecording()
{
    return m_isRecording;
}

void ParticleController::startRecording()
{
    if (!m_isRecording) {
        m_isRecording = true;
        m_recording.clear();
        m_recordingBeganAt = Clock::getElapsedSeconds();
        cout << "startRecording, m_recordingBeganAt: " << m_recordingBeganAt << endl;
    }
}

void ParticleController::stopRecording()
{
    if (m_isRecording) {
        m_isRecording = false;
        m_lastPlaybackAt = Clock::getElapsedSeconds();
        m_recordingLength = Clock::getElapsedSeconds() - m_recordingBeganAt;
        m_playbackHead = m_recording.begin();
        cout << "stopRecording, m_recordingLength: " << m_recordingLength << ", # particles: " << m_recording.size() << endl;
    }
}

void ParticleController::update()
{
    if (!m_isRecording && m_recording.size() > 0) {
        bool playingBack = true;
        while (playingBack) {
            if (m_lastPlaybackAt + m_playbackHead->time < Clock::getElapsedSeconds()) {
                if (m_playbackHead->type == eMouseDown) {
                    Color birthColor(Rand::randFloat(), Rand::randFloat(), Rand::randFloat());
                    Color deathColor = Color(1 - birthColor.r(), 1 - birthColor.g(), 1 - birthColor.b());
                    getParams()->setColor("birthColor", birthColor);
                    getParams()->setColor("deathColor", deathColor);
                    
                }
                emitParticle(m_playbackHead->position, m_playbackHead->direction, getParams());
                m_playbackHead++;
                
                if (m_playbackHead == m_recording.end()) {
                    m_playbackHead = m_recording.begin();
                    m_lastPlaybackAt += m_recordingLength;
                }
                
            } else {
                playingBack = false;
            }
        }
    }
    
    // remove dead particles
    list<Particle *>::iterator dead = remove_if(m_particles.begin(), m_particles.end(), bind1st(mem_fun(&ParticleController::updateRemove), this));
    m_particles.erase(dead, m_particles.end());
    
}

bool ParticleController::updateRemove(Particle *p)
{
    bool removing = !p->update(*this);
    if (removing) {
        delete p;
    }
    return removing;
}

void ParticleController::draw()
{
    
    drawBuffers();
    
//    for( list<Particle *>::iterator p = m_particles.begin(); p != m_particles.end(); ++p ){
//        (*p)->draw();
//    }
}

void ParticleController::emitParticle(const Vec2 &position, const Vec2 &direction, ParamsPtr ptrParams)
{
    Vec2 size = m_appPtr->windowSize();
    Vec2 center = size / 2.0;
    
    Vec2 cPosition = position - center;
    
    float pAngle = std::atan2(cPosition.y(), cPosition.x());
    float dist = cPosition.length(); //sqrt(cPosition.x() * cPosition.x() + cPosition.y() * cPosition.y());
    
    float vAngle = std::atan2(direction.y(), direction.x());
    int symmetry = getParams()->geti("symmetry");
    float slice = M_PI * 2 / symmetry;
    
    for (int i = 0; i < symmetry; i++)
    {
        Vec2 newPos = Vec2(cos(pAngle), sin(pAngle)) * dist + center;
        Vec2 newDir = Vec2(cos(vAngle), sin(vAngle)) * direction.length();
        
        pAngle += slice;
        vAngle += slice;
        
        int num_particles = getParams()->geti("density");
        for (int i = 0; i < num_particles; i++) {
            m_particles.push_back(new Particle(newPos, newDir, ptrParams));
        }
    }
}

void ParticleController::addParticleAt(const Vec2 &position, const Vec2 &direction, ControlType type)
{
    emitParticle(position, direction, getParams());
    
    if (m_isRecording) {
        float timeSinceRecordingBegan = Clock::getElapsedSeconds() - m_recordingBeganAt;
        m_recording.push_back(Recording(position, direction, type, timeSinceRecordingBegan));
    }
}

void ParticleController::removeParticles( int amt )
{
    list<Particle *>::iterator p = m_particles.begin();
    while (p != m_particles.end() && amt > 0) {
        if ((*p)->stage() != Particle::Stage::dying) {
            (*p)->kill();
            amt--;
        }
        p++;
    }
}

void ParticleController::moveParticles(const Vec2 &offset)
{
    for(list<Particle *>::iterator p = m_particles.begin(); p != m_particles.end(); p++)
    {
        (*p)->loc((*p)->loc() + offset);
    }
}

void ParticleController::createBuffers()
{
    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.
    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };

    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, kMaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, kMaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
}

void ParticleController::updateBuffers()
{
    // fill the buffers that will be passed to OpenGL
    int i = 0;
    for(list<Particle *>::iterator p = m_particles.begin(); p != m_particles.end(); ++p ) {
        m_gpuPositionsArray[4 * i + 0] = (*p)->loc().x();
        m_gpuPositionsArray[4 * i + 1] = (*p)->loc().y();
        m_gpuPositionsArray[4 * i + 2] = 0;
        m_gpuPositionsArray[4 * i + 3] = (*p)->radius();
        
        m_gpuColorsArray[4 * i + 0] = (*p)->getColor().r();
        m_gpuColorsArray[4 * i + 1] = (*p)->getColor().g();
        m_gpuColorsArray[4 * i + 2] = (*p)->getColor().b();
        m_gpuColorsArray[4 * i + 3] = (*p)->getColor().a();
        
        i++;
    }

    // Update the buffers that OpenGL uses for rendering.
    // There are much more sophisticated means to stream data from the CPU to the GPU,
    // but this is outside the scope of this tutorial.
    // http://www.opengl.org/wiki/Buffer_Object_Streaming
    
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, kMaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, numParticles() * sizeof(GLfloat) * 4, &m_gpuPositionsArray[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, kMaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, numParticles() * sizeof(GLfloat) * 4, &m_gpuColorsArray[0]);
}

void ParticleController::drawBuffers()
{
    updateBuffers();
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(
                          0, // attribute. No particular reason for 0, but must match the layout in the shader.
                          3, // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0, // stride
                          (void*)0 // array buffer offset
                          );
    
    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(
                          1, // attribute. No particular reason for 1, but must match the layout in the shader.
                          4, // size : x + y + z + size => 4
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0, // stride
                          (void*)0 // array buffer offset
                          );
    
    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(
                          2, // attribute. No particular reason for 2, but must match the layout in the shader.
                          4, // size : r + g + b + a => 4
                          GL_FLOAT, // type
                          GL_FALSE, // normalized? *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
                          0, // stride
                          (void*)0 // array buffer offset
                          );
    
    // These functions are specific to glDrawArrays*Instanced*.
    // The first parameter is the attribute buffer we're talking about.
    // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
    // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center) -> 1
    glVertexAttribDivisor(2, 1); // color : one per quad -> 1
    
    // Draw the particules !
    // This draws many times a small triangle_strip (which looks like a quad).
    // This is equivalent to :
    // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
    // but faster.
    int num_particles = numParticles();
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, num_particles);
    
}
