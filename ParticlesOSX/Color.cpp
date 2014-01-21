//
//  Color.cpp
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#include "Color.h"

Color::Color() :
    Color(0, 0, 0)
{
}

Color::Color(const Color &color)
{
    m_color = color.m_color;
}

Color::Color(const float r, const float g, const float b, const float a)
{
    m_color.r = r;
    m_color.g = g;
    m_color.b = b;
    m_color.a = a;
}

Color::Color(const float r, const float g, const float b) :
    Color(r, g, b, 1)
{
}

Color::Color(const float gray, const float a) :
    Color(gray, gray, gray, 1)
{
}

Color::Color(const float gray) :
    Color(gray, 1)
{
}

Color::Color(GLKVector4 glkVector4) :
    m_color(glkVector4)
{
}

const Color& Color::operator=(const Color &other) {
    m_color = other.m_color;
    return *this;
}

const Color Color::operator + (const Color& other) const {
    return Color(GLKVector4Add(m_color, other.m_color));
}

const Color Color::operator - (const Color& other) const {
    return Color(GLKVector4Subtract(m_color, other.m_color));
}

const Color& Color::operator += (const Color& other) {
    m_color = GLKVector4Add(m_color, other.m_color);
    return *this;
}
const Color& Color::operator -= (const Color& other) {
    m_color = GLKVector4Subtract(m_color, other.m_color);
    return *this;
}

const Color Color::operator * (const float& scalar) const {
    return Color(GLKVector4MultiplyScalar(m_color, scalar));
}

const Color& Color::operator *= (const float& scalar) {
    m_color = GLKVector4MultiplyScalar(m_color, scalar);
    return *this;
}

const Color Color::operator / (const float& scalar) const {
    return Color(GLKVector4DivideScalar(m_color, scalar));
}

const Color& Color::operator /= (const float& scalar) {
    m_color = GLKVector4DivideScalar(m_color, scalar);
    return *this;
}
