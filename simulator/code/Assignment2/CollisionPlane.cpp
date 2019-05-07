#include "CollisionPlane.h"
#include "Particle.h"
#include<iostream>
using namespace std;

CollisionPlane::CollisionPlane(P3D p, V3D n) {
    pointOnPlane = p;
    normal = n.normalized();
}

// If the given point is colliding with this plane, returns
// the projection of that point onto this plane.
// Otherwise, returns the same point.
P3D CollisionPlane::handleCollision(Particle point)
{
    // TODO: implement collision handling with planes.
    V3D before = -pointOnPlane + point.x_i;
    V3D after = -pointOnPlane + point.x_star;
    if (before.dot(normal) * after.dot(normal) <= 0) {
        // Point collides with plane - return projection
        double dist = after.dot(normal) * 2.0;
        // cout << dist << '\n';
        V3D corr = -(normal * dist);
        return point.x_star + corr;
    }
    return point.x_star;
}