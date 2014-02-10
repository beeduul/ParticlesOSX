//
//  Vec2.cpp
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#include "Vec2.h"

Vec2::Vec2() : Vec2(0, 0) {
}

Vec2::Vec2(const Vec2 &vec) {
    m_vec = vec.m_vec;
}

Vec2::Vec2(const float x, const float y) {
    m_vec = GLKVector2Make(x, y);
}

Vec2::Vec2(GLKVector2 glkVector2) :
    m_vec(glkVector2)
{
}

const Vec2 Vec2::operator + (const PSize& size) const {
    return Vec2(m_vec.x + size.width, m_vec.y + size.height);
}

const Vec2 Vec2::operator + (const Vec2& other) const {
    return Vec2(GLKVector2Add(m_vec, other.m_vec));
}

const Vec2 Vec2::operator - (const Vec2& other) const {
    return Vec2(GLKVector2Subtract(m_vec, other.m_vec));
}

const Vec2& Vec2::operator += (const Vec2& other) {
    m_vec = GLKVector2Add(m_vec, other.m_vec);
    return *this;
}
const Vec2& Vec2::operator -= (const Vec2& other) {
    m_vec = GLKVector2Subtract(m_vec, other.m_vec);
    return *this;
}

const Vec2 Vec2::operator * (const float& scalar) const {
    return Vec2(GLKVector2MultiplyScalar(m_vec, scalar));
}

const Vec2& Vec2::operator *= (const float& scalar) {
    m_vec = GLKVector2MultiplyScalar(m_vec, scalar);
    return *this;
}

const Vec2 Vec2::operator / (const float& scalar) const {
    return Vec2(GLKVector2DivideScalar(m_vec, scalar));
}

const Vec2& Vec2::operator /= (const float& scalar) {
    m_vec = GLKVector2DivideScalar(m_vec, scalar);
    return *this;
}

const float Vec2::length() const {
    return GLKVector2Distance(GLKVector2Make(0, 0), m_vec);
}
