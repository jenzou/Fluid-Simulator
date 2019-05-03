#ifdef _WIN32
#include <include/glew.h>
#else
#include <glew.h>
#endif

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "ParticleSystem.h"
#include "GUILib/OBJReader.h"
#include "Utils/Logger.h"
#include "Constants.h"
#include <math.h>

GLuint makeBoxDisplayList();

ParticleSystem::ParticleSystem(vector<ParticleInit>& initialParticles)
	: particleMap(KERNEL_H)
{
	int numParticles = initialParticles.size();
	Logger::consolePrint("Created particle system with %d particles", numParticles);
	drawParticles = true;
	count = 0;

	// Create all particles from initial data
	for (auto ip : initialParticles) {
		Particle p;
		p.x_i = ip.position;
		p.v_i = ip.velocity;
		p.x_star = p.x_i;
		p.neighbors.clear();
		particles.push_back(p);
	}

	// Create floor and walls
	CollisionPlane floor(P3D(0, 0, 0), V3D(0, 1, 0));
	CollisionPlane left_wall(P3D(-1, 0, 0), V3D(1, 0, 0));
	CollisionPlane right_wall(P3D(1, 0, 0), V3D(-1, 0, 0));
	CollisionPlane front_wall(P3D(0, 0, -1), V3D(0, 0, 1));
	CollisionPlane back_wall(P3D(0, 0, 1), V3D(0, 0, -1));
	CollisionPlane ceiling(P3D(0, 2, 0), V3D(0, -1, 0));

	planes.push_back(floor);
	planes.push_back(left_wall);
	planes.push_back(right_wall);
	planes.push_back(front_wall);
	planes.push_back(back_wall);
	planes.push_back(ceiling);

	// Arrays to be passed to OpenGL
	positionArray = vector<double>(numParticles * 3);
	pointsIndexArray = vector<unsigned int>(numParticles);

	for (int i = 0; i < numParticles; i++) {
		pointsIndexArray[i] = i;
	}

	boxList = makeBoxDisplayList();
}

ParticleSystem::~ParticleSystem() {
	if (boxList >= 0) {
		glDeleteLists(boxList, 1);
	}
}

bool ParticleSystem::drawParticles = true;
bool ParticleSystem::enableGravity = true;

P3D ParticleSystem::getPositionOf(int i) {
	return particles[i].x_i;
}

// Set the position of particle i.
void ParticleSystem::setPosition(int i, P3D x) {
	particles[i].x_i = x;
	particles[i].x_star = x;
}

// Set the velocity of particle i.
void ParticleSystem::setVelocity(int i, V3D v) {
	particles[i].v_i = v;
}

int ParticleSystem::particleCount() {
	return particles.size();
}

// Gravitational constant.
const V3D GRAVITY = V3D(0, -9.8, 0);

// Applies external forces to particles in the system.
// This is currently limited to just gravity.
void ParticleSystem::applyForces(double delta) {
	if (enableGravity) {
		// Assume all particles have unit mass to simplify calculations.
		for (auto &p : particles) {
			p.v_i += (GRAVITY * delta);
		}
	}
}

// Integrate one time step.
void ParticleSystem::integrate_PBF(double delta) {
	applyForces(delta);
	// Predict positions for this timestep.
	for (auto &p : particles) {
		p.x_star = p.x_i + (p.v_i * delta);
	}

	// Find neighbors for all particles.
	particleMap.clear();
	for (int i = 0; i < particles.size(); i++) {
		particleMap.add(i, particles[i]);
	}

	for (auto &p_i : particles) {
		particleMap.findNeighbors(p_i, particles);
	}

	// TODO: implement the solver loop.
	int iter = 0;
	while (iter < SOLVER_ITERATIONS) {
        for (auto &p_i : particles) {
            // Update lambda
            double gradient_sum = 0;
            for (int n : p_i.neighbors) {
                Particle neighbor = particles[n];
                gradient_sum += gradient_of_constraint(p_i, neighbor).length2();
            }
            p_i.lambda_i = -density_constraint(p_i) / (gradient_sum + CFM_EPSILON);
        }

        for (auto &p_i : particles) {
            // Calculate change in position
            V3D sum = V3D();
            for (int n : p_i.neighbors) {
                Particle neighbor = particles[n];
                double density_term = poly6((p_i.x_i - neighbor.x_i).length()) / poly6(TENSILE_DELTA_Q);
                double s_corr = -TENSILE_K * pow(density_term, TENSILE_N);
                sum += gradient_of_constraint(p_i, neighbor) * (p_i.lambda_i + neighbor.lambda_i + s_corr);
            }
            p_i.delta_p = sum / REST_DENSITY;

            // Collision detection and response
            for (auto &cp_i : planes) {
                p_i.x_star = cp_i.handleCollision(p_i);
            }

            // Update predicted position
            p_i.x_star += p_i.delta_p;
        }
        iter++;
	}

	for (auto &p : particles) {
		// TODO: edit this loop to apply viscosity.
		p.v_i = (p.x_star - p.x_i) / delta;

		// Apply viscosity


		p.x_i = p.x_star;
	}
}

double ParticleSystem::poly6(double r) {
    return POLY_6 * pow((pow(KERNEL_H, 2) - pow(r, 2)), 3);
}

double ParticleSystem::density_constraint(Particle p_i) {
    for (int n : p_i.neighbors) {
        Particle neighbor = particles[n];
        double distance = (p_i.x_i - neighbor.x_i).length();
        if (distance >= 0 && distance <= KERNEL_H) {
            p_i.density += poly6(distance);
        }
    }
    return (p_i.density / REST_DENSITY) - 1;
}

V3D ParticleSystem::gradient_of_constraint(Particle p_i, Particle p_k) {
    if (p_i.x_i == p_k.x_i) {
        // Return sum of gradients of constraint of all neighbors
        V3D sum = V3D();
        for (int n : p_i.neighbors) {
            Particle neighbor = particles[n];
            if (p_i.x_i != neighbor.x_i) {
                sum += -gradient_of_constraint(p_i, neighbor);
            }
        }
        return sum;
    } else {
        V3D r = p_i.x_i - p_k.x_i;
        double distance = r.length();
        if (distance >= 0 && distance <= KERNEL_H) {
            double scalar = (1.f / REST_DENSITY) * -SPIKY_GRADIENT * (pow(KERNEL_H - distance, 2));
            return r.unit() * scalar;
        }
        return V3D();
    }
}

// Code for drawing the particle system is below here.

GLuint makeBoxDisplayList() {

	GLuint index = glGenLists(1);

	glNewList(index, GL_COMPILE);
	glLineWidth(3);
	glColor3d(0, 0, 0);
	glBegin(GL_LINES);
	glVertex3d(-1, 0, -1);
	glVertex3d(-1, 0, 1);

	glVertex3d(-1, 0, 1);
	glVertex3d(1, 0, 1);

	glVertex3d(1, 0, 1);
	glVertex3d(1, 0, -1);

	glVertex3d(1, 0, -1);
	glVertex3d(-1, 0, -1);

	glVertex3d(-1, 0, -1);
	glVertex3d(-1, 2, -1);

	glVertex3d(-1, 0, 1);
	glVertex3d(-1, 2, 1);

	glVertex3d(1, 0, 1);
	glVertex3d(1, 2, 1);

	glVertex3d(1, 0, -1);
	glVertex3d(1, 2, -1);

	glVertex3d(-1, 2, -1);
	glVertex3d(-1, 2, 1);

	glVertex3d(-1, 2, 1);
	glVertex3d(1, 2, 1);

	glVertex3d(1, 2, 1);
	glVertex3d(1, 2, -1);

	glVertex3d(1, 2, -1);
	glVertex3d(-1, 2, -1);
	glEnd();

	glEndList();
	return index;
}

void ParticleSystem::drawParticleSystem() {

	int numParticles = particles.size();
	int i = 0;

	glCallList(boxList);

	// Copy particle positions into array
	positionArray.clear();
	pointsIndexArray.clear();
	for (auto &p : particles) {
		positionArray.push_back(p.x_i[0]);
		positionArray.push_back(p.x_i[1]);
		positionArray.push_back(p.x_i[2]);
		pointsIndexArray.push_back(i);
		i++;
	}

	if (drawParticles && numParticles > 0) {
		// Draw all particles as blue dots
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, &(positionArray.front()));

		glColor4d(0.2, 0.2, 0.8, 1);
		glPointSize(32);
		glDrawElements(GL_POINTS, numParticles, GL_UNSIGNED_INT, &(pointsIndexArray.front()));

		glDisableClientState(GL_VERTEX_ARRAY);
	}

}
