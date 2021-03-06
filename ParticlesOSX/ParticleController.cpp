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

#include <boost/thread/lock_guard.hpp>

//static const GLfloat g_vertex_buffer_data[] = {
//    -0.5f, -0.5f, 0.0f,
//    0.5f, -0.5f, 0.0f,
//    -0.5f, 0.5f, 0.0f,
//    0.5f, 0.5f, 0.0f,
//};

ParticleController::ParticleController(ParticleApp *appPtr) :
    m_isRecording(false),
    m_useGlobalParams(false),
    m_appPtr(appPtr)
{
}

ParticleController::~ParticleController()
{
    delete m_ui_manager;
    destroyBuffers();
}

void ParticleController::initialize(bool kinect)
{
    int step = 0;
    cout << "ParticleController " << step++ << endl;;
    GetGLError();
    
    m_kinect = kinect;
    
    m_params = ParamsPtr(new Params());
    m_symmetry_center = Vec2(0, 0);

    initializeUI();
    
    createBuffers();
}

void ParticleController::initializeUI()
{
    m_ui_manager = new PUI::PUIManager;
    
    int slider_height = 15;
    int slider_width = 100;
    
    int control_x = 4;
    
    int control_spacing_y = 5;
    int control_y = control_spacing_y;
    
    {
        Color fillColor = Color(255/255.0, 99/255.0, 71/255.0);
        PUI::PSlider *pSlider = new PUI::PSlider("size", 1, 100, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
        pSlider->setValue(m_params->getf("size"));
        pSlider->setColors(fillColor, Color::DkGray);
        pSlider->setDelegate(this);
        m_ui_manager->addControl(pSlider);
        control_y += control_spacing_y + slider_height;
    }
    
    {
        Color fillColor = Color(152/255.0, 251/255.0, 152/255.0);
        PUI::PSlider *pSlider = new PUI::PSlider("lifespan", 1, 30, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
        pSlider->setValue(m_params->getf("lifespan"));
        pSlider->setColors(fillColor, Color::DkGray);
        pSlider->setDelegate(this);
        m_ui_manager->addControl(pSlider);
        control_y += control_spacing_y + slider_height;
    }
    
    {
        Color fillColor = Color(255/255.0, 218/255.0, 185/255.0);
        PUI::PSlider *pSlider = new PUI::PSlider("density", -10, 20, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
        pSlider->setValue(m_params->geti("density"));
        pSlider->setColors(fillColor, Color::DkGray);
        pSlider->setDelegate(this);
        m_ui_manager->addControl(pSlider);
        control_y += control_spacing_y + slider_height;
    }
    control_y += control_spacing_y;
    
    {
        Color fillColor = Color(0/255.0, 206/255.0, 209/255.0);
        PUI::PSlider *pSlider = new PUI::PSlider("gravity", -.5, .5, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
        pSlider->setValue(m_params->getf("gravity"));
        pSlider->setColors(fillColor, Color::DkGray);
        pSlider->setDelegate(this);
        m_ui_manager->addControl(pSlider);
        control_y += control_spacing_y + slider_height;
    }
    
    {
        Color fillColor = Color(220/255.0, 20/255.0, 60/255.0);
        PUI::PSlider *pSlider = new PUI::PSlider("symmetry", 1, MAX_SYMMETRY, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
        pSlider->setValue(m_params->geti("symmetry"));
        pSlider->setColors(fillColor, Color::DkGray);
        pSlider->setDelegate(this);
        m_ui_manager->addControl(pSlider);
        control_y += control_spacing_y + slider_height;
        
        m_symmetry_slider = pSlider;
    }
    control_y += control_spacing_y;
    
    
    {
        Color fillColor = Color(255/255.0, 228/255.0, 225/255.0);
        PUI::PSlider *pSlider = new PUI::PSlider("pulse_rate", 0, 5, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
        pSlider->setValue(m_params->getf("pulse_rate"));
        pSlider->setColors(fillColor, Color::DkGray);
        pSlider->setDelegate(this);
        m_ui_manager->addControl(pSlider);
        control_y += control_spacing_y + slider_height;
    }
    
    {
        Color fillColor = Color(255/255.0, 250/255.0, 250/255.0);
        PUI::PSlider *pSlider = new PUI::PSlider("noise", 0, 1, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
        pSlider->setValue(m_params->getf("noise"));
        pSlider->setColors(fillColor, Color::DkGray);
        pSlider->setDelegate(this);
        m_ui_manager->addControl(pSlider);
        control_y += control_spacing_y + slider_height;
    }
    
    {
        Color fillColor = Color(30/255.0, 150/255.0, 250/255.0);
        PUI::PSlider *pSlider = new PUI::PSlider("jitter", 0, 4, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
        pSlider->setValue(m_params->getf("jitter"));
        pSlider->setColors(fillColor, Color::DkGray);
        pSlider->setDelegate(this);
        m_ui_manager->addControl(pSlider);
        control_y += control_spacing_y + slider_height;
    }
    
    //        {
    //            Color fillColor = Color(255/255.0, 239/255.0, 213/255.0);
    //            PUI::PSlider *pSlider = new PUI::PSlider("draw_style", 0, 2, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
    //            pSlider->setValue(m_params->geti("draw_style"));
    //            pSlider->setColors(fillColor, Color::DkGray);
    //            pSlider->setDelegate(this);
    //            m_ui_manager->addControl(pSlider);
    //            control_y += control_spacing_y + slider_height;
    //        }
    //
    //        {
    //            PUI::PColorWell *pColorWell = new PUI::PColorWell("colorwell", Color::Black, Color::White, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
    ////            pColorWell->setDelegate
    //            m_ui_manager->addControl(pColorWell);
    //            control_y += control_spacing_y + slider_height;
    //        }
    
    control_y += control_spacing_y * 2;
    
    if (m_kinect)
    {
        {
            Color fillColor = Color::White;
            PUI::PSlider *pSlider = new PUI::PSlider("kthreshold", 0, 2047, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
            pSlider->setValue(m_params->geti("kthreshold"));
            pSlider->setColors(fillColor, Color::DkGray);
            pSlider->setDelegate(this);
            m_ui_manager->addControl(pSlider);
            control_y += control_spacing_y + slider_height;
        }
        
        {
            Color fillColor = Color(255/255.0, 218/255.0, 185/255.0);
            PUI::PSlider *pSlider = new PUI::PSlider("kdensity", -50, 1, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
            pSlider->setValue(m_params->geti("kdensity"));
            pSlider->setColors(fillColor, Color::DkGray);
            pSlider->setDelegate(this);
            m_ui_manager->addControl(pSlider);
            control_y += control_spacing_y + slider_height;
        }
        
        {
            Color fillColor = Color(152/255.0, 251/255.0, 152/255.0);
            PUI::PSlider *pSlider = new PUI::PSlider("klifespan", .1, 10, PUI::Rect(Vec2(control_x, control_y), PSize(slider_width, slider_height)));
            pSlider->setValue(m_params->getf("klifespan"));
            pSlider->setColors(fillColor, Color::DkGray);
            pSlider->setDelegate(this);
            m_ui_manager->addControl(pSlider);
            control_y += control_spacing_y + slider_height;
        }
    }
    
    
    Color birthColor(Rand::randFloat(), Rand::randFloat(), Rand::randFloat());
    Color deathColor = Color(1 - birthColor.r(), 1 - birthColor.g(), 1 - birthColor.b());
    m_params->setColor("birthColor", birthColor);
    m_params->setColor("deathColor", deathColor);

}

void ParticleController::controlCallback(const PUI::PControl* control)
{
    string name = control->getName();
    
//    if(name == ">>" || name == "<<")
//    {
//        m_toggleMenu = true;
//    } else
    if (name == "pulse_rate") {
        m_params->setf(name, ((PUI::PSlider *) control)->getValue());
//    } else if (name == "pulse_amplitude") {
//        ciUISlider *slider = (ciUISlider *) event->widget;
//        m_params->setf("pulse_amplitude", slider->getScaledValue());
//        
    } else if (name == "gravity") {
        float value = ((PUI::PSlider *) control)->getValue();
        m_params->setf(name, value);
        cout << "controlCallback '" << name << "'" << " value: " << value << endl;
    } else if (name == "size") {
        float value = ((PUI::PSlider *) control)->getValue();
        m_params->setf(name, value);
        cout << "controlCallback '" << name << "'" << " value: " << value << endl;
    } else if (name == "lifespan") {
        float value = ((PUI::PSlider *) control)->getValue();
        m_params->setf(name, value);
        cout << "controlCallback '" << name << "'" << " value: " << value << endl;
    } else if (name == "density") {
        int value = ((PUI::PSlider *) control)->getValue();
        m_params->seti(name, value);
        cout << "controlCallback '" << name << "'" << " value: " << value << endl;
    } else if (name == "symmetry") {
        int value = ((PUI::PSlider *) control)->getValue();
        m_params->seti(name, value);
        cout << "controlCallback '" << name << "'" << " value: " << value << endl;
    } else if (name == "draw_style") {
        int value = ((PUI::PSlider *) control)->getValue();
        m_params->seti(name, value);
        cout << "controlCallback '" << name << "'" << " value: " << value << endl;
    } else if (name == "noise") {
        float value = ((PUI::PSlider *) control)->getValue();
        m_params->setf(name, value);
        cout << "controlCallback '" << name << "'" << " value: " << value << endl;
    } else if (name == "jitter") {
        float value = ((PUI::PSlider *) control)->getValue();
        m_params->setf(name, value);
        cout << "controlCallback '" << name << "'" << " value: " << value << endl;
    } else if (name == "kthreshold") {
        m_params->seti(name, ((PUI::PSlider *) control)->getValue());
    } else if (name == "kdensity") {
        int value = ((PUI::PSlider *) control)->getValue();
        m_params->seti(name, value);
        cout << "controlCallback '" << name << "'" << " value: " << value << endl;
    } else if (name == "klifespan") {
        float value = ((PUI::PSlider *) control)->getValue();
        m_params->setf(name, value);
        cout << "controlCallback '" << name << "'" << " value: " << value << endl;
    }
    
}

bool ParticleController::mouseDown(PUI::MouseEvent &event)
{
    bool down = m_ui_manager->mouseDown(event);
    if (!down) {
        Color birthColor(Rand::randFloat(), Rand::randFloat(), Rand::randFloat());
        Color deathColor = Color(1 - birthColor.r(), 1 - birthColor.g(), 1 - birthColor.b());
        m_params->setColor("birthColor", birthColor);
        m_params->setColor("deathColor", deathColor);
        
        setParams(m_params);
        
        Vec2 canvasPosition(m_appPtr->worldToView(event.getPoint()));
        addParticleAt(canvasPosition, Vec2(0, 0), ParticleController::eMouseDown);
    }
    return down;
}

bool ParticleController::mouseDrag(PUI::MouseEvent &event, Vec2 &mouse_vec)
{
    bool down = m_ui_manager->mouseDrag(event);
    if (!down)
    {
        Vec2 canvasPosition(m_appPtr->worldToView(event.getPoint()));
        addParticleAt(canvasPosition, mouse_vec, ParticleController::eMouseDrag);
    }
    return down;
}

bool ParticleController::mouseUp(PUI::MouseEvent &event)
{
    return m_ui_manager->mouseUp(event);
}

void ParticleController::mouseMove(PUI::MouseEvent &event)
{
    return m_ui_manager->mouseMove(event);
}


void ParticleController::keyDown(int keyCode, int modifiers)
{
    switch (keyCode) {
        case 'c': {
//            if (modifiers && CONTROLKEY) {
//                removeParticles(particleController->numParticles())
//            }
//            else
            {
                Color birthColor(Rand::randFloat(), Rand::randFloat(), Rand::randFloat());
                Color deathColor = Color(1 - birthColor.r(), 1 - birthColor.g(), 1 - birthColor.b());
                
                m_params->setColor("kinectBirthColor", birthColor);
                m_params->setColor("kinectDeathColor", deathColor);
            }
        }
            
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
            if (modifiers == KeyModifiers_Control) {
                m_symmetry_center = m_appPtr->canvasPosition();
            } else {
            int symmetry = m_params->geti("symmetry");
            symmetry++;
            if (symmetry > MAX_SYMMETRY) {
                symmetry = 1;
            }
            m_params->seti("symmetry", symmetry);
            m_symmetry_slider->setValue(symmetry);
            }
        } break;
            
        case 'r': {
            startRecording();
        } break;
    }

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
    lock_guard<mutex> guard(m_mutex);
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

void ParticleController::draw(PUI::PGraphics &graphics)
{
    drawBuffers();
}

void ParticleController::drawUI(PUI::PGraphics &graphics)
{
    m_ui_manager->draw(graphics);
}

void ParticleController::emitParticle(const Vec2 &position, const Vec2 &direction)
{
    emitParticle(position, direction, getParams());
}

void ParticleController::emitParticle(const Vec2 &position, const Vec2 &direction, ParamsPtr ptrParams)
{
    Vec2 p = position - m_symmetry_center;
    float pAngle = std::atan2(p.y(), p.x());
    float dist = p.length();
    
    float vAngle = std::atan2(direction.y(), direction.x());
    int symmetry = getParams()->geti("symmetry");
    float slice = M_PI * 2 / symmetry;

    int num_particles = 1;
    bool renderParticle = true;
    int density = getParams()->geti("density");

    for (int i = 0; i < symmetry; i++)
    {
        if (density > 0) {
            num_particles = density;
        } else {
            float chance = 1.0 / -(density - 1);
            float roll = Rand::randFloat();
            renderParticle = roll < chance;
//            cout << "chance: " << chance << ", roll: " << roll << ", render: " << renderParticle << endl;
        }
        if (renderParticle) {
            float cx = std::cos(pAngle);
            float sx = std::sin(pAngle);
            Vec2 newPos = Vec2(cx, sx);
            newPos *= dist;
            newPos += m_symmetry_center;
            
            Vec2 newDir = Vec2(cos(vAngle), sin(vAngle)) * direction.length();
            
            pAngle += slice;
            vAngle += slice;
            
            lock_guard<mutex> guard(m_mutex);
            if (m_particles.size() + num_particles < kMaxParticles) {
                for (int i = 0; i < num_particles; i++) {
                    m_particles.push_back(new Particle(newPos, newDir, *this));
                }
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
    lock_guard<mutex> guard(m_mutex);
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
    lock_guard<mutex> guard(m_mutex);
    for(list<Particle *>::iterator p = m_particles.begin(); p != m_particles.end(); p++)
    {
        (*p)->loc((*p)->loc() + offset);
    }
}

void ParticleController::destroyBuffers()
{
    glDeleteVertexArrays(1, &m_vao_id);
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &particles_color_buffer);
//    glDeleteBuffers(1, &particles_center_buffer);
    glDeleteBuffers(1, &particles_uv_buffer);
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

//    int centerBufferSize = kMaxParticles * kNumVerticesPerParticle * kNumVertexComponents * sizeof(GLfloat);
//    glGenBuffers(1, &particles_center_buffer);
//    glBindBuffer(GL_ARRAY_BUFFER, particles_center_buffer);
//    glBufferData(GL_ARRAY_BUFFER, centerBufferSize, NULL, GL_STREAM_DRAW);
//    GetGLError();

    // The VBO containing the uvs of the particles
    int uvBuffersSize = kMaxParticles * kNumVerticesPerParticle * kNumVertexComponents * sizeof(GLfloat);
    glGenBuffers(1, &particles_uv_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, uvBuffersSize, NULL, GL_STREAM_DRAW);
    GetGLError();
    

    glGenVertexArrays(1, &m_vao_id);
    glBindVertexArray(m_vao_id);

    GetGLError();

    glEnableVertexAttribArray(0); // vertex position
    glEnableVertexAttribArray(1); // vertex color
    glEnableVertexAttribArray(2); // vertex uv
//    glEnableVertexAttribArray(3); // vertex center

    GetGLError();

    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(0, kNumVertexComponents, GL_FLOAT, GL_FALSE, 0, NULL);

    GetGLError();
    
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(1, kNumColorComponents, GL_FLOAT, GL_FALSE, 0, NULL);
    
    GetGLError();

    glBindBuffer(GL_ARRAY_BUFFER, particles_uv_buffer);
    glVertexAttribPointer(2, kNumVertexComponents, GL_FLOAT, GL_FALSE, 0, NULL);
    
    GetGLError();

//    glBindBuffer(GL_ARRAY_BUFFER, particles_center_buffer);
//    glVertexAttribPointer(3, kNumVertexComponents, GL_FLOAT, GL_FALSE, 0, NULL);
//    
//    GetGLError();

    
}

void ParticleController::drawBuffers()
{
    ShaderProgramPtr shader_program = ParticleApp::getShaderProgram(ParticleApp::kShaderParticles);
    shader_program->useProgram();
    GLint location = glGetUniformLocation(shader_program->id(), "viewSize");
    GetGLError();
    if (location >= 0) {
        Vec2 size = m_appPtr->windowSize();
        glUniform2i(location, (int) size.x(), (int) size.y());
        GetGLError();
    } else {
        std::cerr << "Couldn't find uniform viewSize" << std::endl;
    }

    lock_guard<mutex> guard(m_mutex);
    
    int nParticles = numParticles();
    
    if (nParticles == 0) {
        return;
    }
    
    int pIdx = 0;
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
        m_gpu_ParticleUVsArray[vIdxBase + 0] = 0;
        m_gpu_ParticleUVsArray[vIdxBase + 1] = 0;
        
        // vertex 1
        vIdxBase = pIdxBase + kNumVertexComponents * 1;
        m_gpuPositionsArray[vIdxBase + 0] = cx - radius;
        m_gpuPositionsArray[vIdxBase + 1] = cy + radius;
        m_gpu_ParticleUVsArray[vIdxBase + 0] = 1;
        m_gpu_ParticleUVsArray[vIdxBase + 1] = 0;

        // vertex 2
        vIdxBase = pIdxBase + kNumVertexComponents * 2;
        m_gpuPositionsArray[vIdxBase + 0] = cx + radius;
        m_gpuPositionsArray[vIdxBase + 1] = cy + radius;
        m_gpu_ParticleUVsArray[vIdxBase + 0] = 1;
        m_gpu_ParticleUVsArray[vIdxBase + 1] = 1;

        // tri 1
        // vertex 3 (repeat v2)
        vIdxBase = pIdxBase + kNumVertexComponents * 3;
        m_gpuPositionsArray[vIdxBase + 0] = cx + radius;
        m_gpuPositionsArray[vIdxBase + 1] = cy + radius;
        m_gpu_ParticleUVsArray[vIdxBase + 0] = 1;
        m_gpu_ParticleUVsArray[vIdxBase + 1] = 1;

        // vertex 4
        vIdxBase = pIdxBase + kNumVertexComponents * 4;
        m_gpuPositionsArray[vIdxBase + 0] = cx + radius;
        m_gpuPositionsArray[vIdxBase + 1] = cy - radius;
        m_gpu_ParticleUVsArray[vIdxBase + 0] = 0;
        m_gpu_ParticleUVsArray[vIdxBase + 1] = 1;

        // vertex 5 (repeat v0)
        vIdxBase = pIdxBase + kNumVertexComponents * 5;
        m_gpuPositionsArray[vIdxBase + 0] = cx - radius;
        m_gpuPositionsArray[vIdxBase + 1] = cy - radius;
        m_gpu_ParticleUVsArray[vIdxBase + 0] = 0;
        m_gpu_ParticleUVsArray[vIdxBase + 1] = 0;

        
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
    
    glBindVertexArray(m_vao_id);
    GetGLError();

    int positionBufferSize = kMaxParticles * kNumVerticesPerParticle * kNumVertexComponents * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positionBufferSize, &m_gpuPositionsArray[0], GL_STREAM_DRAW);
//    cout << "numParticles: " << numParticles() << ", positionBufferSize: " << positionBufferSize << endl;
    GetGLError();

    int colorBufferSize = kMaxParticles * kNumVerticesPerParticle * kNumColorComponents * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, colorBufferSize, &m_gpuColorsArray[0], GL_STREAM_DRAW);
    GetGLError();

//    int centerBufferSize = kMaxParticles * kNumVerticesPerParticle * kNumVertexComponents * sizeof(GLfloat);
//    glBindBuffer(GL_ARRAY_BUFFER, particles_center_buffer);
//    glBufferData(GL_ARRAY_BUFFER, centerBufferSize, &m_gpu_ParticleCentersArray[0], GL_STREAM_DRAW);
//    GetGLError();
    
    int uvBufferSize = kMaxParticles * kNumVerticesPerParticle * kNumVertexComponents * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, particles_uv_buffer);
    glBufferData(GL_ARRAY_BUFFER, uvBufferSize, &m_gpu_ParticleUVsArray[0], GL_STREAM_DRAW);
    GetGLError();
    
    glDrawArrays(GL_TRIANGLES, 0, nParticles * kNumVerticesPerParticle);
    GetGLError();
}
