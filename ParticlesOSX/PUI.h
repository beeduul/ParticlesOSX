//
//  PUI.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 2/2/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef __ParticlesOSX__PUI__
#define __ParticlesOSX__PUI__

#include "ShaderProgram.h"

#include <iostream>
#include <list>

namespace PUI {

    class Line
    {
    private:
        Vec2 m_p0;
        Vec2 m_p1;
    public:
        Line(Vec2 p0, Vec2 p1) { m_p0 = p0; m_p1 = p1; }
    };

    class Rect
    {
    private:
        Vec2 m_p0;
        Vec2 m_p1;
    public:
        Rect(float x0, float y0, float x1, float y1);
        Rect(const Vec2& position, const PSize& size);
        Rect(const Rect& rect);

        const float x0() const { return m_p0.x(); }
        const float y0() const { return m_p0.y(); }
        const float x1() const { return m_p1.x(); }
        const float y1() const { return m_p1.y(); }
        
//        const Vec2 & getPosition() const { return m_p0; }
//        const Vec2 getSize() const { return Vec2(getWidth(), getHeight()); }
        const float getWidth() const { return abs(m_p0.x() - m_p1.x()); }
        const float getHeight() const { return abs(m_p0.y() - m_p1.y()); }
        
        const Vec2 getTopLeft() const { return Vec2(x0(), y0()); }
        const Vec2 getTopRight() const { return Vec2(x1(), y0()); }
        const Vec2 getBottomRight() const { return  Vec2(x1(), y1()); }
        const Vec2 getBottomLeft() const { return Vec2(x0(), y1()); }

        const bool contains(const Vec2& point) const;
    };

    class Event
    {
    private:
    public:
    };

    class MouseEvent
    {
    private:
        Vec2 m_position;
    public:
        MouseEvent(Vec2 position) : m_position(position) {}
        Vec2 getPoint() const { return m_position; }
    };

    class KeyEvent
    {
    private:
    public:
    };


    class PGraphics;
    typedef std::shared_ptr<PGraphics> PGraphicsPtr;

    class PGraphics
    {
    private:
        GLuint m_vao_id;
        GLuint m_rect_buffer;
        std::vector<Line> m_vec_lines;

        ShaderProgramPtr m_shader_program;

        Vec2 m_clip_size; // TODO clip_rect?
        
        Color m_color;
        Color m_stroke_color;
        
        //static PGraphicsPtr m_currentGraphics;
        
    public:
        PGraphics();
        ~PGraphics();
        
        void setColor(const Color& color) { m_color = color; }
        void setStrokeColor(const Color& stroke_color) { m_stroke_color = stroke_color; }
        void drawStrokedRect(const Rect& rect);
        void drawRect(const Rect& rect);
        
        void resize(const Vec2& clip_size); // clip_rect?
    };
    
    class PControl;
    
    class IDelegate
    {
    public:
        virtual void controlCallback(const PControl* control) = 0;
    };
    
    typedef std::shared_ptr<IDelegate> IDelegatePtr;
    
    class PControl
    {
    protected:
        std::string m_name;
        Rect m_rect;
        bool m_enabled;
        bool m_visible;
        
        std::weak_ptr<IDelegate> m_ptrDelegate;
        
    public:
        PControl(std::string name, const Rect& rect);

        std::string getName() const { return m_name; }
        const bool contains(const Vec2& point) const;

        void setDelegate(IDelegatePtr ptrDelegate);
        
        virtual void draw(PGraphics& graphics) = 0;

        virtual void mouseDown(MouseEvent *event) = 0;
        virtual void mouseDrag(MouseEvent *event) = 0;
        virtual void mouseUp(MouseEvent *event) = 0;
        
        virtual void mouseEnter(MouseEvent *event) = 0;
        virtual void mouseMove(MouseEvent *event) = 0;
        virtual void mouseExit(MouseEvent *event) = 0;
        
        virtual void keyDown(KeyEvent *event) = 0;
    };
    
    class PButton : public PControl
    {
    public:
        PButton(std::string name, const Rect& rect);
    };

    class PSlider : public PControl
    {
    private:
        
        float m_previous_value;
        float m_value;
        
        float m_min;
        float m_max;
        
        Color m_fillColor;
        Color m_borderColor;
        void setValueInternal(float value);
        
    public:
        PSlider(std::string name, float min_value, float max_value, const Rect& rect);

        void setValue(float value) { m_value = (value - m_min) / (m_max - m_min); }
        float getValue() const { return (m_max - m_min) * m_value + m_min; }

        void setColors(const Color& fillColor, const Color& borderColor);
        
        virtual void draw(PGraphics& graphics);

        virtual void mouseDown(MouseEvent *event);
        virtual void mouseDrag(MouseEvent *event);
        virtual void mouseUp(MouseEvent *event);
        
        virtual void mouseEnter(MouseEvent *event);
        virtual void mouseMove(MouseEvent *event);
        virtual void mouseExit(MouseEvent *event);

        virtual void keyDown(KeyEvent *event);
    };
    
    class PUIManager
    {
    private:
        PGraphics m_graphics;

        std::weak_ptr<PControl *> m_mouse_over_control;

        std::weak_ptr<PControl *> m_current_control;
        std::list<std::shared_ptr<PControl *>> m_controls;
        
        Vec2 m_window_size;
        
    public:
        PUIManager();
        ~PUIManager();
        void addControl(PControl *control);

        bool mouseDown(MouseEvent *event);
        bool mouseUp(MouseEvent *event);
        bool mouseDrag(MouseEvent *event);

        void mouseMove(MouseEvent *event);

        void keyDown(KeyEvent *event);
        
        void draw();
        
        void resize(const Vec2& window_size);
    };

    
}; // namespace PUI

#endif /* defined(__ParticlesOSX__PUI__) */
