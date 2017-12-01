#pragma once
#include <glm\glm.hpp>
#include "BoundingSphere.h"

//Checks if default gravity for particles has been set, if it hasn't then set it
#ifndef PARTICLESYSTEM_DEFAULT_GRAVITY
	#define PARTICLESYSTEM_DEFAULT_GRAVITY 9.81f
#endif // !PARTICLESYSTEM_DEFAULT_GRAVITY

class Transform;
class MyTexture;
class Camera;

struct GPUParticle
{
	GPUParticle() 
	{
		lifetime = 1;
		lifespan = 0;
	};

	glm::vec3 position;
	glm::vec3 velocity;
	float lifetime;
	float lifespan;
};

class ParticleSystemGPU
{
public:
	ParticleSystemGPU();

	virtual void Initialize(unsigned int maxParticles,
		float lifeSpanMin, float lifeSpanMax,
		const glm::vec3& velocityMin, const glm::vec3& velocityMax,
		float startSize, float endSize,
		float rotationMin, float rotationMax,
		const glm::vec4& startColour, const glm::vec4& endColour,
		MyTexture* texture = nullptr, bool drawTri = false,
		bool useGravity = false, float gravity = PARTICLESYSTEM_DEFAULT_GRAVITY);
	
	virtual BoundingSphere GetBoundingVolume() const;

	virtual bool Draw(float time, Camera* cam, bool drawParticles);
	
	virtual void ResizeAmount(unsigned int newMax);
	virtual Transform* GetTransform() const;

	int				m_lhsBlend = 1,					m_rhsBlend = 0;
	float			m_lifespanMin = 0,				m_lifespanMax = 0;
	float			m_startSize = 0,				m_endSize = 0;
	float			m_rotationStart = 0,			m_rotationEnd = 0;
	glm::vec3		m_velocityMin = glm::vec3(0),	m_velocityMax = glm::vec3(0);
	glm::vec4		m_startColour = glm::vec4(0),	m_endColour = glm::vec4(0);
	float			m_gravity = 0;
	bool			m_useGravity = false, m_drawTri = false, m_preRenderCull = true;
	MyTexture*		m_texture = nullptr;
	
	unsigned int	m_maxParticles; 
	int				m_drawCount;

	virtual ~ParticleSystemGPU();
protected:
	virtual void CreateBuffers();
	virtual void CreateUpdateShader();
	virtual void CreateDrawShader();

	BoundingSphere m_sphere;
	GPUParticle*  m_particles;
	Transform* m_transform;

	unsigned int m_VAO[2], m_VBO[2];
	unsigned int m_activeBuffer;

	unsigned int m_draw, m_update;

	float m_lastDrawTime;
};

