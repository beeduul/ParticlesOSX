//
//  Clock.cpp
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#include "Clock.h"

std::chrono::time_point<std::chrono::system_clock> Clock::m_app_start;

void Clock::reset()
{
    m_app_start = std::chrono::system_clock::now();
}

#include <iostream>

float Clock::getElapsedSeconds()
{
    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - m_app_start;

//    std::cout << "getElapsedSeconds: " << elapsed_seconds.count() << std::endl;
    
    return elapsed_seconds.count();
}
