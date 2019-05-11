#pragma once

#include "MathLib/P3D.h"
#include "Particle.h"

class CollisionSphere {
private:
    double radius;
public:
    P3D origin;
    CollisionSphere(P3D o, double r);
    P3D handleCollision(Particle p);
};