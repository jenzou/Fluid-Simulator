/* http://mmacklin.com/pbf_sig_preprint.pdf */

class Particle {
    float mass;
    Vector3D new_position;
    Vector3D position;
    Vector3D velocity;
    Vector3D ext_force;
    float lambda;
    Vector3D delta_p;
}

class Vector3D {
    float x, y, z;
}

class Simulation {

    List<Particle> particles;
    int side_count;
    Vector3D corner1, corner2;

    float delta_t;
    float gravity = -9.8;

    Map<Particle, List<Particle>> neighbors;
    int iterations;

    float rest_density;
    float eps;

    float poly6_h;
    float spiky_h;

    void initialize() {
        float x_inc = (corner2.x - corner1.x) / side_count; 
        float y_inc = (corner2.y - corner1.y) / side_count;
        float z_inc = (corner2.y - corner1.y) / side_count;

        for (int x = corner1.x; x < corner2.x; x += x_inc) {
            for (int y = corner1.y; y < corner2.y; y += y_inc) {
                for (int z = corner1.z; z < corner2.z; z += z_inc) {
                    Particle p = new Particle(x, y, z);
                    p.velocity = 0;
                    particles.append(particle);
                }
            }
        }
    }

    void iterate() {
        for (Particle i : particles) {
            // apply forces
            i.velocity += delta_t * gravity;
            // predict position
            i.new_position = i.position + delta_t * velocity;
        }

        for (Particle i : particles) {
            // build acceleration struct of neighbors

        }

        int iter = 0;
        while (iter++ < iterations) {
            for (Particle i : particles) {
                i.lambda = getLambda(i);
            }

            for (Particle i : particles) {
                i.delta_p = getDeltaP(i);

                // collision detection
                
            }

            for (Particle i : particles) {
                i.new_position += i.delta_p;
            }
        }

        for (Particle i : particles) {
            // update velocity
            i.velocity = (i.new_position - i.position) / delta_t;

            // vorticity confinement


            // XSPH viscosity


            // update position
            i.position = i.new_position;
        }

    }

    float getLambda(Particle i) {
        float c = getC(i);

        float grad_sum = 0;
        for (Particle k : neighbors.get(i)) { // OR ALL PARTICLES?
            Vector3D grad_c = getGradC(i, k);
            grad_sum += pow(norm(grad_c), 2);
        }

        return c / (grad_sum + eps);
    }

    float getC(Particle i) {
        return get_density(i)/rest_density - 1;
    }

    float getDensity(Particle i) {
        float density = 0;

        for (Particle j : particles) {
            density += poly6(i.position - j.position, poly6_h);
        }

        return density;
    }

    Vector3D getGradC(Particle i, Particle k) { // K WILL BE A NEIGHBOR OF I
        Vector3D grad_c = new Vector3D();

        if (i == k) {
            for (Particle j : neighbors.get(i)) { // OR ALL PARTICLES?
                grad_c += spiky(i.position - j.position, spiky_h, true);
            }
        } else {
            grad_c = -spiky(i.position - j.position, spiky_h, false);
        }

        return grad_c * (1 / rest_density);
    }

    float poly6(Vector3D r, float h) {

    }

    Vector3D spiky(Vector3D r, float h, boolean wrt_first) {

    }

    Vector3D getDeltaP(Particle i) {
        Vector3D delta_p = new Vector3D();

        for (particle j : particles) {
            delta_p += (i.lambda + j.lambda + getCorr(i, j)) * spiky(i.position - j.position, spiky_h, true);
        }

        return delta_p * (1 / rest_density);
    }

    int n;
    float q;
    float k;

    float getCorr(Particle i, Particle j) {

    }
}