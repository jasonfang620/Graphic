#pragma once
#include "ParticleEmitter.h"
#include <random>

class  ParticleEmitterExpolsion : public ParticleEmitter
{
public:
	ParticleEmitterExpolsion();

	virtual void Update(float msec);

protected:
	virtual Particle* GetFreeParticle();

	std::default_random_engine generatorSnow;
};