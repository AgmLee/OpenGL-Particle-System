#pragma once
#include "ParticleSystemGPU.h"

class GPUPSCube :
	public ParticleSystemGPU
{
public:
	GPUPSCube();
	GPUPSCube(glm::vec3* rotationAxis);

	virtual bool Draw(float time, Camera* cam, bool drawParticles) override;

	virtual ~GPUPSCube();
protected:
	virtual void CreateDrawShader() override;

	glm::vec3* m_rotationAxis = nullptr;
};

