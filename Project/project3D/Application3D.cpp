#include "Application3D.h"
#include "Gizmos.h"
#include "Input.h"

#include "Renderer.h"
#include "GPUPSCube.h"
#include "SceneControl.h"
#include "FlyCamera.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <gl\GLU.h>
#include <imgui.h>
#include <iostream>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

template<typename T>
void DeleteVector(std::vector<T*>& vector);
Mesh* GenerateGrid(unsigned int rows, unsigned int cols);

bool Application3D::startup() {

	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);
	SceneGraph::Create();

	//Set up Camera
	cam = new FlyCamera();
	cam->setPosition(vec3(0, 0, 10));
	cam->setPerspective(glm::pi<float>() * 0.25f, getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.f);

	//Resize vectors
	m_models.resize(0);
	m_shaders.resize(0);
	m_materials.resize(0);
	m_textures.resize(0);
	m_objects.resize(0);

	//Load/Generate Models
	m_models.push_back(GenerateGrid(10, 10));
	m_models.push_back(new OBJMesh("../bin/models/soulspear.obj"));
	m_models.push_back(new OBJMesh("../bin/models/Bunny.obj"));

	//Load Textures
	m_textures.push_back(new MyTexture("../bin/textures/soulspear_diffuse.tga", 0, GL_TEXTURE0, GL_LINEAR));
	m_textures.push_back(new MyTexture("../bin/textures/soulspear_normal.tga", 1, GL_TEXTURE1, GL_LINEAR));
	m_textures.push_back(new MyTexture("../bin/textures/soulspear_specular.tga", 2, GL_TEXTURE2, GL_LINEAR));
	m_textures.push_back(new MyTexture("../bin/textures/Fire_Test.png", 0, GL_TEXTURE0, GL_NEAREST, true));
	m_textures.push_back(new MyTexture("../bin/textures/snow.png", 0, GL_TEXTURE0, GL_NEAREST, true));

	//Load Shaders
	m_shaders.push_back(new Shader("../bin/shaders/default"));

	//Setup Materials
	{//Plane
		Material* mat = new DefaultMaterial();
		mat->SetShader(m_shaders[0]);
		mat->SetProperty(vec3(0.75f, 0.5f, 0), "diffuseMap.colour");
		mat->SetProperty(vec3(1), "specularMap.colour");
		m_materials.push_back(mat);
	}
	{//Soulspear
		Material* mat = new DefaultMaterial();
		mat->SetShader(m_shaders[0]);
		mat->SetProperty(m_textures[0], "diffuseMap.textureMap");
		mat->SetProperty(m_textures[1], "normalMap.textureMap");
		mat->SetProperty(m_textures[2], "specularMap.textureMap");
		mat->SetProperty(true, "diffuseMap.isSet");
		mat->SetProperty(true, "normalMap.isSet");
		mat->SetProperty(true, "specularMap.isSet");
		m_materials.push_back(mat);
	}
	{//Bunny
		Material* mat = new DefaultMaterial();
		mat->SetShader(m_shaders[0]);
		mat->SetProperty(vec3(1), "diffuseMap.colour");
		mat->SetProperty(vec3(1), "specularMap.colour");
		mat->SetProperty(false, "diffuseMap.isSet");
		mat->SetProperty(false, "normalMap.isSet");
		mat->SetProperty(false, "specularMap.isSet");
		m_materials.push_back(mat);
	}
	
	//Spawn Initial Objects
	m_objects.push_back(new Object(new Transform(vec3(20, 0, -20)),
							m_models[1], m_materials[1]));
	m_objects.push_back(new Object(new Transform(vec3(20, 0, -10)),
							m_models[1], m_materials[1]));
	m_objects.push_back(new Object(new Transform(vec3(20, 0, 0)),
							m_models[2], m_materials[2]));
	m_objects.push_back(new Object(new Transform(vec3(20, 0, 10)),
							m_models[1], m_materials[1]));
	m_objects.push_back(new Object(new Transform(vec3(20, 0, 20)),
							m_models[2], m_materials[2]));
	m_objects.push_back(new Object(new Transform(vec3(-4.5f,0,-4.5f)), 
							m_models[0], m_materials[0]));

	//Setup Lights
	m_directionals.resize(1);
	m_directionals[0].colour = vec3(1, 1, 1);
	m_directionals[0].direction = glm::normalize(vec3(1, 0, 1));
	m_directionals[0].intensity = 1;

	//Setup Particle Systems
	m_emitters.resize(2);
	//Cubes
	m_emitters[0] = new GPUPSCube(&rotationAxis);
	m_emitters[0]->Initialize(100000,
		0.1f, 3.0f,
		vec3(-5, 2, -5), vec3(5, 2, 5),
		1, 0.1f,
		0, 0,
		glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1));
	m_emitters[0]->m_lhsBlend = GL_SRC_ALPHA;
	m_emitters[0]->m_rhsBlend = GL_ONE_MINUS_SRC_ALPHA;
	//Billboard
	m_emitters[1] = new ParticleSystemGPU();
	m_emitters[1]->Initialize(100000,
		0.1f, 3.0f,
		vec3(-5, 2, -5), vec3(5, 2, 5),
		1, 0.1f,
		0, 0,
		glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 1));
	m_emitters[1]->m_lhsBlend = GL_SRC_ALPHA;
	m_emitters[1]->m_rhsBlend = GL_ONE_MINUS_SRC_ALPHA;

	//Setup framebuffers
	{
		{//fboA (First Pass)
			glGenFramebuffers(1, &m_fboA);
			glBindFramebuffer(GL_FRAMEBUFFER, m_fboA);

			glGenTextures(1, &m_fboTexture);
			glBindTexture(GL_TEXTURE_2D, m_fboTexture);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, getWindowWidth(), getWindowHeight());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glGenRenderbuffers(1, &m_fboDepth);
			glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, getWindowWidth(), getWindowHeight());

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);

			GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);
		}
		{//fboB (Luminance Pass)
			glGenFramebuffers(1, &m_fboB);
			glBindFramebuffer(GL_FRAMEBUFFER, m_fboB);

			glGenTextures(1, &m_luImage);
			glBindTexture(GL_TEXTURE_2D, m_luImage);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, getWindowWidth(), getWindowHeight());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_luImage, 0);

			GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);
		}
		{//fboC (Blur Pass)
			glGenFramebuffers(1, &m_fboC);
			glBindFramebuffer(GL_FRAMEBUFFER, m_fboC);

			glGenTextures(1, &m_blurImage);
			glBindTexture(GL_TEXTURE_2D, m_blurImage);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, getWindowWidth(), getWindowHeight());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_blurImage, 0);

			GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);

			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE)
				printf("Framebuffer Error!\n");

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		//Setup Screen Size Quad
		vec2 halfTexel = 1.0f / glm::vec2(getWindowWidth(), getWindowHeight()) * 0.5f; 
		float vertexData[] = {	-1, -1, 0, 1,	halfTexel.x, halfTexel.y, 
								1, 1, 0, 1,		1 - halfTexel.x, 1 - halfTexel.y, 
								-1, 1, 0, 1,	halfTexel.x, 1 - halfTexel.y,
								-1, -1, 0, 1,	halfTexel.x, halfTexel.y,
								1, -1, 0, 1,	1 - halfTexel.x, halfTexel.y,
								1, 1, 0, 1,		1 - halfTexel.x, 1 - halfTexel.y, }; 
		glGenVertexArrays(1, &m_vao); 
		glBindVertexArray(m_vao); 
		glGenBuffers(1, &m_vbo); 
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo); 
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, vertexData, GL_STATIC_DRAW); 
		glEnableVertexAttribArray(0); 
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0); 
		glEnableVertexAttribArray(1); 
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16); 
		glBindVertexArray(0); 
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Shaders
		m_luShader = new Shader("../bin/shaders/luminancePass");
		m_blurShader = new Shader("../bin/shaders/BlurPass");
		m_finalShader = new Shader("../bin/shaders/FinalPass");
	}

	return true;
}
void Application3D::shutdown() {
	//Clear the vectors
	DeleteVector(m_models);
	DeleteVector(m_shaders);
	DeleteVector(m_materials);
	DeleteVector(m_textures);
	DeleteVector(m_objects);
	DeleteVector(m_emitters);

	//Destroy Singletons
	SceneGraph::Destroy();
	Gizmos::destroy();
}

void Application3D::update(float deltaTime) {

	// query time since application started
	float time = getTime();

	//Get the currently active particle system
	ParticleSystemGPU* currentActive;
	if (drawCubes)
		currentActive = m_emitters[0];
	else
		currentActive = m_emitters[1];
	
	//Set up local static variables
	static bool systemMove = true;
	static bool flip = false;
	static vec3 endPos = glm::vec3(0, 5, 0);
	static vec3 startPos = glm::vec3(0);
	static float speed = 2.0f;
	static vec3 dir = glm::normalize(endPos - currentActive->GetTransform()->m_positon);
	static vec3 spawnPos = vec3(0);
	static float distance = 0.5;
	static int currentMax = currentActive->m_maxParticles;
	static vec3 preNormalAxis = rotationAxis;
	static bool showTransform = true;
	static bool seperateCounter = false;
	static bool OptionsOpen = false;
	
	// wipe the gizmos clean for this frame
	Gizmos::clear();

#ifndef _DEBUG
	//Controls display (Only shows once and only outside of debug)
	static bool initialOpen = true;
	if (initialOpen)
	{
		if(ImGui::Begin("Controls", &initialOpen, ImVec2(300, 500), -1.0f, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Mouse to look around.");
			ImGui::Text("WASD to move around.");
			ImGui::Text("Q/E to move down/up.");
			ImGui::Text("Arrows also look around.");
			ImGui::Text("ESC to close.");
			ImGui::Text("Don't resize the window, will cause an issue.");
			ImGui::Text("(Sorry about that.)");
			ImGui::Text("Hold Ctrl and click an element to input values.");
			ImGui::Text("Use the bottom right of the \'OPTIONS\' window to resize it.");
			ImGui::Text("Double Click the \'OPTIONS\' menu to hide/open it.");
			if (ImGui::Button("Close This Window.", ImVec2(0, 50)))
			{
				initialOpen = false;
			}
		}
		ImGui::End();
	}
#endif // !_DEBUG
	
	//Main Options menu
	if (ImGui::Begin("OPTIONS", &OptionsOpen, ImVec2(500, 0), -1.0f, ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		/**** Debug Options ****/
		if (ImGui::CollapsingHeader("DEBUG", "Debug Options"))
		{
			ImGui::BeginChild("Debug", ImVec2(0, 350), true, ImGuiWindowFlags_AlwaysAutoResize);
			/** Contents **/
			//Background Colour
			ImGui::ColorEdit3("Clear Colour", glm::value_ptr(backgroundColour));
			//Camera
			ImGui::Checkbox("Use Mouse for Camera", &cam->useMouse);
			ImGui::SliderFloat("Mouse Key Rotation Speed", &cam->keyRotationSpeed, 1, 5);
			//Information
			ImGui::Checkbox("Seperate Counters", &seperateCounter);
			if (!seperateCounter)
			{
				ImGui::Text("FPS: %d.", m_fps);
				ImGui::Text("Bounding Objects Rendering: %d", renderedObjects);
			}
			//Objects
			ImGui::Checkbox("Show Bounding Volumes", &showBoundingSpheres);
			if (showBoundingSpheres)
			{
				ImGui::ColorEdit3("Bounding Volume Colour", glm::value_ptr(boundVolumeColour));
				ImGui::Checkbox("Show Volumes as Wireframes", &showAsWireframe);
			}
			ImGui::DragFloat3("Objects Spawn Location", &spawnPos[0]);
			if (ImGui::Button("Spawn Plane", ImVec2(0, 20)))
				m_objects.push_back(new Object(new Transform(spawnPos), m_models[0], m_materials[0]));
			if (ImGui::Button("Spawn Soulspear", ImVec2(0, 20)))
				m_objects.push_back(new Object(new Transform(spawnPos), m_models[1], m_materials[1]));
			if (ImGui::Button("Spawn Bunny", ImVec2(0, 20)))
				m_objects.push_back(new Object(new Transform(spawnPos), m_models[2], m_materials[2]));
			if (m_objects.size() > 0)
			{
				if (ImGui::Button("Delete Last Object", ImVec2(0, 20)))
				{
					delete m_objects.back();
					m_objects.pop_back();
				}
			}
			ImGui::Checkbox("Show Spawn Position", &showTransform);
			/** END Contents **/
			ImGui::EndChild();
		}

		/**** Particle System ****/
		if (ImGui::CollapsingHeader("PARTICLE SYSTEMS", "PS Options"))
		{
			ImGui::BeginChild("Particle Systems", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysAutoResize);
			/** Contents **/
			//Application3D variables
			ImGui::Text("Global Variables");
			//System Movement
			if (ImGui::Button("Reset Particle Systems Movement", ImVec2(0, 20)))
			{
				currentActive->GetTransform()->m_positon = startPos;
				flip = false;
				dir = glm::normalize(endPos - currentActive->GetTransform()->m_positon);
			}
			ImGui::Checkbox("Move System", &systemMove);
			ImGui::DragFloat3("Bounce Position 1", &startPos[0]);
			if (systemMove) 
			{
				ImGui::DragFloat3("Bounce Position 2", &endPos[0]);
				ImGui::DragFloat("Speed", &speed);
				ImGui::DragFloat("Distance Buffer", &distance, 1.0f);
				if (distance < 0.001f)
					distance = 0.001f;
			}
			//Draw Type
			ImGui::Checkbox("Draw Cubes", &drawCubes);
			//ParticleSystemGPU variables
			ImGui::Text("Systems Local Variables");
			//Type Specific
			if (drawCubes)
			{
				ImGui::DragFloat3("Rotation Axis", &preNormalAxis[0]);
				if (preNormalAxis != rotationAxis)
				{
					if (ImGui::Button("Set Axis", ImVec2(0, 20)))
					{
						rotationAxis = glm::normalize(preNormalAxis);
						preNormalAxis = rotationAxis;
					}
				}
			}
			else
			{
				ImGui::Checkbox("Draw as Tri", &currentActive->m_drawTri);
			}
			//Rendering
			ImGui::Checkbox("Pre Buffer Cull", &currentActive->m_preRenderCull);
			//Particle Amounts
			ImGui::SliderInt("Amount to Draw", &currentActive->m_drawCount, 0, currentActive->m_maxParticles);
			ImGui::DragInt("Maximum Amount of Particles", &currentMax);
			if (currentMax < 0)
				currentMax = 0;
			if (currentActive->m_maxParticles != currentMax)
				ImGui::Text("Reload the Particle System to have the\nabove change take effect.");
			ImGui::Text("The current amount is %d.", currentActive->m_maxParticles);
			if (ImGui::Button("Reload Particle System", ImVec2(0, 20)))
				currentActive->ResizeAmount(currentMax);
			//Lifespan
			ImGui::DragFloat("Minimum Lifespan", &currentActive->m_lifespanMin, 1.0f);
			ImGui::DragFloat("Maximum Lifespan", &currentActive->m_lifespanMax, 1.0f, currentActive->m_lifespanMin);
			if (currentActive->m_lifespanMin < 0)
				currentActive->m_lifespanMin = 0;
			if (currentActive->m_lifespanMax < currentActive->m_lifespanMin)
				currentActive->m_lifespanMax = currentActive->m_lifespanMin;

			//Size
			ImGui::DragFloat("Start Size", &currentActive->m_startSize);
			ImGui::DragFloat("End Size", &currentActive->m_endSize);
			//Rotation
			ImGui::SliderAngle("Start Rotation", &currentActive->m_rotationStart);
			ImGui::SliderAngle("End Rotation", &currentActive->m_rotationEnd);
			//Velocity
			ImGui::DragFloat3("Minimum Velocity", &currentActive->m_velocityMin[0]);
			ImGui::DragFloat3("Maximum Velocity", &currentActive->m_velocityMax[0]);
			//Colour
			ImGui::ColorEdit4("Start Colour", &currentActive->m_startColour[0]);
			ImGui::ColorEdit4("End Colour", &currentActive->m_endColour[0]);
			//Texture
			if (ImGui::Button("Use Texture 1", ImVec2(0, 20)))
				currentActive->m_texture = m_textures[3];
			if (ImGui::Button("Use Texture 2", ImVec2(0, 20)))
				currentActive->m_texture = m_textures[4];
			if (ImGui::Button("Don\'t use Texture", ImVec2(0, 20)))
				currentActive->m_texture = nullptr;
			//Gravity
			ImGui::Checkbox("Use Gravity", &currentActive->m_useGravity);
			ImGui::DragFloat("Gravity Amount", &currentActive->m_gravity);
			if (ImGui::Button("Reset To Default Gravity", ImVec2(0, 20)))
				currentActive->m_gravity = PARTICLESYSTEM_DEFAULT_GRAVITY;
			//Blend Mode
			ImGui::Text("Blend Mode");
			if (ImGui::Button("Default Blends", ImVec2(0, 20)))
			{
				currentActive->m_lhsBlend = GL_SRC_ALPHA;
				currentActive->m_rhsBlend = GL_ONE_MINUS_SRC_ALPHA;
			}
			ImGui::Columns(2);
			//Left Side
			ImGui::Text("Left Side Blend");
			switch (currentActive->m_lhsBlend)
			{
			case GL_ONE:
				ImGui::Text("Current Left Blend: ONE");
				break;
			case GL_SRC_COLOR:
				ImGui::Text("Current Left Blend: SRC_COLOR");
				break;
			case GL_ONE_MINUS_SRC_COLOR:
				ImGui::Text("Current Left Blend: ONE_MINUS_SRC_COLOR");
				break;
			case GL_DST_COLOR:
				ImGui::Text("Current Left Blend: DST_COLOR");
				break;
			case GL_ONE_MINUS_DST_COLOR:
				ImGui::Text("Current Left Blend: ONE_MINUS_DST_COLOR");
				break;
			case GL_SRC_ALPHA:
				ImGui::Text("Current Left Blend: SRC_ALPHA");
				break;
			case GL_ONE_MINUS_SRC_ALPHA:
				ImGui::Text("Current Left Blend: ONE_MINUS_SRC_ALPHA");
				break;
			case GL_DST_ALPHA:
				ImGui::Text("Current Left Blend: DST_ALPHA");
				break;
			case GL_ONE_MINUS_DST_ALPHA:
				ImGui::Text("Current Left Blend: ONE_MINUS_DST_ALPHA");
				break;
			case GL_CONSTANT_ALPHA:
				ImGui::Text("Current Left Blend: CONSTANT_ALPHA");
				break;
			case GL_ONE_MINUS_CONSTANT_ALPHA:
				ImGui::Text("Current Left Blend: ONE_MINUS_CONSTANT_ALHPA");
				break;
			case GL_SRC_ALPHA_SATURATE:
				ImGui::Text("Current Left Blend: SRC_ALPHA_SATURATE");
				break;
			case GL_ZERO:
				ImGui::Text("Current Left Blend: ZERO");
				break;
			default:
				ImGui::Text("Current Left Blend: ERROR");
				break;
			}
			if (ImGui::Button("Set Left: 1", ImVec2(0, 20)))
				currentActive->m_lhsBlend = GL_ONE;
			if (ImGui::Button("Set Left: Src Color", ImVec2(0, 20)))
				currentActive->m_lhsBlend = GL_SRC_COLOR;
			if (ImGui::Button("Set Left: 1 - Src C", ImVec2(0, 20)))
				currentActive->m_lhsBlend = GL_ONE_MINUS_SRC_COLOR;
			if (ImGui::Button("Set Left: Dst C", ImVec2(0, 20)))
				currentActive->m_lhsBlend = GL_DST_COLOR;
			if (ImGui::Button("Set Left: 1 - Dst C", ImVec2(0, 20)))
				currentActive->m_lhsBlend = GL_ONE_MINUS_DST_COLOR;
			if (ImGui::Button("Set Left: Src A", ImVec2(0, 20)))
				currentActive->m_lhsBlend = GL_SRC_ALPHA;
			if (ImGui::Button("Set Left: 1 - Src A", ImVec2(0, 20)))
				currentActive->m_lhsBlend = GL_ONE_MINUS_SRC_ALPHA;
			if (ImGui::Button("Set Left: Dst A", ImVec2(0, 20)))
				currentActive->m_lhsBlend = GL_DST_ALPHA;
			if (ImGui::Button("Set Left: 1 - Dst A", ImVec2(0, 20)))
				currentActive->m_lhsBlend = GL_ONE_MINUS_DST_ALPHA;
			if (ImGui::Button("Set Left: Constant A", ImVec2(0, 20)))
				currentActive->m_lhsBlend = GL_CONSTANT_ALPHA;
			if (ImGui::Button("Set Left: 1 - Constant A", ImVec2(0, 20)))
				currentActive->m_lhsBlend = GL_ONE_MINUS_CONSTANT_ALPHA;
			if (ImGui::Button("Set Left: Src A Saturate", ImVec2(0, 20)))
				currentActive->m_lhsBlend = GL_SRC_ALPHA_SATURATE;
			if (ImGui::Button("Set Left: 0", ImVec2(0, 20)))
				currentActive->m_lhsBlend = GL_ZERO;
			
			//Right Side
			ImGui::NextColumn();
			ImGui::Text("Right Side Blend");
			switch (currentActive->m_rhsBlend)
			{
			case GL_ONE:
				ImGui::Text("Current Right Blend: ONE");
				break;
			case GL_SRC_COLOR:
				ImGui::Text("Current Right Blend: SRC_COLOR");
				break;
			case GL_ONE_MINUS_SRC_COLOR:
				ImGui::Text("Current Right Blend: ONE_MINUS_SRC_COLOR");
				break;
			case GL_DST_COLOR:
				ImGui::Text("Current Right Blend: DST_COLOR");
				break;
			case GL_ONE_MINUS_DST_COLOR:
				ImGui::Text("Current Right Blend: ONE_MINUS_DST_COLOR");
				break;
			case GL_SRC_ALPHA:
				ImGui::Text("Current Right Blend: SRC_ALPHA");
				break;
			case GL_ONE_MINUS_SRC_ALPHA:
				ImGui::Text("Current Right Blend: ONE_MINUS_SRC_ALPHA");
				break;
			case GL_DST_ALPHA:
				ImGui::Text("Current Right Blend: DST_ALPHA");
				break;
			case GL_ONE_MINUS_DST_ALPHA:
				ImGui::Text("Current Right Blend: ONE_MINUS_DST_ALPHA");
				break;
			case GL_CONSTANT_ALPHA:
				ImGui::Text("Current Right Blend: CONSTANT_ALPHA");
				break;
			case GL_ONE_MINUS_CONSTANT_ALPHA:
				ImGui::Text("Current Blend: ONE_MINUS_CONSTANT_ALHPA");
				break;
			case GL_SRC_ALPHA_SATURATE:
				ImGui::Text("Current Right Blend: SRC_ALPHA_SATURATE");
				break;
			case GL_ZERO:
				ImGui::Text("Current Right Blend: ZERO");
				break;
			default:
				ImGui::Text("Current Right Blend: ERROR");
				break;
			}
			if (ImGui::Button("Set Right: 1", ImVec2(0, 20)))
				currentActive->m_rhsBlend = GL_ONE;
			if (ImGui::Button("Set Right: Src Color", ImVec2(0, 20)))
				currentActive->m_rhsBlend = GL_SRC_COLOR;
			if (ImGui::Button("Set Right: 1 - Src C", ImVec2(0, 20)))
				currentActive->m_rhsBlend = GL_ONE_MINUS_SRC_COLOR;
			if (ImGui::Button("Set Right: Dst C", ImVec2(0, 20)))
				currentActive->m_rhsBlend = GL_DST_COLOR;
			if (ImGui::Button("Set Right: 1 - Dst C", ImVec2(0, 20)))
				currentActive->m_rhsBlend = GL_ONE_MINUS_DST_COLOR;
			if (ImGui::Button("Set Right: Src A", ImVec2(0, 20)))
				currentActive->m_rhsBlend = GL_SRC_ALPHA;
			if (ImGui::Button("Set Right: 1 - Src A", ImVec2(0, 20)))
				currentActive->m_rhsBlend = GL_ONE_MINUS_SRC_ALPHA;
			if (ImGui::Button("Set Right: Dst A", ImVec2(0, 20)))
				currentActive->m_rhsBlend = GL_DST_ALPHA;
			if (ImGui::Button("Set Right: 1 - Dst A", ImVec2(0, 20)))
				currentActive->m_rhsBlend = GL_ONE_MINUS_DST_ALPHA;
			if (ImGui::Button("Set Right: Constant A", ImVec2(0, 20)))
				currentActive->m_rhsBlend = GL_CONSTANT_ALPHA;
			if (ImGui::Button("Set Right: 1 - Constant A", ImVec2(0, 20)))
				currentActive->m_rhsBlend = GL_ONE_MINUS_CONSTANT_ALPHA;
			if (ImGui::Button("Set Right: Src A Saturate", ImVec2(0, 20)))
				currentActive->m_rhsBlend = GL_SRC_ALPHA_SATURATE;
			if (ImGui::Button("Set Right: 0", ImVec2(0, 20)))
				currentActive->m_rhsBlend = GL_ZERO;

			ImGui::Columns(1);
			/** END Contents **/
			ImGui::EndChild();
		}
		
		/**** Shader Options ****/
		if (ImGui::CollapsingHeader("SHADER", "Shader Options"))
		{
			ImGui::BeginChild("Shader", ImVec2(0, 425), true, ImGuiWindowFlags_AlwaysAutoResize);
			/** Contents **/
			//HDR Pass
				ImGui::Text("Luminance Pass");
				ImGui::Text("Render Mode: %d", luRenderMode);
				if (ImGui::Button("Colour Only", ImVec2(0, 20)))
					luRenderMode = 0;
				if (ImGui::Button("Luminance Only", ImVec2(0, 20)))
					luRenderMode = 1;
				if (ImGui::Button("Colour * Luminance", ImVec2(0, 20)))
					luRenderMode = 2;
			//Blur Pass
				ImGui::Text("Blur Pass");
				ImGui::Text("Render Mode: %d", blurRenderMode);
				if (ImGui::Button("No Blur", ImVec2(0, 20)))
					blurRenderMode = 0;
				if (ImGui::Button("Box Blur", ImVec2(0, 20)))
					blurRenderMode = 1;
				if (ImGui::Button("Gaussian Blur Pass 1", ImVec2(0, 20)))
					blurRenderMode = 2;
			//Final Pass
				ImGui::Text("Final Pass");
				ImGui::Text("Render Mode: %d", finalRenderMode);
				if (ImGui::Button("Final Render", ImVec2(0, 20)))
					finalRenderMode = 0;
				if (ImGui::Button("Original Render", ImVec2(0, 20)))
					finalRenderMode = 1;
				if (ImGui::Button("Previous Passes Only", ImVec2(0, 20)))
					finalRenderMode = 2;
				if (ImGui::Button("Gaussian Blur Pass 2", ImVec2(0, 20)))
					finalRenderMode = 3;
			//Presets
				ImGui::Text("Presets");
				if (ImGui::Button("Normal Rendering", ImVec2(0, 20)))
				{
					finalRenderMode = 0;
					luRenderMode = 0;
					blurRenderMode = 0;
				}
				if (ImGui::Button("Grayscale Rendering", ImVec2(0, 20)))
				{
					luRenderMode = 1;
					finalRenderMode = 2;
					blurRenderMode = 0;
				}
			/** END Contents **/
			ImGui::EndChild();
		}
	}
	ImGui::End();

	//If showing the counters as a seperate window
	if (seperateCounter)
	{
		if (ImGui::Begin("COUNTERS", &seperateCounter, ImVec2(250, 80)))
		{
			ImGui::Text("FPS: %d.", m_fps);
			ImGui::Text("Bounding Objects Rendering: %d", renderedObjects);
		}
		ImGui::End();
	}

	cam->update(deltaTime, m_window);
	
	//Move the particle systems between startPos and endPos
	if (systemMove)
	{
		for (unsigned int i = 0; i < m_emitters.size(); i++)
		{
			Transform* t = m_emitters[i]->GetTransform();
			t->m_positon += dir * speed * deltaTime;
			if (!flip && glm::distance2(t->m_positon, endPos) < distance)
			{
				dir = glm::normalize(startPos - t->m_positon);
				flip = true;
			}
			else if (flip && glm::distance2(t->m_positon, startPos) < distance)
			{
				dir = glm::normalize(endPos - t->m_positon);
				flip = false;
			}
		}
	}

	//Update objects
	for (Object* o : m_objects)
	{
		o->Update(deltaTime);
	}

	m_directionals[0].direction = glm::normalize(vec3(sin(getTime()), 0, cos(getTime())));
	
	//Spawn Transform
	if (showTransform)
	{
		mat4 tr;
		tr[3] = vec4(spawnPos, 1);
		Gizmos::addTransform(tr, 3);
	}

	SceneGraph::UpdateScene();

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();
	
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}
void Application3D::draw() {
	//First Pass (No Filters)
	renderedObjects = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboA); 
	glViewport(0, 0, getWindowWidth(), getWindowHeight()); 
	glClearColor(backgroundColour.r, backgroundColour.g, backgroundColour.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	
	// update perspective in case window resized
	cam->setPerspective(glm::pi<float>() * 0.25f, getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.f);

	//Draw Objects
	for (Object* o : m_objects)
	{
		if (o->Draw(cam, m_directionals))
			renderedObjects++;
		if (showBoundingSpheres)
		{
			BoundingSphere b = o->GetBoundingVolume();
			Gizmos::addSphere(b.offset, b.radius, 6, 6, vec4(boundVolumeColour, 1));
		}
	}
	
	//Update the emiters
	if (m_emitters[0]->Draw(getTime(), cam, drawCubes))
		renderedObjects++;
	if (showBoundingSpheres && drawCubes && m_emitters[0]->m_preRenderCull)
	{
		BoundingSphere b = m_emitters[0]->GetBoundingVolume();
		Gizmos::addSphere(b.centre, b.radius, 6, 6, vec4(boundVolumeColour, 1));
	}

	if (m_emitters[1]->Draw(getTime(), cam, !drawCubes))
		renderedObjects++;
	if (showBoundingSpheres && !drawCubes && m_emitters[1]->m_preRenderCull)
	{
		BoundingSphere b = m_emitters[1]->GetBoundingVolume();
		Gizmos::addSphere(b.centre, b.radius, 6, 6, vec4(boundVolumeColour, 1));
	}

	//Draw Gizmos (Bounding spheres and transforms)
	if (showAsWireframe)
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		Gizmos::draw(cam->getProjectionView());
		glPolygonMode(GL_FRONT, GL_FILL);
	}
	else
		Gizmos::draw(cam->getProjectionView());
	
	//Reset Materials for next frame
	Material::DeactivateAll();

	//Second Pass (Lumanice)
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboB);
	glViewport(0, 0, getWindowWidth(), getWindowHeight());
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	unsigned int program = m_luShader->GetProgram();
	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	int loc = glGetUniformLocation(program, "target");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(program, "renderMode");
	glUniform1i(loc, luRenderMode);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Third Pass (Blur)
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboC);
	glViewport(0, 0, getWindowWidth(), getWindowHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program = m_blurShader->GetProgram();
	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_luImage);
	loc = glGetUniformLocation(program, "target");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(program, "renderMode");
	glUniform1i(loc, blurRenderMode);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Final Pass (Final Render)
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	glViewport(0, 0, getWindowWidth(), getWindowHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	program = m_finalShader->GetProgram();
	glUseProgram(program);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	
	loc = glGetUniformLocation(program, "fullimage");
	glUniform1i(loc, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_blurImage);

	loc = glGetUniformLocation(program, "blurimage");
	glUniform1i(loc, 1);

	loc = glGetUniformLocation(program, "renderMode");
	glUniform1i(loc, finalRenderMode);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

Application3D::Application3D() {

}
Application3D::~Application3D() {

}

template<typename T>
void DeleteVector(std::vector<T*>& vector)
{
	for (unsigned int i = 0; i < vector.size(); i++)
	{
		if (vector[i])
		{
			delete vector[i];
			vector[i] = nullptr;
		}
	}
}
Mesh* GenerateGrid(unsigned int rows, unsigned int cols)
{
	//Vertices
	std::vector<Vertex> aoVertices;
	aoVertices.resize(rows * cols);
	for (unsigned int r = 0; r < rows; r++) {
		for (unsigned int c = 0; c < cols; c++)
		{
			unsigned int index = r * cols + c;
			//Pos
			aoVertices[index].x = (float)c;
			aoVertices[index].y = 0;
			aoVertices[index].z = (float)r;
			aoVertices[index].w = 1;
			
			//UV
			aoVertices[index].u = (float)c / cols;
			aoVertices[index].v = 1-((float)r / rows);

			//Normals
			aoVertices[index].nx = 0;
			aoVertices[index].ny = 0;
			aoVertices[index].nz = 0;

			//Tangents/Bitangents
			aoVertices[index].tx = aoVertices[index].bx = 0;
			aoVertices[index].ty = aoVertices[index].by = 0;
			aoVertices[index].tz = aoVertices[index].bz = 0;
		}
	}
	
	//Indices
	std::vector<unsigned int> auiIndices;
	auiIndices.resize((rows - 1)*(cols - 1) * 6);
	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); r++) {
		for (unsigned int c = 0; c < (cols - 1); c++) {
			//Triangle 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			
			//Triangle 2 
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	//Calculate Normals
	for (unsigned int i = 0; i < auiIndices.size(); i += 3)
	{
		Vertex a = aoVertices[auiIndices[i + 0]];
		Vertex b = aoVertices[auiIndices[i + 1]];
		Vertex c = aoVertices[auiIndices[i + 2]];

		vec3 ap = vec3(a.x, a.y, a.z);
		vec3 bp = vec3(b.x, b.y, b.z);
		vec3 cp = vec3(c.x, c.y, c.z);

		vec3 dir = glm::cross(bp - ap, cp - ap);
		vec3 nor = glm::normalize(dir);

		a.nx += nor.x;
		b.nx += nor.x;
		c.nx += nor.x;
		a.ny += nor.y;
		b.ny += nor.y;
		c.ny += nor.y;
		a.nz += nor.z;
		b.nz += nor.z;
		c.nz += nor.z;
	}
	for (unsigned int i = 0; i < aoVertices.size(); i++)
	{
		vec3 norm = vec3(aoVertices[i].nx, aoVertices[i].ny, aoVertices[i].nz);

		norm = glm::normalize(norm);

		aoVertices[i].nx = norm.x;
		aoVertices[i].ny = norm.y;
		aoVertices[i].nz = norm.z;
	}
	
	//Set Mesh Pointer and return
	Mesh* newMesh = new VertexMesh(aoVertices.data(), auiIndices.data(), ((rows - 1)*(cols - 1) * 6), (rows * cols), false);
	return newMesh;
}

/*void Application3D::DrawDirectionalLight(const vec3 & direction, const vec3 & diffuse)
{
	vec4 viewSpaceLight = cam->getView() * vec4(glm::normalize(direction), 0);

	int loc = glGetUniformLocation(m_directionalShader->GetProgram(), "lightDirection");
	glUniform3fv(loc, 1, &viewSpaceLight[0]);

	loc = glGetUniformLocation(m_directionalShader->GetProgram(), "lightDiffuse");
	glUniform3fv(loc, 1, &diffuse[0]);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}*/
