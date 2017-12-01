#include "SceneGraph.h"
#include "Transform.h"
#include <iostream>

SceneGraph* SceneGraph::sm_singleton = 0;

SceneGraph::SceneGraph()
{
	m_origin = new Transform();
}

void SceneGraph::Create()
{
	if (!sm_singleton)
		sm_singleton = new SceneGraph();
}

void SceneGraph::Destroy()
{
	if (sm_singleton)
	{
		delete sm_singleton;
		sm_singleton = nullptr;
	}
}

bool SceneGraph::AddTransform(Transform * child)
{
	if (!sm_singleton)
	{
		std::cerr << "No instance of SceneGraph Exists!" << std::endl;
		return false;
	}
	Transform::ERROR_RETURNS c = child->SetParent(sm_singleton->m_origin);
	switch (c)
	{
	case Transform::MAJOR_ERROR_RETURN:
		std::cerr << "Origin returned Major Error: " << sm_singleton << std::endl;
		return false;
	case Transform::MINOR_ERROR_RETURN:
		std::cerr << "Origin retrned Minor Error: " << sm_singleton << std::endl;
	default:
		return true;
	}
}

void SceneGraph::UpdateScene()
{
	sm_singleton->m_origin->UpdateTransform();
}

SceneGraph::~SceneGraph()
{
	if (m_origin != nullptr)
	{
		delete m_origin;
		m_origin = nullptr;
	}
}
