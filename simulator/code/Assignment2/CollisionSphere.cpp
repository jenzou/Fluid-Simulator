#include "Particle.h"
#include "CollisionSphere.h"
#include <iostream>
using namespace std;

Sphere::Sphere(P3D o, double r) {
    origin = o;
    radius = r;
}

// If the given point is colliding with this sphere, returns
// the projection of that point onto this sphere.
// Otherwise, returns the same point.
P3D Sphere::handleCollision(Particle p)
{
    double dist_from_origin = (p.x_star - origin).norm();
    if (dist_from_origin <= radius) {
        // "Bump" point mass position up to the surface of the sphere:
        V3D tangent_point = p.x_star;
        if (dist_from_origin < radius) {
            // Compute where the particle should have intersected the sphere by extending the path between its
            // 'position' and the sphere's origin to the sphere's surface.
            // Call the surface intersection point the tangent point.

            V3D direction = (p.x_star - origin).unit();
            tangent_point = origin + (direction * radius);
        }

        // Compute the correction vector needed to be applied to the particle's x_i in order to reach the
        // tangent point.
        V3D correction = tangent_point - p.x_i;

        // Finally, let the particle's new position be its x_i adjusted by the above correction vector
        p.x_star = p.x_i + correction * 1.1;
    }

    return p.x_star;
}