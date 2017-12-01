#include "Object.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "SceneGraph.h"
#include <iostream>
#include <Gizmos.h>

void Object::Update(float DT)
{
	//If it has a model, update the offset of the bounding sphere
	if (m_model != nullptr)
	{
		m_boundingSphere.offset = glm::vec3(m_transform->GetWorld()[3]) + m_boundingSphere.centre;
	}
}
bool Object::Draw(Camera * activeCam, std::vector<Light::Directional> directionals)
{
	if (m_model != nullptr)
	{
		if (m_material != nullptr)
		{
			//Check aggenst frustum planes
			glm::vec4 planes[6];
			activeCam->getFrustumPlanes(planes);
			for (unsigned int i = 0; i < 6; i++)
			{
				float d = glm::dot(glm::vec3(planes[i]), m_boundingSphere.offset) + planes[i].w;
				if (d < -(m_boundingSphere.radius))
				{
					return false;
				}
			}

			m_renderer->Draw(activeCam, m_material, m_transform, m_model, directionals);
			return true;
		}
		else
		{
			//Object was expecting to draw
			std::cerr << "Material pointer is null: " << this << std::endl;
		}
	}
	return false;
}

Object::Object()
{
	m_transform = new Transform();
	m_renderer = new MeshRenderer();
	m_model = nullptr;
	m_material = nullptr;
	SceneGraph::AddTransform(m_transform);
	m_transform->UpdateTransform();
	if (m_model)
	{
		m_boundingSphere.fit(m_model->GetVertexPoints());
	}
}
Object::Object(Transform * transform, Mesh * mesh, Material * material)
{
	m_transform = transform;
	m_model = mesh;
	m_material = material;
	m_renderer = new MeshRenderer();
	SceneGraph::AddTransform(m_transform);
	m_transform->UpdateTransform();
	if (m_model)
	{
		m_boundingSphere.fit(m_model->GetVertexPoints());
	}
}
Object::~Object()
{
	if (m_transform != nullptr)
	{
		delete m_transform;
		m_transform = nullptr;
	}
	if (m_model != nullptr)
	{
		m_model = nullptr;
	}
	if (m_material != nullptr)
	{
		m_material = nullptr;
	}
	if (m_renderer != nullptr)
	{
		m_renderer = nullptr;
	}
}

MeshRenderer * Object::GetRenderer() const
{
	return m_renderer;
}
Transform * Object::GetTransform() const
{
	return m_transform;
}
Material * Object::GetMaterial() const
{
	return m_material;
}
Mesh * Object::GetMesh() const
{
	return m_model;
}

BoundingSphere Object::GetBoundingVolume() const
{
	return m_boundingSphere;
}

bool Object::SetMesh(Mesh * newMesh)
{
	if (newMesh != nullptr)
	{
		m_model = newMesh;
		m_boundingSphere.fit(m_model->GetVertexPoints());
		return true;
	}
	std::cerr << "Mesh pointer is null: " << this << std::endl;
	return false;
}
bool Object::SetMaterial(Material * newMaterial)
{
	if (newMaterial != nullptr)
	{
		m_material = newMaterial;
		return true;
	}
	std::cerr << "Material pointer is null: " << this << std::endl;
	return false;
}
