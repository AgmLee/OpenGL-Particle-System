#include "ParticleSystemGPU.h"
#include "Transform.h"
#include "SceneGraph.h"
#include "MyTexture.h"
#include "Camera.h"
#include <gl_core_4_4.h>
#include <fstream>
#include <iostream>
#include <string>

static unsigned int CreateShader(const std::string& text, GLenum shaderType);
static void CheckShaderError(unsigned int shader, GLuint flag, bool isProgram, std::string errorMsg);
static std::string LoadShader(const std::string& fileName);

bool ParticleSystemGPU::Draw(float time, Camera* cam, bool drawParticles)
{
	if (m_preRenderCull)
	{
		glm::vec4 planes[6];
		cam->getFrustumPlanes(planes);
		m_sphere.centre = m_transform->m_positon;
		m_sphere.centre += (m_velocityMin + m_velocityMax) * 0.5f;
		if (m_useGravity)
			m_sphere.centre.y -= m_gravity;
		m_sphere.radius = glm::distance(m_velocityMin, m_velocityMax) * (m_startSize + m_endSize);
		for (unsigned int i = 0; i < 6; i++)
		{
			float d = glm::dot(glm::vec3(planes[i]), m_sphere.centre) + planes[i].w;
			if (d < -(m_sphere.radius))
			{
				return false;
			}
		}
	}

	if (m_startColour.a < 1 || m_endColour.a < 1)
	{
		glBlendFunc(m_lhsBlend, m_rhsBlend);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
	}

	glUseProgram(m_update);

	int loc = glGetUniformLocation(m_update, "time");
	glUniform1f(loc, time);

	float DT = time - m_lastDrawTime; m_lastDrawTime = time;

	loc = glGetUniformLocation(m_update, "deltaTime");
	glUniform1f(loc, DT);

	loc = glGetUniformLocation(m_update, "emitterPosition");
	glUniform3fv(loc, 1, &m_transform->m_positon[0]);

	loc = glGetUniformLocation(m_update, "lifeMin");
	glUniform1f(loc, m_lifespanMin);
	loc = glGetUniformLocation(m_update, "lifeMax");
	glUniform1f(loc, m_lifespanMax);

	loc = glGetUniformLocation(m_update, "velMin");
	glUniform3fv(loc, 1, &m_velocityMin[0]);
	loc = glGetUniformLocation(m_update, "velMax");
	glUniform3fv(loc, 1, &m_velocityMax[0]);

	loc = glGetUniformLocation(m_update, "gravity");
	glUniform1f(loc, m_gravity);
	loc = glGetUniformLocation(m_update, "useGravity");
	glUniform1i(loc, m_useGravity);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_VAO[m_activeBuffer]);

	unsigned int otherBuffer = (m_activeBuffer + 1) % 2;

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_VBO[otherBuffer]);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	if (drawParticles)
	{
		glUseProgram(m_draw);

		loc = glGetUniformLocation(m_draw, "projection");
		glUniformMatrix4fv(loc, 1, false, &cam->getProjectionView()[0][0]);

		loc = glGetUniformLocation(m_draw, "camTransform");
		glUniformMatrix4fv(loc, 1, false, &cam->getWorldTransform()[0][0]);

		loc = glGetUniformLocation(m_draw, "sizeStart");
		glUniform1f(loc, m_startSize);
		loc = glGetUniformLocation(m_draw, "sizeEnd");
		glUniform1f(loc, m_endSize);

		loc = glGetUniformLocation(m_draw, "colourStart");
		glUniform4fv(loc, 1, &m_startColour[0]);
		loc = glGetUniformLocation(m_draw, "colourEnd");
		glUniform4fv(loc, 1, &m_endColour[0]);

		loc = glGetUniformLocation(m_draw, "rotationStart");
		glUniform1f(loc, m_rotationStart);
		loc = glGetUniformLocation(m_draw, "rotationEnd");
		glUniform1f(loc, m_rotationEnd);

		loc = glGetUniformLocation(m_draw, "drawTri");
		glUniform1i(loc, m_drawTri);

		if (m_texture)
		{
			loc = glGetUniformLocation(m_draw, "diffuse");
			glUniform1i(loc, 0);
			loc = glGetUniformLocation(m_draw, "maskSet");
			glUniform1i(loc, true);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_texture->GetTexture());
		}
		else
		{
			loc = glGetUniformLocation(m_draw, "maskSet");
			glUniform1i(loc, false);
		}

		glBindVertexArray(m_VAO[otherBuffer]);
		glDrawArrays(GL_POINTS, 0, m_drawCount);
	}

	if (m_startColour.a < 1 || m_endColour.a < 1)
	{
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	m_activeBuffer = otherBuffer;
	return drawParticles;
}

void ParticleSystemGPU::ResizeAmount(unsigned int newMax)
{
	delete[] m_particles;

	glDeleteVertexArrays(2, m_VAO);
	glDeleteBuffers(2, m_VBO);

	m_maxParticles = newMax;
	m_particles = new GPUParticle[m_maxParticles];

	if (m_drawCount > m_maxParticles)
		m_drawCount = m_maxParticles;

	m_activeBuffer = 0;

	CreateBuffers();
}

Transform * ParticleSystemGPU::GetTransform() const
{
	return m_transform;
}

ParticleSystemGPU::ParticleSystemGPU()
{ 
	m_transform = new Transform();
	m_particles = nullptr;
	m_maxParticles = 0;
	m_VAO[0] = 0; 
	m_VAO[1] = 0; 
	m_VBO[0] = 0; 
	m_VBO[1] = 0;
	m_draw = 0;
	m_update = 0;
	m_activeBuffer = 0;
	m_drawCount = 0;
	m_lastDrawTime = 0.0f;
	SceneGraph::AddTransform(m_transform);
}
ParticleSystemGPU::~ParticleSystemGPU()
{
	delete[] m_particles;
	
	glDeleteVertexArrays(2, m_VAO);
	glDeleteBuffers(2, m_VBO);

	glDeleteProgram(m_draw);
	glDeleteProgram(m_update);
}

void ParticleSystemGPU::Initialize(unsigned int maxParticles, float lifeSpanMin, float lifeSpanMax, const glm::vec3& velocityMin, const glm::vec3& velocityMax, float startSize, float endSize, float rotationStart, float rotationEnd, const glm::vec4 & startColour, const glm::vec4 & endColour, MyTexture* texture, bool drawTri, bool useGravity, float gravity)
{
	m_startColour = startColour;
	m_endColour = endColour;
	
	m_startSize = startSize;
	m_endSize = endSize;
	
	m_rotationStart = rotationStart;
	m_rotationEnd = rotationEnd;
	
	m_lifespanMin = lifeSpanMin;
	m_lifespanMax = lifeSpanMax;
	
	m_velocityMin = velocityMin;
	m_velocityMax = velocityMax;
	
	m_gravity = gravity;
	m_useGravity = useGravity;
	
	m_texture = texture;
	m_drawTri = drawTri;

	m_maxParticles = maxParticles;
	m_drawCount = m_maxParticles;

	m_particles = new GPUParticle[m_maxParticles];

	m_activeBuffer = 0;

	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();
}

BoundingSphere ParticleSystemGPU::GetBoundingVolume() const
{
	return m_sphere;
}

void ParticleSystemGPU::CreateBuffers()
{
	glGenVertexArrays(2, m_VAO);
	glGenBuffers(2, m_VBO);

	glBindVertexArray(m_VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 28);
	
	glBindVertexArray(m_VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(GPUParticle), 0, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 28);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void ParticleSystemGPU::CreateUpdateShader()
{
	unsigned int vs = CreateShader(LoadShader("../bin/shaders/ParticleUpdate.vert"), GL_VERTEX_SHADER);
	
	m_update = glCreateProgram();
	glAttachShader(m_update, vs);
	
	const char* varyings[] = { "vPosition","vVelocity",
		"vLifetime", "vLifespan"};
	glTransformFeedbackVaryings(m_update, 4, varyings, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(m_update);
	CheckShaderError(m_update, GL_LINK_STATUS, true, "Failed linking.");
	CheckShaderError(m_update, GL_VALIDATE_STATUS, true, "Invalid.");

	glDeleteShader(vs);
}
void ParticleSystemGPU::CreateDrawShader()
{
	unsigned int vs = CreateShader(LoadShader("../bin/shaders/ParticleDraw.vert"), GL_VERTEX_SHADER);
	unsigned int gs = CreateShader(LoadShader("../bin/shaders/ParticleDraw.geom"), GL_GEOMETRY_SHADER);
	unsigned int fs = CreateShader(LoadShader("../bin/shaders/ParticleDraw.frag"), GL_FRAGMENT_SHADER);

	m_draw = glCreateProgram();
	glAttachShader(m_draw, vs);
	glAttachShader(m_draw, gs);
	glAttachShader(m_draw, fs);

	glLinkProgram(m_draw);
	CheckShaderError(m_draw, GL_LINK_STATUS, true, "Error: Program linking failed.");
	CheckShaderError(m_draw, GL_VALIDATE_STATUS, true, "Error: Program invalid.");

	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);
}

//Static functions
unsigned int CreateShader(const std::string & text, GLenum shaderType)
{
	unsigned int shader = glCreateShader(shaderType);

	if (shader == 0)
		std::cerr << "Error compiling shader type " << shaderType << std::endl;

	const GLchar* p[1];
	p[0] = text.c_str();
	GLint lengths[1];
	lengths[0] = text.length();

	glShaderSource(shader, 1, p, lengths);
	glCompileShader(shader);

	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!");

	return shader;
}
std::string LoadShader(const std::string& fileName)
{
	std::ifstream file;
	file.open((fileName).c_str());

	std::string output;
	std::string line;

	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);
			output.append(line + "\n");
		}
	}
	else
	{
		std::cerr << "Unable to Load Shader: " << fileName << std::endl;
	}

	return output;
}
void CheckShaderError(unsigned int shader, GLuint flag, bool isProgram, std::string errorMsg)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };
	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		if (isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		std::cerr << errorMsg << ": '" << error << "'" << std::endl;
	}
}
