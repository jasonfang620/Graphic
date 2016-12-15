#pragma once
#include "../../NCLGL/Vector3.h"
#include "../../NCLGL/Vector4.h"
#include "../../NCLGL/OGLRenderer.h"
#include "../../NCLGL/Mesh.h"
#include <vector>

struct Particle {
	Vector3 position;
	Vector4 colour;
	Vector3 direction;
	float gravity;
};

class ParticleEmitter : public Mesh {
public:
	ParticleEmitter();
	ParticleEmitter(string name);
	~ParticleEmitter(void);

	/*
	To update our particle positions, we must have an update
	function - which has a msec float, just like the other
	updating functions you've seen.
	*/
	void Update(float msec);

	virtual void Draw();

	/*
	How often we spit out some new particles!
	*/
	float	GetParticleRate() { return particleRate; }
	void	SetParticleRate(float rate) { particleRate = rate; }

	/*
	How long each particle lives for!
	*/
	float	GetParticleLifetime() { return particleLifetime; }
	void	SetParticleLifetime(float life) { particleLifetime = life; }

	/*
	How big each particle will be!
	*/
	float	GetParticleSize() { return particleSize; }
	void	SetParticleSize(float size) { particleSize = size; }

	/*
	How much variance of the direction axis each particle can have when
	being launched. Variance of 0 = each particle's direction is = to
	the emitter direction. Variance of 1 = Each particle can go in
	any direction (with a slight bias towards the emitter direction)
	*/
	float	GetParticleVariance() { return particleVariance; }
	void	SetParticleVariance(float variance) { particleVariance = variance; }

	/*
	Linear velocity of the particle
	*/
	float	GetParticleSpeed() { return particleSpeed; }
	void	SetParticleSpeed(float speed) { particleSpeed = speed; }

	/*
	How many particles does the emitter launch when it hits it's update time
	*/
	int		GetLaunchParticles() { return numLaunchParticles; }
	void	SetLaunchParticles(int num) { numLaunchParticles = num; }

	/*
	Launch direction of the particles
	*/
	void	SetDirection(const Vector3 dir) { initialDirection = dir; }
	Vector3 GetDirection() { return initialDirection; }

protected:
	/*
	This is the magic of our free list. If there's a particle 'spare',
	this function will return that, otherwise it'll return a 'new' one
	*/
	Particle* GetFreeParticle();

	/*
	Resizes our vertex buffers
	*/
	void	ResizeArrays();

	float particleRate;
	float particleLifetime;
	float particleSize;
	float particleVariance;
	float particleSpeed;
	int	  numLaunchParticles;

	Vector3 initialDirection;

	float nextParticleTime;		//How long until we next spit out some particles?

	unsigned int largestSize;	//How large has our particle array become?

	std::vector<Particle*>	particles;	//Active particles stay in here :)
	std::vector<Particle*>	freeList;	//'Spare' particles stay in here...
};