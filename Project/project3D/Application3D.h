#pragma once
#include <Application.h>

#include <glm\glm.hpp>
#include <vector>
#include "Light.h"

class Object;
class Mesh;
class Shader;
class Material;
class MyTexture;
class Camera;
class ParticleSystemGPU;

enum BlendMode_;

class Application3D : public aie::Application {
public:

	Application3D();
	virtual ~Application3D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:
	//void DrawDirectionalLight(const glm::vec3& direction, const glm::vec3& diffuse);
	
	Camera* cam;
	
	//Framebuffers
	unsigned int m_fboA;
	unsigned int m_fboB;
	unsigned int m_fboC;
	
	//fbo Textures
	unsigned int m_fboTexture;
	unsigned int m_luImage;
	unsigned int m_blurImage;

	//Depth Buffer
	unsigned int m_fboDepth;

	//Fullscreen Quad
	unsigned int m_vbo, m_vao;

	//fbo Shaders
	Shader* m_luShader;
	Shader* m_blurShader;
	Shader* m_finalShader;

	//Particle Systems
	glm::vec3 rotationAxis = glm::normalize(glm::vec3(1));
	bool drawCubes = true;

	//Shaders
	int finalRenderMode = 0;
	int luRenderMode = 0;
	int blurRenderMode = 0;
	
	//Debug
	glm::vec3 boundVolumeColour = glm::vec3(1);
	glm::vec3 backgroundColour = glm::vec3(0.25f);
	bool showBoundingSpheres = false;
	bool showAsWireframe = false;
	
	//Counters
	unsigned int renderedObjects = 0;

	//Element Vectors
	std::vector<Mesh*>				m_models;
	std::vector<Shader*>			m_shaders;
	std::vector<Material*>			m_materials;
	std::vector<MyTexture*>			m_textures;
	std::vector<Object*>			m_objects;
	std::vector<Light::Directional>	m_directionals;
	std::vector<ParticleSystemGPU*>	m_emitters;
};