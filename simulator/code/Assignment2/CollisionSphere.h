#pragma once

#include "MathLib/P3D.h"
#include "Particle.h"

class Sphere {
private:
    P3D origin;
    double radius;
public:
    Sphere(P3D o, double r);
    P3D handleCollision(Particle p);
};