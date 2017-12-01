#include "VertexMesh.h"
#include <gl_core_4_4.h>
#include <glm\glm.hpp>
#include <iostream>

VertexMesh::VertexMesh(Vertex * aoVertices, unsigned int * auiIndices, unsigned int indicesSize, unsigned int verticesSize, bool calcTBN)
{	
	m_faces.resize(0);
	for (unsigned int i = 0; i < indicesSize; i += 3)
	{
		Face* face = new Face();
		face->vertex[0] = aoVertices[auiIndices[i + 0]];
		face->vertex[1] = aoVertices[auiIndices[i + 1]];
		face->vertex[2] = aoVertices[auiIndices[i + 2]];
		face->index[0] = auiIndices[i + 0];
		face->index[1] = auiIndices[i + 1];
		face->index[2] = auiIndices[i + 2];
		m_faces.push_back(face);
		m_indices.push_back(auiIndices[i + 0]);
		m_indices.push_back(auiIndices[i + 1]);
		m_indices.push_back(auiIndices[i + 2]);
	}
	for (unsigned int i = 0; i < verticesSize; i++)
	{
		m_vertices.push_back(aoVertices[i]);
	}
	if (calcTBN)
	{
		RecalculateTBN(m_vertices);
		for (unsigned int i = 0; i < m_faces.size(); i++)
		{
			m_faces[i]->vertex[0] = m_vertices[m_faces[i]->index[0]];
			m_faces[i]->vertex[1] = m_vertices[m_faces[i]->index[1]];
			m_faces[i]->vertex[2] = m_vertices[m_faces[i]->index[2]];
		}
	}
	m_buffers.resize(1);
	m_buffers[0].drawCount = m_indices.size();
	CreateGLBuffers();
}

void VertexMesh::RecalculateTBN(std::vector<Vertex>& vertices)
{
	for (unsigned int i = 0; i < m_indices.size(); i += 3)
	{
		Vertex v0 = m_vertices[m_indices[i + 0]];
		Vertex v1 = m_vertices[m_indices[i + 1]];
		Vertex v2 = m_vertices[m_indices[i + 2]];

		glm::vec3 p0 = glm::vec3(v0.x, v0.y, v0.z);
		glm::vec3 p1 = glm::vec3(v1.x, v1.y, v1.z);
		glm::vec3 p2 = glm::vec3(v2.x, v2.y, v2.z);

		glm::vec2 u0 = glm::vec2(v0.u, v0.v);
		glm::vec2 u1 = glm::vec2(v1.u, v1.v);
		glm::vec2 u2 = glm::vec2(v2.u, v2.v);

		glm::vec3 p1p0 = p1 - p0;
		glm::vec3 p2p0 = p2 - p0;
		
		glm::vec2 u1u0 = u1 - u0;
		glm::vec2 u2u0 = u2 - u0;

		float r = 1.0f / (u1u0.x * u2u0.y - u1u0.y * u2u0.x);
		glm::vec3 tangent = (p1p0 * u2u0.y - p2p0 * u1u0.y) * r;
		glm::vec3 bitangent = (p2p0 * u1u0.x - p1p0 * u2u0.x) * r;

		v0.tx += tangent.x;	v0.ty += tangent.y;	v0.tz += tangent.z;
		v0.bx += bitangent.x;	v0.by += bitangent.y;	v0.bz += bitangent.z;

		m_vertices[m_indices[i + 0]] = v0;
		m_vertices[m_indices[i + 1]] = v1;
		m_vertices[m_indices[i + 2]] = v2;
	}

	for (unsigned int i = 0; i < m_vertices.size(); i++)
	{
		glm::vec3 n = glm::vec3(m_vertices[i].nx, m_vertices[i].ny, m_vertices[i].nz);
		glm::vec3 t = glm::vec3(m_vertices[i].tx, m_vertices[i].ty, m_vertices[i].tz);
		glm::vec3 b = glm::vec3(m_vertices[i].bx, m_vertices[i].by, m_vertices[i].bz);

		glm::vec4 tangent = glm::vec4(glm::normalize(t - n * glm::dot(n, t)), 0);
		tangent.w = (glm::dot(glm::cross(n, t), b) < 0.0f) ? 1.0f : -1.0f;
		glm::vec3 bitangent = glm::normalize(b);

		m_vertices[i].tx = tangent.x; m_vertices[i].ty = tangent.y; m_vertices[i].tz = tangent.z; m_vertices[i].tw = tangent.w;
		m_vertices[i].bx = bitangent.x; m_vertices[i].by = bitangent.y; m_vertices[i].bz = bitangent.z;
	}
}

void VertexMesh::CreateGLBuffers()
{

	//Generate the Buffers
	glGenVertexArrays(1, &m_buffers[0].VAO);
	glGenBuffers(1, &m_buffers[0].VBO);
	glGenBuffers(1, &m_buffers[0].IBO);

	//Bind Array
	glBindVertexArray(m_buffers[0].VAO);

	//Bind Buffers
	//Vertices
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[0].VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //Position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(1); //Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)16);

	glEnableVertexAttribArray(2); //TexCoords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)28);

	glEnableVertexAttribArray(3); //Tangent
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)36);

	glEnableVertexAttribArray(4); //Bitangent
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)52);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[0].IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

	//Unbind Arrays / Buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VertexMesh::Draw()
{
	glBindVertexArray(m_buffers[0].VAO);
	glDrawElements(GL_TRIANGLES, m_buffers[0].drawCount, GL_UNSIGNED_INT, 0);
}