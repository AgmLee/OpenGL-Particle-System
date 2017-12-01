#include "OBJMesh.h"
#include <gl_core_4_4.h>
#include <glm\glm.hpp>
#include <algorithm>
#include <iostream>

OBJMesh::OBJMesh(std::string& filePath)
{
	Init(filePath.c_str());
}
OBJMesh::OBJMesh(const char * filePath)
{
	Init(filePath);
}
void OBJMesh::Init(const char * filePath)
{
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	bool success = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, filePath);

	if (!success)
		std::cerr << err << std::endl;
	else
		FillData(attribs, shapes);
}

void OBJMesh::Draw()
{
	for (auto& g : m_buffers)
	{
		glBindVertexArray(g.VAO);
		glDrawArrays(GL_TRIANGLES, 0, g.drawCount * 3);
	}
}

void OBJMesh::CreateGLBuffers() {}

void OBJMesh::RecalculateTBN(std::vector<Vertex>& vertices)
{
	unsigned int vertexCount = (unsigned int)vertices.size();
	glm::vec4* tan1 = new glm::vec4[vertexCount * 2];
	glm::vec4* tan2 = tan1 + vertexCount;
	memset(tan1, 0, vertexCount * sizeof(glm::vec4) * 2);

	unsigned int indexCount = (unsigned int)vertices.size();
	for (unsigned int a = 0; a < indexCount; a += 3) {
		//because our obj doesn't use indices we use the vertex index (each three vertices is a triangle)
		long i1 = a;
		long i2 = a + 1;
		long i3 = a + 2;

		const glm::vec4& v1 = glm::vec4(vertices[i1].x, vertices[i1].y, vertices[i1].z, 1);
		const glm::vec4& v2 = glm::vec4(vertices[i2].x, vertices[i2].y, vertices[i2].z, 1);
		const glm::vec4& v3 = glm::vec4(vertices[i3].x, vertices[i3].y, vertices[i3].z, 1);

		const glm::vec2& w1 = glm::vec2(vertices[i1].u, 1.0f - vertices[i1].v); //v component is upside down in soul spear
		const glm::vec2& w2 = glm::vec2(vertices[i2].u, 1.0f - vertices[i2].v);
		const glm::vec2& w3 = glm::vec2(vertices[i3].u, 1.0f - vertices[i3].v);

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0F / (s1 * t2 - s2 * t1);
		glm::vec4 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r, 0);
		glm::vec4 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r, 0);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	for (unsigned int a = 0; a < vertexCount; a++) {
		const glm::vec3& n = glm::vec3(vertices[a].nx, vertices[a].ny, vertices[a].nz);
		const glm::vec3& t = glm::vec3(tan1[a]);
		glm::vec4 tangent;
		// Gram-Schmidt orthogonalize
		tangent = glm::vec4(glm::normalize(t - n * glm::dot(n, t)), 0);

		// Calculate handedness (direction of bitangent)
		tangent.w = (glm::dot(glm::cross(glm::vec3(n), glm::vec3(t)), glm::vec3(tan2[a])) < 0.0F) ? 1.0F : -1.0F;
		vertices[a].tx = tangent.x;
		vertices[a].ty = tangent.y;
		vertices[a].tz = tangent.z;
		vertices[a].tw = tangent.w;

		glm::vec3 bitangent = glm::normalize(glm::cross(n, glm::vec3(tangent)));

		vertices[a].bx = bitangent.x;
		vertices[a].by = bitangent.y;
		vertices[a].bz = bitangent.z;
	}
	delete[] tan1;
}

void OBJMesh::FillData(tinyobj::attrib_t& attribs, std::vector<tinyobj::shape_t>& shapes)
{
	bool calcTBN = false;
	m_buffers.resize(shapes.size());
	int shapeIndex = 0;
	for (auto& shape : shapes)
	{
		glGenVertexArrays(1, &m_buffers[shapeIndex].VAO);
		glGenBuffers(1, &m_buffers[shapeIndex].VBO);
		m_buffers[shapeIndex].IBO = 0;
		m_buffers[shapeIndex].drawCount = shape.mesh.num_face_vertices.size();

		std::vector<Vertex> vertices;
		int index = 0;
		for (auto face : shape.mesh.num_face_vertices)
		{
			for (int i = 0; i < 3; ++i)
			{
				tinyobj::index_t idx = shape.mesh.indices[index + i];
				Vertex v = { 0 };
				
				//Positions
				v.x = attribs.vertices[3 * idx.vertex_index + 0];
				v.y = attribs.vertices[3 * idx.vertex_index + 1];
				v.z = attribs.vertices[3 * idx.vertex_index + 2];
				v.w = 1;

				//Normals
				if (attribs.normals.size() > 0)
				{
					v.nx = attribs.normals[3 * idx.normal_index + 0];
					v.ny = attribs.normals[3 * idx.normal_index + 1];
					v.nz = attribs.normals[3 * idx.normal_index + 2];
				}
				//TexCoords
				if (attribs.texcoords.size() > 0)
				{
					v.u = attribs.texcoords[2 * idx.texcoord_index + 0];
					v.v = 1-attribs.texcoords[2 * idx.texcoord_index + 1];
				}
				vertices.push_back(v);
				m_vertices.push_back(v);
			}
			index += face;
		}
		RecalculateTBN(vertices);

		glBindVertexArray(m_buffers[shapeIndex].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_buffers[shapeIndex].VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

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
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		shapeIndex++;
	}
}
