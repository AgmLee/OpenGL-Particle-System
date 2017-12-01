#include "MeshRenderer.h"
#include <gl_core_4_4.h>
#include <iostream>
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"
#include "Transform.h"


void MeshRenderer::Draw(Camera* activeCamera, Material* material, Transform* transform, Mesh* model, std::vector<Light::Directional> directionalLights)
{
	material->SetProperty(transform->GetWorld(), "modelMatrix");
	material->SetProperty(activeCamera->getProjectionView(), "projectionMatrix");
	material->SetProperty(activeCamera->getPosition(), "camPos");
	material->SetProperty(directionalLights[0]);
	material->Activate();
	model->Draw();
}

MeshRenderer::MeshRenderer()
{
	m_directionals.resize(0);
}

MeshRenderer::~MeshRenderer()
{
}
