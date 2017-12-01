#include "ParticleSystem.h"
#include "Transform.h"
#include "SceneGraph.h"
#include <gl_core_4_4.h>

ParticleSystem::ParticleSystem()
	: m_particles(nullptr),
	m_firstDead(0),
	m_maxParticles(0),
	m_transform(new Transform()),
	m_VAO(0), m_VBO(0), m_IBO(0),
	m_vertexData(nullptr)
{
	SceneGraph::AddTransform(m_transform);
}

void ParticleSystem::SetCameraReference(Camera * activeCam)
{
	m_activeCamReference = activeCam;
}

ParticleSystem::~ParticleSystem()
{
	if (m_particles)
	{
		delete[] m_particles;
		m_particles = nullptr;
	}
	if (m_vertexData)
	{
		delete[] m_vertexData;
		m_vertexData = nullptr;
	}
	if (m_transform)
	{
		delete m_transform;
		m_transform = nullptr;
	}

	if (m_VAO != 0)
	{
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
	if (m_VBO != 0)
	{
		glDeleteBuffers(1, &m_VBO);
		m_VBO = 0;
	}
	if (m_IBO != 0)
	{
		glDeleteBuffers(1, &m_IBO);
		m_IBO = 0;
	}
}

