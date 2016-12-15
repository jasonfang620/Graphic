#include "ParticleEmitterExpolsion.h"


#define RAND() ((rand()%101)/100.0f)


ParticleEmitterExpolsion::ParticleEmitterExpolsion() {
	texture = SOIL_load_OGL_texture(TEXTUREDIR"smoke.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);

	if (!texture)
	{
		return;
	}
}

void ParticleEmitterExpolsion::Update(float msec)
{
	nextParticleTime -= msec;

	while (nextParticleTime <= 0)
	{
		nextParticleTime += particleRate;
		for (int i = 0; i < numLaunchParticles; ++i)
		{
			particles.push_back(GetFreeParticle());
		}
	}

	for (std::vector<Particle *>::iterator i = particles.begin(); i != particles.end();/*No I++ here!!! */)
	{
		Particle *p = (*i);

		p->colour.w -= (msec / particleLifetime);

		if (p->colour.w <= 0.0f)
		{
			freeList.push_back(p);
			i = particles.erase(i);
		}
		else
		{
			p->position += p->direction*(msec*particleSpeed);

			++i;
		}
	}

	if (particles.size() > largestSize)
	{
		ResizeArrays();
	}
}

Particle* ParticleEmitterExpolsion::GetFreeParticle()
{
	Particle * p = NULL;

	if (freeList.size() > 0)
	{
		p = freeList.back();
		freeList.pop_back();
	}
	else
	{
		p = new Particle();
	}

	std::uniform_real_distribution<float> dis1(0.f, 1.f);
	std::uniform_real_distribution<float> dis2(-1.f, 1.f);
	std::uniform_real_distribution<float> dis3(-1.f, 0.f);

	/*p->gravity = 2.0f;*/
	p->colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	p->direction = initialDirection;
	p->direction.x += ((RAND() - RAND()) * particleVariance);
	p->direction.y += ((RAND() - RAND()) * particleVariance);
	p->direction.z += ((RAND() - RAND()) * particleVariance);

	p->direction.Normalise();
	float radius = 500.0f;
	p->position.x = dis1(generatorSnow) * radius;
	p->position.y = 300.0f;
	p->position.z = dis1(generatorSnow) * radius;

	return p;
}
