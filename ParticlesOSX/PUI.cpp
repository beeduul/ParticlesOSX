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

    glGenBuffers(1, &m_rect_colors_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_colors_buffer);
    
    GetGLError();

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    
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
    glDeleteBuffers(1, &m_rect_colors_buffer);
    
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
    
    // send rects to the graphics card
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    
    Color colors[4];
    colors[0] = m_stroke_color;
    colors[1] = m_stroke_color;
    colors[2] = m_stroke_color;
    colors[3] = m_stroke_color;
    
    // send color values to the graphics card
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_colors_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    
    GetGLError();

    glBindVertexArray(m_vao_id);

    m_shader_program->useProgram();
    m_shader_program->setUniform2f("viewSize", m_clip_size.x(), m_clip_size.y());
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    
    Color colors[4];
    colors[0] = m_color;
    colors[1] = m_color;
    colors[2] = m_color;
    colors[3] = m_color;
    
    // send color values to the graphics card
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_colors_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    
    GetGLError();
    
    glBindVertexArray(m_vao_id);
    
    m_shader_program->useProgram();
    m_shader_program->setUniform2f("viewSize", m_clip_size.x(), m_clip_size.y()); // TODO

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    GetGLError();

}

void PGraphics::drawRectGradient(const PUI::Rect &rect, const Color &color0, const Color &color1)
{
    glBindVertexArray(m_vao_id);
    m_shader_program->useProgram();
    m_shader_program->setUniform2f("viewSize", m_clip_size.x(), m_clip_size.y()); // TODO
    
    Vec2 points[4];
    points[0] = rect.getTopLeft();
    points[1] = rect.getTopRight();
    points[2] = rect.getBottomRight();
    points[3] = rect.getBottomLeft();
    
    // send rects to the graphics card
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    
    Color colors[4];
    colors[0] = color0;
    colors[1] = color1;
    colors[2] = color1;
    colors[3] = color0;
    
    // send color values to the graphics card
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_colors_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
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

void PControl::setDelegate(IDelegate *delegate)
{
    m_delegate = delegate;
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
    
    if (m_delegate) {
        m_delegate->controlCallback(this);
    }
}

void PSlider::mouseDown(MouseEvent &event)
{
    m_previous_value = m_value;
    setValueInternal((event.getPoint().x() - m_rect.x0()) / m_rect.getWidth());
}

void PSlider::mouseUp(MouseEvent &event)
{
    if (expandedContains(event.getPoint())) {
        setValueInternal((event.getPoint().x() - m_rect.x0()) / m_rect.getWidth());
    } else {
        m_value = m_previous_value;
    }
}

void PSlider::mouseDrag(MouseEvent &event)
{
    if (expandedContains(event.getPoint())) {
        setValueInternal((event.getPoint().x() - m_rect.x0()) / m_rect.getWidth());
    } else {
        m_value = m_previous_value;
    }
}

void PSlider::mouseEnter(MouseEvent &event)
{
    m_borderColor = Color::White;
}

void PSlider::mouseMove(MouseEvent &event)
{
    
}

void PSlider::mouseExit(MouseEvent &event)
{
    m_borderColor = Color::Black;
}


void PSlider::keyDown(KeyEvent &event)
{
}

// PColorWell
PColorWell::PColorWell(std::string name, const Color& color_min, const Color& color_max, const Rect& rect) :
    PControl(name, rect)
{
    m_color_min = color_min;
    m_color_max = color_max;
}

void PColorWell::setValueInternal(float value)
{
    if (value <= 0) {
        m_value = 0;
    } else if (value > 1) {
        m_value = 1;
    } else {
        m_value = value;
    }
    
    if (m_delegate) {
        m_delegate->controlCallback(this);
    }
}

void PColorWell::draw(PGraphics& g)
{
    float mid_x = m_rect.x0() + (m_rect.x1() - m_rect.x0()) * m_value;
    
    Rect filled(m_rect.x0(), m_rect.y0(), mid_x, m_rect.y1());
    g.drawRectGradient(m_rect, m_color_min, m_color_max);
    
    g.setStrokeColor(m_borderColor);
    g.drawStrokedRect(m_rect);
}

void PColorWell::mouseDown(MouseEvent &event)
{
    m_previous_value = m_value;
    setValueInternal((event.getPoint().x() - m_rect.x0()) / m_rect.getWidth());
}

void PColorWell::mouseDrag(MouseEvent &event)
{
    if (expandedContains(event.getPoint())) {
        setValueInternal((event.getPoint().x() - m_rect.x0()) / m_rect.getWidth());
    } else {
        m_value = m_previous_value;
    }
}

void PColorWell::mouseUp(MouseEvent &event)
{
    if (expandedContains(event.getPoint())) {
        setValueInternal((event.getPoint().x() - m_rect.x0()) / m_rect.getWidth());
    } else {
        m_value = m_previous_value;
    }
}

void PColorWell::mouseEnter(MouseEvent &event) { }
void PColorWell::mouseMove(MouseEvent &event) { }
void PColorWell::mouseExit(MouseEvent &event) { }
void PColorWell::keyDown(KeyEvent &event) { }


// PUI

PUIManager::PUIManager()
{
    m_controls.clear();
}

PUIManager::~PUIManager()
{
}

void PUIManager::addControl(PControl *control)
{
    auto c = std::make_shared<PControl *>(control);
    m_controls.push_back(c);
}

void PUIManager::removeControl(PControl *remove)
{
    BOOST_FOREACH(std::shared_ptr<PControl *> control, m_controls)
    {
        if (*control == remove) {
            m_controls.remove(control);
            return;
        }
    }
}

void PUIManager::removeControls()
{
    m_controls.clear();
}


bool PUIManager::mouseDown(MouseEvent &event)
{
//    cout << "PUIManager::mouseDown " << event->getPoint().x() << ", " << event->getPoint().y() << endl;
    BOOST_FOREACH(std::shared_ptr<PControl *> control, m_controls)
    {
        if ((*control)->contains(event.getPoint())) {
            m_current_control = control;
            if (auto c = m_current_control.lock()) {
                (*c)->mouseDown(event);
            }
            return true;
        }
    }
    return false;
}

bool PUIManager::mouseUp(MouseEvent &event)
{
//    cout << "PUIManager::mouseUp " << event->getPoint().x() << ", " << event->getPoint().y() << endl;

    if (auto c = m_current_control.lock()) {
        (*c)->mouseUp(event);
        m_current_control.reset();
        return true;
    }
    
    return false;
}

bool PUIManager::mouseDrag(MouseEvent &event)
{
//    cout << "PUIManager::mouseDrag " << event->getPoint().x() << ", " << event->getPoint().y() << endl;

    if (auto c = m_current_control.lock()) {
        (*c)->mouseDrag(event);
        return true;
    }
    
    return false;
}

void PUIManager::mouseMove(MouseEvent &event)
{
//    cout << "PUIManager::mouseMove " << event->getPoint().x() << ", " << event->getPoint().y() << endl;
    
    if (m_current_control.expired()) { // no mouse enter/leave/move events if we have mouse down on a control
        
        std::weak_ptr<PControl *> c;
        
        BOOST_FOREACH(std::shared_ptr<PControl *> control, m_controls)
        {
            if ((*control)->contains(event.getPoint())) {
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

void PUIManager::keyDown(KeyEvent &event)
{
    if (auto c = m_current_control.lock()) {
        (*c)->keyDown(event);
    }
}

void PUIManager::draw(PGraphics &graphics)
{
    BOOST_FOREACH(std::shared_ptr<PControl *> control, m_controls)
    {
        (*control)->draw(graphics);
    }
    
}

void PUIManager::resize(const Vec2 &window_size)
{
//    m_window_size = window_size;
}