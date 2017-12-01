#include "Mesh.h"
#include <iostream>
#include <gl_core_4_4.h>

std::vector<glm::vec3> Mesh::GetVertexPoints()
{
	std::vector<glm::vec3> points;
	for (unsigned int i = 0; i < m_vertices.size(); i++)
	{
		Vertex v = m_vertices[i];
		points.push_back(glm::vec3(v.x, v.y, v.z));
	}
	return points;
}

Mesh::~Mesh()
{
	for (unsigned int i = 0; i < m_faces.size(); i++)
	{
		if (m_faces[i] != nullptr)
		{
			delete m_faces[i];
			m_faces[i] = nullptr;
		}
	}
	for (GLInfo g : m_buffers)
	{
		if (g.VAO != 0)
			glDeleteVertexArrays(1, &g.VAO);
		if (g.VAO != 0)
			glDeleteBuffers(1, &g.VBO);
		if (g.IBO != 0)
			glDeleteBuffers(1, &g.IBO);
	}
}
