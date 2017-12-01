#pragma once
#include "ParticleSystem.h"


class ParticleSystemCPU :
	public ParticleSystem
{
public:
	ParticleSystemCPU();
	virtual void Initialize(unsigned int maxParticles,
		unsigned int emitRate, Material* material,
		float lifeSpanMin, float lifeSpanMax,
		const glm::vec3& velocityMin, const glm::vec3& velocityMax,
		float startSize, float endSize,
		float startRotation, float endRotation,
		const glm::vec4& startColour, const glm::vec4& endColour,
		bool useGravity = false, float gravity = PARTICLESYSTEM_DEFAULT_GRAVITY);
	virtual void Update(float DT);
	virtual void Draw();
	virtual ~ParticleSystemCPU();
protected:
	virtual void Emit();
};

