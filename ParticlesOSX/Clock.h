//
//  Clock.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#ifndef __ParticlesOSX__Clock__
#define __ParticlesOSX__Clock__

#include <ctime>

class Clock {

public:
    static void reset();
    static float getElapsedSeconds();

private:
    static clock_t m_app_start_time;

};
#endif /* defined(__ParticlesOSX__Clock__) */
