#include "CollisionCube.h"
#include "Particle.h"
#include "Utils/Logger.h"
#include <iostream>
using namespace std;

CollisionCube::CollisionCube(P3D o, double r) {
    origin = o;
    radius = r;
}

// If the given point is colliding with this cube, returns
// the projection of that point onto this cube.
// Otherwise, returns the same point.
P3D CollisionCube::handleCollision(Particle p)
{
    double dist_from_origin = (p.x_star - origin).norm();
    if (dist_from_origin <= radius) {
        // "Bump" point mass position up to the surface of the cube:
        P3D tangent_point;
        if (dist_from_origin < radius) {
            // Compute where the particle should have intersected the cube by extending the path between its
            // 'position' and the cube's origin to the cube's surface.
            // Call the surface intersection point the tangent point.

            V3D direction = (p.x_star - origin).unit();
            tangent_point = origin + (direction * (radius));
        }
        return tangent_point;
    }
    return p.x_star;
}