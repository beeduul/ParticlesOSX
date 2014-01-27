//
//  ParticleController.cpp
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#include "ParticleController.h"
#include "ParticleApp.h"

#include <cmath>
#include <iostream>

static const GLfloat g_vertex_buffer_data[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f,
    0.5f, 0.5f, 0.0f,
};

ParticleController::ParticleController(ParticleApp *appPtr) :
    m_isRecording(false),
    m_useGlobalParams(false),
    m_appPtr(appPtr)
{
}

void ParticleController::initialize()
{
    int step = 0;
    cout << "ParticleController " << step++ << endl;;
    GetGLError();
    
    createBuffers();
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

//    cout << "# particles: " << m_particles.size() << endl;
    
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
}

void ParticleController::emitParticle(const Vec2 &position, const Vec2 &direction, ParamsPtr ptrParams)
{
    if (false)
    {
        int numToAdd = 10;
        if (m_particles.size() + numToAdd < kMaxParticles) {
            for (int i = 0; i < numToAdd; i++) {
                m_particles.push_back(new Particle(position, direction, ptrParams));
            }
        }

    } else {
        Vec2 size = m_appPtr->windowSize();
        Vec2 center = Vec2(0, 0); // size / 2.0;
        
        Vec2 cPosition = position - center;
        
        float pAngle = std::atan2(cPosition.y(), cPosition.x());
        float dist = cPosition.length(); //sqrt(cPosition.x() * cPosition.x() + cPosition.y() * cPosition.y());
        
        float vAngle = std::atan2(direction.y(), direction.x());
        ParamsPtr p = getParams();
        int symmetry = p->geti("symmetry");
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
}

void ParticleController::addParticleAt(const Vec2 &position, const Vec2 &direction, ControlType type)
{
    m_p = position;
    
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

    // The VBO containing the positions of the particles
    int positionBufferSize = kMaxParticles * kNumVerticesPerParticle * kNumVertexComponents * sizeof(GLfloat);
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, positionBufferSize, NULL, GL_STREAM_DRAW);

    GetGLError();

    // The VBO containing the colors of the particles
    int colorBufferSize = kMaxParticles * kNumVerticesPerParticle * kNumColorComponents * sizeof(GLfloat);
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, colorBufferSize, NULL, GL_STREAM_DRAW);

    GetGLError();

    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    GetGLError();

    glEnableVertexAttribArray(0); // vertex position
    glEnableVertexAttribArray(1); // vertex color

    GetGLError();

    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(0, kNumVertexComponents, GL_FLOAT, GL_FALSE, 0, NULL);

    GetGLError();

    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(1, kNumColorComponents, GL_FLOAT, GL_FALSE, 0, NULL);

    GetGLError();

}

void ParticleController::drawBuffers()
{
    int nParticles = numParticles();
    
    int pIdx = 0;
    if (true) {
        for(list<Particle *>::iterator p = m_particles.begin(); p != m_particles.end(); p++)
        {
            float cx = (*p)->loc().x();
            float cy = (*p)->loc().y();
            float radius = (*p)->radius();

            int pIdxBase = pIdx * kNumVerticesPerParticle * kNumVertexComponents;
            int vIdxBase;
            
            // tri 0
            // vertex 0
            vIdxBase = pIdxBase + kNumVertexComponents * 0;
            m_gpuPositionsArray[vIdxBase + 0] = cx - radius;
            m_gpuPositionsArray[vIdxBase + 1] = cy - radius;

            // vertex 1
            vIdxBase = pIdxBase + kNumVertexComponents * 1;
            m_gpuPositionsArray[vIdxBase + 0] = cx - radius;
            m_gpuPositionsArray[vIdxBase + 1] = cy + radius;

            // vertex 2
            vIdxBase = pIdxBase + kNumVertexComponents * 2;
            m_gpuPositionsArray[vIdxBase + 0] = cx + radius;
            m_gpuPositionsArray[vIdxBase + 1] = cy + radius;

            // tri 1
            // vertex 3 (repeat v2)
            vIdxBase = pIdxBase + kNumVertexComponents * 3;
            m_gpuPositionsArray[vIdxBase + 0] = cx + radius;
            m_gpuPositionsArray[vIdxBase + 1] = cy + radius;
            
            // vertex 4
            vIdxBase = pIdxBase + kNumVertexComponents * 4;
            m_gpuPositionsArray[vIdxBase + 0] = cx + radius;
            m_gpuPositionsArray[vIdxBase + 1] = cy - radius;

            // vertex 5 (repeat v0)
            vIdxBase = pIdxBase + kNumVertexComponents * 5;
            m_gpuPositionsArray[vIdxBase + 0] = cx - radius;
            m_gpuPositionsArray[vIdxBase + 1] = cy - radius;

            
            int pColorIndexBase = pIdx * kNumVerticesPerParticle * kNumColorComponents;
            const Color c = (*p)->getColor();
            for (int i = 0; i < kNumVerticesPerParticle; i++) {
                int cIdxBase = pColorIndexBase + kNumColorComponents * i;
                m_gpuColorsArray[cIdxBase + 0] = c.r();
                m_gpuColorsArray[cIdxBase + 1] = c.g();
                m_gpuColorsArray[cIdxBase + 2] = c.b();
            }

            
            pIdx++;
        }

        assert(nParticles == pIdx);
    } else {
        nParticles = 1;
        int i = 0;
        m_gpuPositionsArray[i++] = m_p.x() - 10; m_gpuPositionsArray[i++] = m_p.y() - 10;
        m_gpuPositionsArray[i++] = m_p.x() - 10; m_gpuPositionsArray[i++] = m_p.y() + 10;
        m_gpuPositionsArray[i++] = m_p.x() + 10; m_gpuPositionsArray[i++] = m_p.y() + 10;
        m_gpuPositionsArray[i++] = m_p.x() + 10; m_gpuPositionsArray[i++] = m_p.y() + 10; // v2
        m_gpuPositionsArray[i++] = m_p.x() + 10; m_gpuPositionsArray[i++] = m_p.y() - 10;
        m_gpuPositionsArray[i++] = m_p.x() - 10; m_gpuPositionsArray[i++] = m_p.y() - 10; // v0
        
        for (int i = 0; i < kNumVerticesPerParticle * kNumColorComponents; i += kNumColorComponents) {
            m_gpuColorsArray[i+0] = 1; m_gpuColorsArray[i+1] = 0; m_gpuColorsArray[i+2] = 0;
        }
    }
    
    glBindVertexArray(m_vaoID);
    GetGLError();

    int positionBufferSize = kMaxParticles * kNumVerticesPerParticle * kNumVertexComponents * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positionBufferSize, &m_gpuPositionsArray[0], GL_STREAM_DRAW);
    GetGLError();

    int colorBufferSize = kMaxParticles * kNumVerticesPerParticle * kNumColorComponents * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, colorBufferSize, &m_gpuColorsArray[0], GL_STREAM_DRAW);
    GetGLError();

    GLint location = glGetUniformLocation(m_appPtr->shaderProgram()->id(), "viewSize");
    GetGLError();
    if (location >= 0) {
        Vec2 windowSize = m_appPtr->windowSize();
        glUniform2i(location, (int) windowSize.x(), (int) windowSize.y());
        GetGLError();
    } else {
        std::cerr << "Couldn't find uniform viewSize" << std::endl;
    }
    
    glDrawArrays(GL_TRIANGLES, 0, nParticles * kNumVerticesPerParticle);
    GetGLError();
}
