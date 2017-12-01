#pragma once
#include <glm\glm.hpp>
#include <vector>

struct Vertex
{
	float x, y, z, w;
	float nx, ny, nz;
	float u, v;
	float tx, ty, tz, tw;
	float bx, by, bz;
};

struct Face
{
	Vertex vertex[3];
	unsigned int index[3];
};

struct GLInfo
{
	unsigned int IBO, VBO, VAO;
	unsigned int drawCount;
};

class Mesh
{
public:
	virtual void Draw() = 0;
	virtual void RecalculateTBN(std::vector<Vertex>& vertices) = 0;

	virtual std::vector<glm::vec3> GetVertexPoints();

	virtual ~Mesh();
protected:
	virtual void CreateGLBuffers() = 0;

	std::vector<GLInfo> m_buffers;
	std::vector<Face*> m_faces;
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
};