#pragma once
#include "Light.h"
#include "BoundingSphere.h"
#include <vector>

class Camera;
class MeshRenderer;
class Mesh;
class Transform;
class Material;

class Object
{
public:
	Object();
	Object(Transform* transform, Mesh* mesh, Material* material);
	virtual ~Object();

	MeshRenderer* GetRenderer() const;
	Transform* GetTransform() const;
	Material* GetMaterial() const;
	Mesh* GetMesh() const;
	BoundingSphere GetBoundingVolume() const;

	bool SetMesh(Mesh* newMesh);
	bool SetMaterial(Material* newMaterial);

	virtual void Update(float DT);
	virtual bool Draw(Camera* activeCam, std::vector<Light::Directional> directionals);

private:
	Transform* m_transform;
	Mesh* m_model;
	Material* m_material;
	MeshRenderer* m_renderer;
	
	BoundingSphere m_boundingSphere;
};

