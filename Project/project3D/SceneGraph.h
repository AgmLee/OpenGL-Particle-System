#pragma once

class Transform;

class SceneGraph
{
public:
	SceneGraph(SceneGraph& other) = delete;
	void operator=(SceneGraph& other) = delete;

	static void Create();
	static void Destroy();
	static bool AddTransform(Transform* child);
	static void UpdateScene();
private:
	SceneGraph();
	~SceneGraph();

	Transform* m_origin;
	
	static SceneGraph* sm_singleton;
};