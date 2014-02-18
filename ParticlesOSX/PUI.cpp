//
//  PUI.cpp
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 2/2/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#include "PUI.h"
#include "ParticleApp.h"

#include <boost/foreach.hpp>

using namespace PUI;
using namespace std;

// Rect
Rect::Rect(float x0, float y0, float x1, float y1) :
    m_p0(x0, y0),
    m_p1(x1, y1)
{
}

Rect::Rect(const Vec2& p0, const PSize& size) :
    m_p0(p0),
    m_p1(p0 + size)
{
}

Rect::Rect(const Rect& rect) :
    m_p0(rect.getTopLeft()),
    m_p1(rect.getBottomRight())
{
}


const bool Rect::contains(const Vec2& point) const
{
    bool x_0 = x0() <= point.x();
    bool x_1 = point.x() <= x1();
    bool y_0 = y0() <= point.y();
    bool y_1 = point.y() <= y1();
    
    return (x_0 && x_1 && y_0 && y_1);
}

PGraphics::PGraphics()
{
    glGenVertexArrays(1, &m_vao_id);
    glBindVertexArray(m_vao_id);

    GetGLError();

    glGenBuffers(1, &m_rect_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_buffer);

    GetGLError();

    glEnableVertexAttribArray(0); // vertex position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    GetGLError();
    
    m_shader_program = ShaderProgramPtr(new ShaderProgram);
    
    string vertex_shader_source = ParticleApp::getFileContents("rect.vert");
    string fragment_shader_source = ParticleApp::getFileContents("rect.frag");
    
    bool ok;
    ok = m_shader_program->initialize(vertex_shader_source, fragment_shader_source);
    if (!ok) {
        cerr << "PROGRAM LINK ERROR" << endl;
    }

    GetGLError();

}

PGraphics::~PGraphics()
{
    glDeleteVertexArrays(1, &m_vao_id);
    glDeleteBuffers(1, &m_rect_buffer);
   
    GetGLError();

}

void PGraphics::resize(const Vec2 &clip_size)
{
    m_clip_size = clip_size;
}


void PGraphics::drawStrokedRect(const Rect &rect)
{
    Vec2 points[4];
    
    points[0] = rect.getTopLeft();
    points[1] = rect.getTopRight();
    points[2] = rect.getBottomRight();
    points[3] = rect.getBottomLeft();

    
    //    for (int i = 0; i < 8; i++) {
    //        ((float*) &points)[i] = (int) ((float*) &points)[i];
    //    }
    //
//    cout << "drawStrokedRect " <<
//    "(" << points[0].x() << ", " << points[0].y() << "), " <<
//    "(" << points[1].x() << ", " << points[1].y() << "), " <<
//    "(" << points[2].x() << ", " << points[2].y() << "), " <<
//    "(" << points[3].x() << ", " << points[3].y() << "), " << endl;

    // send rects to the graphics card
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_buffer);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(points), // float per vertex
                 points,
                 GL_STATIC_DRAW);
    
    GetGLError();

    glBindVertexArray(m_vao_id);

    m_shader_program->useProgram();
    m_shader_program->setUniform2f("viewSize", m_clip_size.x(), m_clip_size.y());
    m_shader_program->setUniform4fv("inColor", m_stroke_color);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    
    GetGLError();
}

void PGraphics::drawRect(const Rect &rect)
{
    Vec2 points[4];
    
    points[0] = rect.getTopLeft();
    points[1] = rect.getTopRight();
    points[2] = rect.getBottomRight();
    points[3] = rect.getBottomLeft();

    
    // send rects to the graphics card
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_buffer);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(points), // float per vertex
                 points,
                 GL_STATIC_DRAW);
    
    GetGLError();
    
    glBindVertexArray(m_vao_id);
    
    m_shader_program->useProgram();
    m_shader_program->setUniform2f("viewSize", m_clip_size.x(), m_clip_size.y()); // TODO
    m_shader_program->setUniform4fv("inColor", m_color);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    m_shader_program->setUniform4fv("inColor", m_stroke_color);
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    GetGLError();

}

// PControl
PControl::PControl(std::string name, const Rect& rect) :
    m_name(name),
    m_enabled(true),
    m_visible(true),
    m_rect(rect)
{
    
}

const bool PControl::expandedContains(const Vec2 &point)
{
    const int kExpandBy = 10; // px
    return m_enabled && Rect(
        m_rect.x0() - kExpandBy,
        m_rect.y0() - kExpandBy,
        m_rect.x1() + kExpandBy,
        m_rect.y1() + kExpandBy).contains(point);
}

const bool PControl::contains(const Vec2& point) const
{
    return m_enabled && m_rect.contains(point);
}

void PControl::setDelegate(IDelegatePtr ptrDelegate)
{
    m_ptrDelegate = ptrDelegate;
}

PButton::PButton(std::string name, const Rect& rect) :
    PControl(name, rect)
{
}

PSlider::PSlider(std::string name, float min, float max, const Rect& rect) :
    PControl(name, rect),
    m_value(0),
    m_previous_value(0),
    m_min(min),
    m_max(max),
    m_borderColor(Color::Black)
{
}

void PSlider::setColors(const Color &fillColor, const Color &borderColor)
{
    m_fillColor = fillColor;
    m_borderColor = borderColor;
}

void PSlider::draw(PGraphics& g)
{
    float mid_x = m_rect.x0() + (m_rect.x1() - m_rect.x0()) * m_value;

    g.setColor(m_fillColor);
    Rect filled(m_rect.x0(), m_rect.y0(), mid_x, m_rect.y1());
    g.drawRect(filled);

    Color unfilled(m_fillColor);
    unfilled.a(.3);
    g.setColor(unfilled);
    Rect unfilledRect(mid_x, m_rect.y0(), m_rect.x1(), m_rect.y1());
    g.drawRect(unfilledRect);
    
    g.setStrokeColor(m_borderColor);
    g.drawStrokedRect(m_rect);
}

void PSlider::setValueInternal(float value)
{
    if (value <= 0) {
        m_value = 0;
    } else if (value > 1) {
        m_value = 1;
    } else {
    m_value = value;
    }
    
    if (auto d = m_ptrDelegate.lock()) {
        d->controlCallback(this);
    }
}

void PSlider::mouseDown(MouseEvent *event)
{
    m_previous_value = m_value;
    setValueInternal((event->getPoint().x() - m_rect.x0()) / m_rect.getWidth());
}

void PSlider::mouseUp(MouseEvent *event)
{
    if (expandedContains(event->getPoint())) {
        setValueInternal((event->getPoint().x() - m_rect.x0()) / m_rect.getWidth());
    } else {
        m_value = m_previous_value;
    }
}

void PSlider::mouseDrag(MouseEvent *event)
{
    if (expandedContains(event->getPoint())) {
        setValueInternal((event->getPoint().x() - m_rect.x0()) / m_rect.getWidth());
    } else {
        m_value = m_previous_value;
    }
}

void PSlider::mouseEnter(MouseEvent *event)
{
    m_borderColor = Color::White;
}

void PSlider::mouseMove(MouseEvent *event)
{
    
}

void PSlider::mouseExit(MouseEvent *event)
{
    m_borderColor = Color::Black;
}


void PSlider::keyDown(KeyEvent *event)
{
}

// PUI

PUIManager::PUIManager()
{
}

PUIManager::~PUIManager()
{
}

void PUIManager::addControl(PControl *control)
{
    auto c = std::make_shared<PControl *>(control);
    m_controls.push_back(c);
}

bool PUIManager::mouseDown(MouseEvent *event)
{
//    cout << "PUIManager::mouseDown " << event->getPoint().x() << ", " << event->getPoint().y() << endl;
    BOOST_FOREACH(std::shared_ptr<PControl *> control, m_controls)
    {
        if ((*control)->contains(event->getPoint())) {
            m_current_control = control;
            if (auto c = m_current_control.lock()) {
                (*c)->mouseDown(event);
            }
            return true;
        }
    }
    return false;
}

bool PUIManager::mouseUp(MouseEvent *event)
{
//    cout << "PUIManager::mouseUp " << event->getPoint().x() << ", " << event->getPoint().y() << endl;

    if (auto c = m_current_control.lock()) {
        (*c)->mouseUp(event);
        m_current_control.reset();
        return true;
    }
    
    return false;
}

bool PUIManager::mouseDrag(MouseEvent *event)
{
//    cout << "PUIManager::mouseDrag " << event->getPoint().x() << ", " << event->getPoint().y() << endl;

    if (auto c = m_current_control.lock()) {
        (*c)->mouseDrag(event);
        return true;
    }
    
    return false;
}

void PUIManager::mouseMove(MouseEvent *event)
{
//    cout << "PUIManager::mouseMove " << event->getPoint().x() << ", " << event->getPoint().y() << endl;
    
    if (m_current_control.expired()) { // no mouse enter/leave/move events if we have mouse down on a control
        
        std::weak_ptr<PControl *> c;
        
        BOOST_FOREACH(std::shared_ptr<PControl *> control, m_controls)
        {
            if ((*control)->contains(event->getPoint())) {
                c = control;
                break;
            }
        }

        if (!c.expired()) { // we are over a control
            auto current = c.lock();
            if (m_mouse_over_control.expired()) {
                (*current)->mouseEnter(event);
                m_mouse_over_control = c;
            } else {
                auto prev = m_mouse_over_control.lock();
                if (current == prev) {
                    (*current)->mouseMove(event);
                } else {
                    m_mouse_over_control = c;
                    (*prev)->mouseExit(event);
                    (*current)->mouseEnter(event);
                }
            }
        }
    
    }
}

void PUIManager::keyDown(KeyEvent *event)
{
    if (auto c = m_current_control.lock()) {
        (*c)->keyDown(event);
    }
}

void PUIManager::draw()
{
    BOOST_FOREACH(std::shared_ptr<PControl *> control, m_controls)
    {
        (*control)->draw(m_graphics);
    }
    
}

void PUIManager::resize(const Vec2 &window_size)
{
    m_window_size = window_size;
    m_graphics.resize(window_size);
}