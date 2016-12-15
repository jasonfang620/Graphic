#pragma once

#include "ParticleEmitter.h"
#include <random>

class ParticleEmittersmoke : public ParticleEmitter
{
public:
	ParticleEmittersmoke();

	virtual void Update(float msec);

protected:
	virtual Particle* GetFreeParticle();
	
	std::default_random_engine generatorSnow;
};