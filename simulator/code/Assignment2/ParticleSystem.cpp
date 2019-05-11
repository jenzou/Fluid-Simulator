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
#include <vector>

#include "KDTree.hpp"

#include <iostream>
using namespace std;

GLuint makeBoxDisplayList();

// UPDATING REST_DENSITY IN CONSTANTS.H WAS NOT OVERRIDING THEIR CACHED VALUES. SMH!
volatile double rd = 9000000.0; 

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
		p.vorticity_W = V3D();
		p.vorticity_N = V3D();
		particles.push_back(p);
	}

	// Create floor and walls
	CollisionPlane floor(P3D(0, -1, 0), V3D(0, 1, 0));
	//CollisionPlane floor3(P3D(-1, 0, 0), V3D(0.4, 1, 0));
	//CollisionPlane floor2(P3D(1, 0, 0), V3D(-0.3, 1, 0));
	CollisionPlane left_wall(P3D(-1, 0, 0), V3D(1, 0, 0));
	CollisionPlane right_wall(P3D(1, 0, 0), V3D(-1, 0, 0));
	CollisionPlane front_wall(P3D(0, 0, -1), V3D(0, 0, 1));
	CollisionPlane back_wall(P3D(0, 0, 1), V3D(0, 0, -1));
	CollisionPlane ceiling(P3D(0, 2, 0), V3D(0, -1, 0));

	planes.push_back(floor);
	//planes.push_back(floor3);
	//planes.push_back(floor2);
	planes.push_back(right_wall);
	planes.push_back(front_wall);
	planes.push_back(back_wall);
	planes.push_back(ceiling);
	planes.push_back(left_wall);

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
bool ParticleSystem::addSphere = true;

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

using point_t = std::vector< double >;
using pointVec = std::vector< point_t >;

// Integrate one time step.
void ParticleSystem::integrate_PBF(double delta) {
    //moveWall();

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

    /*pointVec points;
    for (int i = 0; i < particles.size(); i++) {
        point_t point;
        point.push_back((double) particles[i].x_star[0]);
        point.push_back((double) particles[i].x_star[1]);
        point.push_back((double) particles[i].x_star[2]);
        points.push_back(point);
    }
    KDTree tree(points);
    for (int i = 0; i < points.size(); i++) {
        particles[i].neighbors.clear();
        for (size_t neighborIndex : tree.neighborhood_indices(points[i], KERNEL_H)) {
            particles[i].neighbors.push_back(neighborIndex);
        }
    }*/

	// TODO: implement the solver loop.
	int iter = 0;
	while (iter++ < SOLVER_ITERATIONS) {
		for (int i = 0; i < particles.size(); i++) {
			// Update lambda
			particles[i].lambda_i = getLambda(i);
		}

		for (int i = 0; i < particles.size(); i++) {
			// Calculate change in position
			particles[i].delta_p = getDeltaP(i);
		}

		for (auto &p_i : particles) {
			// Update predicted position
			p_i.x_star += p_i.delta_p;
		}
		
		for (int i = 0; i < particles.size(); i++) {
			for (CollisionPlane cp_i : planes) {
				// Collision detection and response
				particles[i].x_star = cp_i.handleCollision(particles[i]);
			}

            if (addSphere) {
                particles[i].x_star = sphere.handleCollision(particles[i]);
            }
		}
	}
	for (int i = 0; i < particles.size(); i++) {
		particles[i].v_i = (particles[i].x_star - particles[i].x_i) / delta;
	}
	for (int i = 0; i < particles.size(); i++) {
		particles[i].vorticity_W = getVorticityW(i);
	}
	for (int i = 0; i < particles.size(); i++) {
		particles[i].vorticity_N = getVorticityN(i);
		V3D vorticity_F = (particles[i].vorticity_N.cross(particles[i].vorticity_W)) * VORTICITY_EPSILON;
		particles[i].v_i += vorticity_F * delta;
	}

	int particle_index = 0;
	for (auto &p : particles) {
		// TODO: edit this loop to apply vorticity and viscosity.
		// p.v_i = (p.x_star - p.x_i) / delta;

		// Apply vorticity
		// V3D vorticity_F = (p.vorticity_N.cross(p.vorticity_W)) * VORTICITY_EPSILON;
		// p.v_i += vorticity_F * delta;
		//cout << vorticity_F * delta << "   " << GRAVITY * delta << "\n";

		// Apply viscosity
		p.v_i += getXSPH(particle_index);

		p.x_i = p.x_star;

		particle_index++;
	}
}

void ParticleSystem::moveWall() {
    double wave = 0.05;
    double wallx = -1.0;
    int delay = 0;

    // Move left wall
    if (delay++ > 100) {
        planes.pop_back();
        if (wallx > 0.0 || wallx < -1.5) {
            wave *= -1;
        }
        wallx += wave;
        planes.push_back(CollisionPlane(P3D(wallx, 0, 0), V3D(1, 0, 0)));
    }
}

double ParticleSystem::getLambda(int i) {
	double c = getC(i);

	double grad_sum = 0.0;
	for (int k : particles[i].neighbors) {
		V3D grad_c = getGradC(i, k);
		grad_sum += grad_c.length2();
	}

	return -c / (grad_sum + CFM_EPSILON);
}

double ParticleSystem::getC(int i) {
	particles[i].density = getDensity(i);
	// cout << particles[i].density << "\n";
	
	return (particles[i].density / rd) - 1.0;
}

double ParticleSystem::getDensity(int i) {
	double density = 0.0;
	for (int j = 0; j < particles.size(); j++) {
		V3D j_to_i = particles[i].x_star - particles[j].x_star;
		density += poly6(j_to_i, KERNEL_H);
	}

	return density;
}

V3D ParticleSystem::getGradC(int i, int k) {
	V3D grad_c = V3D();

	if (i == k) {
		for (int j : particles[i].neighbors) {
			V3D j_to_i = particles[i].x_star - particles[j].x_star;
			grad_c += spiky(j_to_i, KERNEL_H, false);
		}
	} else {
		V3D k_to_i = particles[i].x_star - particles[k].x_star;
		grad_c = -spiky(k_to_i, KERNEL_H, true);
	}

	return grad_c / rd;
}

V3D ParticleSystem::getDeltaP(int i) {
	V3D delta_p = V3D();

	for (int j : particles[i].neighbors) {
		double coeff = particles[i].lambda_i + particles[j].lambda_i + getCorr(i, j);
		V3D j_to_i = particles[i].x_star - particles[j].x_star;
		V3D term = spiky(j_to_i, KERNEL_H, false);

		delta_p += term * coeff;
	}

	return delta_p / rd;
}

double ParticleSystem::getCorr(int i, int j) {
	V3D j_to_i = particles[i].x_star - particles[j].x_star;
	double num = poly6(j_to_i, KERNEL_H);

	V3D delta_q = V3D(TENSILE_DELTA_Q, 0.0, 0.0);
	double denom = poly6(delta_q, KERNEL_H);

	return -TENSILE_K * pow(num/denom, TENSILE_N);
}

double ParticleSystem::poly6(V3D r, double h) {
	if (r.length() > h) {
		return 0.0;
	}

	double coeff = 315.0 / 64.0 / PI / pow(h, 9);
	double term = pow(pow(h, 2) - r.length2(), 3);

	return coeff * term;
}

V3D ParticleSystem::spiky(V3D r, double h, bool wrt_first) {
	if (r.length() > h) {
		return V3D();
	}

	double coeff = -45.0 / PI / pow(h, 6);
	double term = pow(h - r.length(), 2);
	V3D dir = wrt_first ? r.unit() : -r.unit();

	return dir * (coeff * term);
}

V3D ParticleSystem::getVorticityW(int i) {
	V3D vorticity = V3D();
	for (int j : particles[i].neighbors) {
		V3D rel_vel = particles[j].v_i - particles[i].v_i;

		V3D j_to_i = particles[i].x_star - particles[j].x_star;
		V3D smoothing = spiky(j_to_i, KERNEL_H, false);

		vorticity += rel_vel.cross(smoothing);
	}

	return vorticity;
}

V3D ParticleSystem::getVorticityN(int i) {
	V3D grad_w = getGradW(i);
	return grad_w / (grad_w.length() + pow(10, -20));
}

V3D ParticleSystem::getGradW(int i) {
	V3D grad_w = V3D();
	for (int j : particles[i].neighbors) {
		double diff_w = particles[j].vorticity_W.length() - particles[i].vorticity_W.length();
		V3D j_to_i = particles[i].x_star - particles[j].x_star;
		double diff_p = j_to_i.length() + pow(10, -20);

		grad_w += spiky(j_to_i, KERNEL_H, false) * (diff_w / diff_p);
	}

	return grad_w;
}

V3D ParticleSystem::getXSPH(int i) {
	V3D delta_v = V3D();

	for (int j : particles[i].neighbors) {
		V3D rel_vel = particles[j].v_i - particles[i].v_i;
		V3D j_to_i = particles[i].x_star - particles[j].x_star;
		delta_v += rel_vel * poly6(j_to_i, KERNEL_H);
	}

	return delta_v * VISCOSITY_C;
}

// Code for drawing the particle system is below here.

GLuint makeBoxDisplayList() {

	GLuint index = glGenLists(1);

	glNewList(index, GL_COMPILE);
	glLineWidth(3);
	glColor3d(0, 0, 0);
	glBegin(GL_LINES);
	/*glVertex3d(1, 0, -1);
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
	glVertex3d(-1, 2, -1);*/
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

		glColor4d(0.2, 0.2, 0.8, 0.5);
		glPointSize(20);
		glDrawElements(GL_POINTS, numParticles, GL_UNSIGNED_INT, &(pointsIndexArray.front()));

		glDisableClientState(GL_VERTEX_ARRAY);
	}

	if (addSphere) {
        // Draw sphere as large blue dot
        vector<double> positionArray2 = vector<double>{sphere.origin[0], sphere.origin[1]+.25, sphere.origin[2]};
        vector<double> pointsArray2 = vector<double>{0};

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, 0, &(positionArray2.front()));

        glColor4d(0.2, 0.2, 0.8, 1);
        glPointSize(185);
        glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, &(pointsArray2.front()));

        glDisableClientState(GL_VERTEX_ARRAY);
	}
}
