#include "CollisionSphere.h"
#include "Particle.h"
#include "Utils/Logger.h"
#include <iostream>
using namespace std;

CollisionSphere::CollisionSphere(P3D o, double r) {
    origin = o;
    radius = r;
}

// If the given point is colliding with this sphere, returns
// the projection of that point onto this sphere.
// Otherwise, returns the same point.
P3D CollisionSphere::handleCollision(Particle p)
{
    double dist_from_origin = (p.x_star - origin).norm();
    if (dist_from_origin <= radius) {
        // "Bump" point mass position up to the surface of the sphere:
        P3D tangent_point;
        if (dist_from_origin < radius) {
            // Compute where the particle should have intersected the sphere by extending the path between its
            // 'position' and the sphere's origin to the sphere's surface.
            // Call the surface intersection point the tangent point.

            V3D direction = (p.x_star - origin).unit();
            tangent_point = origin + (direction * (radius));
        }
        return tangent_point;
    }
    return p.x_star;
}