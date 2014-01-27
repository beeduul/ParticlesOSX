//
//  Clock.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef __ParticlesOSX__Clock__
#define __ParticlesOSX__Clock__

#include <chrono>

class Clock {

public:
    static void reset();
    static float getElapsedSeconds();

private:
    static std::chrono::time_point<std::chrono::system_clock> m_app_start;

};
#endif /* defined(__ParticlesOSX__Clock__) */
