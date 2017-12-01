#pragma once
#include <glm\glm.hpp>

class Transform;
class Material;
class Camera;

//Checks if default gravity for particles has been set, if it hasn't then set it
#ifndef PARTICLESYSTEM_DEFAULT_GRAVITY
	#define PARTICLESYSTEM_DEFAULT_GRAVITY 9.81f
#endif // !PARTICLESYSTEM_DEFAULT_GRAVITY

struct Particle
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 colour;
	float rotaion;
	float size;
	float lifeTime;
	float lifeSpan;

	static bool SortByAlpha(const Particle& lhs, const Particle& rhs)
	{
		return lhs.colour.w > rhs.colour.w;
	}
	static bool SortByLifeTime(const Particle& lhs, const Particle& rhs)
	{
		return lhs.lifeTime > rhs.lifeTime;
	}
};

struct ParticleVertex
{
	glm::vec4 position;
	glm::vec4 colour;
	glm::vec2 uv;
};

class ParticleSystem
{
public:
	ParticleSystem();
	
	virtual void Initialize(unsigned int maxParticles,
		unsigned int emitRate, Material* material,
		float lifeSpanMin, float lifeSpanMax,
		const glm::vec3& velocityMin, const glm::vec3& velocityMax,
		float startSize, float endSize,
		float startRotation, float endRotation,
		const glm::vec4& startColour, const glm::vec4& endColour,
		bool useGravity = false, float gravity = PARTICLESYSTEM_DEFAULT_GRAVITY) = 0;
	virtual void Update(float DT) = 0;
	virtual void Draw() = 0;
	
	virtual void SetCameraReference(Camera* activeCam);

	virtual ~ParticleSystem();
protected:
	virtual void Emit() = 0;
	//Settings
	Transform*	m_transform;
	float		m_emitTimer,	m_emitRate;
	float		m_lifeSpanMin,	m_lifeSpanMax;
	glm::vec3	m_velocityMin,	m_velocityMax;
	float		m_startSize,	m_endSize;
	float		m_startRot,		m_endRot;
	glm::vec4	m_startColour,	m_endColour;

	float m_gravity;
	bool m_useGravity;

	Particle* m_particles;
	unsigned int m_firstDead;
	unsigned int m_maxParticles;

	Material* m_material;
	Camera* m_activeCamReference;

	unsigned int m_VAO, m_VBO, m_IBO;
	ParticleVertex* m_vertexData;
};

