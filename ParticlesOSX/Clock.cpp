//
//  Clock.cpp
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#include "Clock.h"

clock_t Clock::m_app_start_time = 0;

void Clock::reset()
{
    m_app_start_time = clock();
}

float Clock::getElapsedSeconds()
{
    return (clock() - m_app_start_time) / CLOCKS_PER_SEC;
}
