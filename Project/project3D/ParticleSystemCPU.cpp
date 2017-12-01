#include "ParticleSystemCPU.h"
#include <gl_core_4_4.h>
#include <iostream>
#include "Transform.h"
#include "Material.h"
#include "Camera.h"
#include <algorithm>

//Clips value based on clip1 and clip2, if larger than the larger one, returns larger and vice versa
float Clip(float value, float clip1, float clip2)
{
	float tMax, tMin;
	if (clip2 > clip1)
	{
		tMax = clip2;
		tMin = clip1;
	}
	else
	{
		tMax = clip1;
		tMin = clip2;
	}
	if (value >= tMax)
		return tMax;
	if (value <= tMin)
		return tMin;
	return value;
}

void ParticleSystemCPU::Update(float DT)
{
	//Emit any new particles it may need
	const glm::mat4 & camTransform = m_activeCamReference->getWorldTransform();
	m_emitTimer += DT;
	while (m_emitTimer > m_emitRate)
	{
		Emit();
		m_emitTimer -= m_emitRate;
	}

	unsigned int quad = 0;

	for (unsigned int i = 0; i < m_firstDead; i++)
	{
		Particle* particle = &m_particles[i];

		particle->lifeTime += DT;
		if (particle->lifeTime >= m_particles->lifeSpan)
		{
			//Remove from living particles
			*particle = m_particles[m_firstDead - 1];
			m_firstDead--;
		}
		else
		{
			//Update particle
			if (m_useGravity)
				particle->velocity.y -= m_gravity * DT;
			particle->position += particle->velocity * DT;
			particle->size = glm::lerp<float>(m_startSize, m_endSize, particle->lifeTime / particle->lifeSpan);
			particle->rotaion = glm::lerp<float>(m_startRot, m_endRot, particle->lifeTime / particle->lifeSpan);
			particle->colour = glm::mix(m_startColour, m_endColour, particle->lifeTime / particle->lifeSpan);
			glm::quat rotation = glm::angleAxis(particle->rotaion, glm::vec3(0, 0, 1));
			
			//Setup Quad
			float halfSize = particle->size * 0.5f;
			m_vertexData[quad * 4 + 0].position = rotation * glm::vec4(halfSize, halfSize, 0, 1);
			m_vertexData[quad * 4 + 0].colour = particle->colour;
			m_vertexData[quad * 4 + 0].uv = glm::vec2(1, -1);
			m_vertexData[quad * 4 + 1].position = rotation * glm::vec4(-halfSize, halfSize, 0, 1);
			m_vertexData[quad * 4 + 1].colour = particle->colour;
			m_vertexData[quad * 4 + 1].uv = glm::vec2(0, -1);
			m_vertexData[quad * 4 + 2].position = rotation * glm::vec4(-halfSize, -halfSize, 0, 1);
			m_vertexData[quad * 4 + 2].colour = particle->colour;
			m_vertexData[quad * 4 + 2].uv = glm::vec2(0, 0);
			m_vertexData[quad * 4 + 3].position = rotation *  glm::vec4(halfSize, -halfSize, 0, 1);
			m_vertexData[quad * 4 + 3].colour = particle->colour;
			m_vertexData[quad * 4 + 3].uv = glm::vec2(1, 0);

			glm::vec3 zAxis = glm::normalize(glm::vec3(camTransform[3]) - particle->position);
			glm::vec3 xAxis = glm::cross(glm::vec3(camTransform[1]), zAxis);
			glm::vec3 yAxiz = glm::cross(zAxis, xAxis);
			glm::mat4 billboard(glm::vec4(xAxis, 0), glm::vec4(yAxiz, 0), glm::vec4(zAxis, 0), glm::vec4(0, 0, 0, 1));

			m_vertexData[quad * 4 + 0].position = billboard * m_vertexData[quad * 4 + 0].position + glm::vec4(particle->position, 0);
			m_vertexData[quad * 4 + 1].position = billboard * m_vertexData[quad * 4 + 1].position + glm::vec4(particle->position, 0);
			m_vertexData[quad * 4 + 2].position = billboard * m_vertexData[quad * 4 + 2].position + glm::vec4(particle->position, 0);
			m_vertexData[quad * 4 + 3].position = billboard * m_vertexData[quad * 4 + 3].position + glm::vec4(particle->position, 0);

			++quad;
		}
	}
}
void ParticleSystemCPU::Draw()
{
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_firstDead * 4 * sizeof(ParticleVertex), m_vertexData);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_firstDead * 6, GL_UNSIGNED_INT, 0);
	
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void ParticleSystemCPU::Initialize(unsigned int maxParticles, unsigned int emitRate, Material* material, float lifeSpanMin, float lifeSpanMax, const glm::vec3& velocityMin, const glm::vec3& velocityMax, float startSize, float endSize, float startRotaions, float endRotation, const glm::vec4 & startColour, const glm::vec4 & endColour, bool useGravity, float gravity)
{
	m_emitTimer = 0;
	m_emitRate = 1.0f / emitRate;

	m_startRot = startRotaions;
	m_endRot = endRotation;
	m_startColour = startColour;
	m_endColour = endColour;
	m_startSize = startSize;
	m_endSize = endSize;
	m_velocityMax = velocityMax;
	m_velocityMin = velocityMin;
	m_lifeSpanMax = lifeSpanMax;
	m_lifeSpanMin = lifeSpanMin;
	m_maxParticles = maxParticles;
	m_material = material;
	m_gravity = gravity;
	m_useGravity = useGravity;

	m_particles = new Particle[m_maxParticles];
	m_firstDead = 0;

	m_vertexData = new ParticleVertex[m_maxParticles * 4];

	unsigned int* indexData = new unsigned int[m_maxParticles * 6];
	for (unsigned int i = 0; i < m_maxParticles; ++i)
	{
		indexData[i * 6 + 0] = i * 4 + 0;
		indexData[i * 6 + 1] = i * 4 + 1;
		indexData[i * 6 + 2] = i * 4 + 2;

		indexData[i * 6 + 3] = i * 4 + 0;
		indexData[i * 6 + 4] = i * 4 + 2;
		indexData[i * 6 + 5] = i * 4 + 3;
	}

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * 4 * sizeof(ParticleVertex), m_vertexData, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_maxParticles * 6 * sizeof(unsigned int), indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), ((char*)0) + 16);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), ((char*)0) + 32);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] indexData;
}

void ParticleSystemCPU::Emit()
{
	if (m_firstDead >= m_maxParticles)
		return;
	Particle& particle = m_particles[m_firstDead++];

	particle.position = m_transform->m_positon;

	particle.lifeTime = 0;
	particle.lifeSpan = (rand() / (float)RAND_MAX) * (m_lifeSpanMax - m_lifeSpanMin) + m_lifeSpanMin;

	particle.colour = m_startColour;
	particle.size = m_startSize;
	particle.rotaion = m_startRot;

	glm::vec3 velocity = glm::vec3(((rand() / (float)RAND_MAX) * (m_velocityMax.x - m_velocityMin.x) + m_velocityMin.x), ((rand() / (float)RAND_MAX) * (m_velocityMax.y - m_velocityMin.y) + m_velocityMin.y), ((rand() / (float)RAND_MAX) * (m_velocityMax.z - m_velocityMin.z) + m_velocityMin.z));
	particle.velocity.x = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity.y = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity.z = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity = glm::normalize(particle.velocity) * velocity;

}

ParticleSystemCPU::ParticleSystemCPU()
{
}
ParticleSystemCPU::~ParticleSystemCPU()
{
}
