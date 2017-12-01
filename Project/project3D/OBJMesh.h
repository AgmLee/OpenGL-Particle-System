#pragma once
#include <string>
#include "tiny_obj_loader.h"
#include "Mesh.h"

class OBJMesh :
	public Mesh
{
public:
	OBJMesh(std::string& filePath);
	OBJMesh(const char* filePath);

	virtual void Draw();
	virtual void RecalculateTBN(std::vector<Vertex>& vertices);
	
private:
	virtual void CreateGLBuffers();
	
	void Init(const char* filePath);
	void FillData(tinyobj::attrib_t& attribs, std::vector<tinyobj::shape_t>& shapes);
};