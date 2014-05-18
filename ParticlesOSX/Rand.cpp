//
//  Rand.cpp
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#include "Rand.h"

std::random_device Rand::rd;
std::mt19937 Rand::e2;

void Rand::initialize()
{
    e2 = std::mt19937(rd());
}

float Rand::randFloat()
{
    std::uniform_real_distribution<> dist(0, 1);
    return dist(e2);
}

float Rand::randFloat(float max)
{
    std::uniform_real_distribution<> dist(0, max);
    return dist(e2);
}

float Rand::randFloat(float min, float max)
{
    std::uniform_real_distribution<> dist(min, max);
    return dist(e2);
}

int Rand::randInt(int min, int max)
{
    std::uniform_int_distribution<> dist(min, max);
    return dist(e2);
}
