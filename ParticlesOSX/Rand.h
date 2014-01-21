//
//  Rand.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef __ParticlesOSX__Rand__
#define __ParticlesOSX__Rand__

#include <random>

class Rand {
public:
    static void initialize();
    static float randFloat();
    static float randFloat(float max);
    static float randFloat(float min, float max);

private:
    static std::random_device rd;
    static std::mt19937 e2;
};

#endif /* defined(__ParticlesOSX__Rand__) */
