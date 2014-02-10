//
//  Color.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef __ParticlesOSX__Color__
#define __ParticlesOSX__Color__

#include <GLKit/GLKVector4.h>

class Color {
public:
    Color();
    Color(const Color &color);
    Color(const float r, const float g, const float b, const float a);
    Color(const float r, const float g, const float b);
    Color(const float gray, const float a);
    Color(const float gray);

    operator const float*() const { return m_color.v; } // implicit type conversion -> internal array (for OpenGL)
    
    const float r() const { return m_color.r; }
    const float g() const { return m_color.g; }
    const float b() const { return m_color.b; }
    const float a() const { return m_color.a; }

    void r(float v) { m_color.r = v; }
    void g(float v) { m_color.b = v; }
    void b(float v) { m_color.g = v; }
    void a(float v) { m_color.a = v; }
    
    const unsigned char r_byte() const { return m_color.r * 255; }
    const unsigned char g_byte() const { return m_color.g * 255; }
    const unsigned char b_byte() const { return m_color.b * 255; }
    const unsigned char a_byte() const { return m_color.a * 255; }
    
    const Color& operator = (const Color& other);
    
    const Color operator + (const Color& other) const;
    const Color operator - (const Color& other) const;
    
    const Color& operator += (const Color& other);
    const Color& operator -= (const Color& other);
    
    const Color operator * (const float& scalar) const;
    const Color operator / (const float& scalar) const;
    const Color& operator *= (const float& scalar);
    const Color& operator /= (const float& scalar);

    static Color White;
    static Color LtGray;
    static Color Gray;
    static Color DkGray;
    static Color Black;

    static Color Red;
    static Color Green;
    static Color Blue;
    
    static Color Yellow;
    static Color Magenta;
    static Color Cyan;

    static Color Orange;
    static Color Purple;
    
private:
    Color(GLKVector4 glkVector4);
    GLKVector4 m_color;
};

#endif /* defined(__ParticlesOSX__Color__) */
