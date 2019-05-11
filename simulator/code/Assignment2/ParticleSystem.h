#pragma once

#include <vector>
#include <list>
#include "GUILib/GLMesh.h"
#include "CollisionPlane.h"
#include "Particle.h"
#include "SpatialMap.h"
#include "CollisionSphere.h"

using namespace std;

struct ParticleInit {
	P3D position;
	P3D velocity;
	double mass;
};

using namespace Eigen;

class ParticleSystem {
private:
	vector<Particle> particles;
	vector<CollisionPlane> planes;
	SpatialMap particleMap;
    CollisionSphere sphere = CollisionSphere(P3D(0, .5, 0), 0.5);

	// Vectors to pass to OpenGL for drawing.
	// Each time step, the relevant data are copied into these lists.
	vector<double> positionArray;
	vector<unsigned int> pointsIndexArray;
	vector<unsigned int> edgesIndexArray;
	vector<double> zlSpringPositionArray;
	int count;
	
	unsigned int boxList;

	void moveWall();

	double getLambda(int i);
	double getC(int i);
	double getDensity(int i);
	V3D getGradC(int i, int k);
	V3D getDeltaP(int i);
	double getCorr(int i, int j);

	double poly6(V3D r, double h);
	V3D spiky(V3D r, double h, bool wrt_first);

	V3D getVorticityW(int i);
	V3D getVorticityN(int i);
	V3D getGradW(int i);
	V3D getXSPH(int i);
	
public:
	ParticleSystem(vector<ParticleInit>& particles);
	~ParticleSystem();
	P3D getPositionOf(int i);
	int particleCount();

	void applyForces(double delta);
	void integrate_PBF(double delta);

	// Functions for display and interactivity
	void drawParticleSystem();
	void setPosition(int i, P3D x);
	void setVelocity(int i, V3D v);

	// Whether or not we should draw springs and particles as lines and dots respectively.
	static bool drawParticles;
	static bool enableGravity;

	// Whether or not we should collide with and draw sphere
	static bool addSphere;
};
