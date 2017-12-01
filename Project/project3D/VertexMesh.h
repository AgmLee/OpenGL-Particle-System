#pragma once
#include "Mesh.h"

class VertexMesh :
	public Mesh
{
public:
	VertexMesh(Vertex* aoVertices, unsigned int* auiIndices, unsigned int indicesSize, unsigned int verticesSize, bool calcTBN);

	virtual void RecalculateTBN(std::vector<Vertex>& vertices);

	virtual void Draw();
private:
	virtual void CreateGLBuffers();
};