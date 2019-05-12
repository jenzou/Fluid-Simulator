#pragma once

#include "MathLib/P3D.h"
#include "Particle.h"

class CollisionCube {
private:
    double radius;
public:
    P3D origin;
    CollisionCube(P3D o, double r);
    P3D handleCollision(Particle p);
};