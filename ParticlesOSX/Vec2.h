//
//  Vec2.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef __ParticlesOSX__Vec2__
#define __ParticlesOSX__Vec2__


#include <GLKit/GLKVector2.h>

struct PSize
{
    PSize(int w, int h) { width = w; height = h; }
    float width, height;
};


class Vec2 {
public:
    Vec2();
    Vec2(const Vec2 &vec);
    Vec2(const float x, const float y);
    
    const float x() const { return m_vec.x; }
    const float y() const { return m_vec.y; }

    const Vec2 operator + (const PSize& size) const;

    const Vec2 operator + (const Vec2& other) const;
    const Vec2 operator - (const Vec2& other) const;

    const Vec2& operator += (const Vec2& other);
    const Vec2& operator -= (const Vec2& other);

    const Vec2 operator * (const float& scalar) const;
    const Vec2 operator / (const float& scalar) const;
    const Vec2& operator *= (const float& scalar);
    const Vec2& operator /= (const float& scalar);

    const float length() const;
    
private:
    Vec2(GLKVector2 glkVector2);
    GLKVector2 m_vec;
};

#endif /* defined(__ParticlesOSX__Vec2__) */
