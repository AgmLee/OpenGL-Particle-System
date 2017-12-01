#pragma once
#include <vector>
#include "Light.h"

class Mesh;
class Material;
class Camera;
class Transform;

class MeshRenderer
{
public:
	MeshRenderer();

	void Draw(Camera* activeCamera, Material* material, Transform* transform, Mesh* model, std::vector<Light::Directional> directionalLights);
	
	virtual ~MeshRenderer();
private:
	int m_camPos = -1;
	int m_projection = -1;
	int m_modelMat = -1;
	std::vector<unsigned int> m_directionals;
};

