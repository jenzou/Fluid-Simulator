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
	while (iter++ < SOLVER_ITERATIONS) {
		for (int i = 0; i < particles.size(); i++) {
			// Update lambda
			particles[i].lambda_i = getLambda(i);
		}

		for (int i = 0; i < particles.size(); i++) {
			// Calculate change in position
			particles[i].delta_p = getDeltaP(i);

			/*for (CollisionPlane cp_i : planes) {
				// Collision detection and response
				particles[i].x_star = cp_i.handleCollision(particles[i]);
			}*/
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
		}
	}

	for (auto &p : particles) {
		// TODO: edit this loop to apply vorticity and viscosity.
		p.v_i = (p.x_star - p.x_i) / delta;

		// Apply vorticity


		// Apply viscosity


		p.x_i = p.x_star;
	}
}

double ParticleSystem::getLambda(int i) {
	double c = getC(i);

	double grad_sum = 0.0;
	for (int k : particles[i].neighbors) {
		V3D grad_c = getGradC(i, k);
		grad_sum += grad_c.length2();
	}

	return c / (grad_sum + CFM_EPSILON);
}

double ParticleSystem::getC(int i) {
	particles[i].density = getDensity(i);
	
	return (particles[i].density / REST_DENSITY) - 1.0;
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
			grad_c += spiky(j_to_i, KERNEL_H, true);
		}
	} else {
		V3D k_to_i = particles[i].x_star - particles[k].x_star;
		grad_c = - spiky(k_to_i, KERNEL_H, false);
	}

	return grad_c / REST_DENSITY;
}

V3D ParticleSystem::getDeltaP(int i) {
	V3D delta_p = V3D();

	for (int j : particles[i].neighbors) {
		double coeff = particles[i].lambda_i + particles[j].lambda_i + getCorr(i, j);
		V3D j_to_i = particles[i].x_star - particles[j].x_star;
		V3D term = spiky(j_to_i, KERNEL_H, true);

		delta_p += term * coeff;
	}

	return delta_p / REST_DENSITY;
}

double ParticleSystem::getCorr(int i, int j) {
	V3D j_to_i = particles[i].x_star - particles[j].x_star;
	double num = poly6(j_to_i, KERNEL_H);

	V3D delta_q = V3D(TENSILE_DELTA_Q, 0.0, 0.0);
	double denom = poly6(delta_q, KERNEL_H);

	return - TENSILE_K * pow(num/denom, TENSILE_N);
}

double ParticleSystem::poly6(V3D r, double h) {
	if (r.length() > h) {
		return 0.0;
	}

	double coeff = 315 / 64 / PI / pow(h, 9);
	double term = pow(pow(h, 2) - r.length2(), 3);

	return coeff * term;
}

V3D ParticleSystem::spiky(V3D r, double h, bool wrt_first) {
	if (r.length() > h) {
		return V3D();
	}

	double coeff = -45 / PI / pow(h, 6);
	double term = pow(h - r.length(), 2);
	V3D dir = wrt_first ? r.unit() : - r.unit();

	return dir * (coeff * term);
}

V3D ParticleSystem::getVorticity(int i) {
	V3D vorticity = V3D();
	for (int j : particles[i].neighbors) {
		V3D rel_vel = particles[j].v_i - particles[i].v_i;

		V3D j_to_i = particles[i].x_i - particles[j].x_i;
		V3D smoothing = spiky(j_to_i, KERNEL_H, false);

		vorticity += rel_vel.cross(smoothing);
	}

	return vorticity;
}

void ParticleSystem::applyXSPH(int i) {
	V3D delta_v = V3D();

	for (int j : particles[i].neighbors) {
		V3D rel_vel = particles[j].v_i - particles[i].v_i;
		V3D j_to_i = particles[i].x_i - particles[j].x_i;
		delta_v += rel_vel * poly6(j_to_i, KERNEL_H);
	}

	particles[i].v_i += delta_v * VISCOSITY_C;
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
		glPointSize(11);
		glDrawElements(GL_POINTS, numParticles, GL_UNSIGNED_INT, &(pointsIndexArray.front()));

		glDisableClientState(GL_VERTEX_ARRAY);
	}

}
