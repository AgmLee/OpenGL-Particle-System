#include "Material.h"
#include "Shader.h"
#include "MyTexture.h"
#include <iostream>
#include <gl_core_4_4.h>
#include <glm\gtc\type_ptr.hpp>

//Forward Declarations
unsigned int Material::sm_currentProgram = 0;

//Class Functions
Material::Material(Shader * shader)
{
	m_shader = shader;
}

void Material::SetShader(Shader * shader)
{
	m_shader = shader;
}
const Shader * Material::GetShader() const
{
	return m_shader;
}

void Material::Activate()
{
	if (m_shader != nullptr)
	{
		unsigned int shader = m_shader->GetProgram();
		if (!IsCurrent(shader))
		{
			glUseProgram(shader);
			SetCurrentProgram(shader);
		}
	}
}

void Material::Bind(const Material::Property<glm::vec2>& prop, unsigned int& program)
{
	unsigned int loc = glGetUniformLocation(program, prop.variableName.c_str());
	glUniform2f(loc, prop.data.x, prop.data.y);
}
void Material::Bind(const Material::Property<glm::vec3>& prop, unsigned int& program)
{
	unsigned int loc = glGetUniformLocation(program, prop.variableName.c_str());
	glUniform3f(loc, prop.data.x, prop.data.y, prop.data.z);
}
void Material::Bind(const Material::Property<glm::vec4>& prop, unsigned int& program)
{
	unsigned int loc = glGetUniformLocation(program, prop.variableName.c_str());
	glUniform4f(loc, prop.data.x, prop.data.y, prop.data.z, prop.data.w);
}
void Material::Bind(const Material::Property<glm::mat4>& prop, unsigned int& program)
{
	unsigned int loc = glGetUniformLocation(program, prop.variableName.c_str());
	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(prop.data));
}
void Material::Bind(const Material::Property<float>& prop, unsigned int& program)
{
	unsigned int loc = glGetUniformLocation(program, prop.variableName.c_str());
	glUniform1f(loc, prop.data);
}
void Material::Bind(const Material::Property<int>& prop, unsigned int& program)
{
	unsigned int loc = glGetUniformLocation(program, prop.variableName.c_str());
	glUniform1i(loc, prop.data);
}
void Material::Bind(const Material::Property<bool>& prop, unsigned int& program)
{
	unsigned int loc = glGetUniformLocation(program, prop.variableName.c_str());
	glUniform1i(loc, prop.data);
}
void Material::Bind(const Material::Property<unsigned int>& prop, unsigned int& program)
{
	unsigned int loc = glGetUniformLocation(program, prop.variableName.c_str());
	glUniform1ui(loc, prop.data);
}
void Material::Bind(const Material::Property<MyTexture*>& prop, unsigned int& program)
{
	MyTexture* texture = prop.data;
	if (texture)
	{
		glActiveTexture(texture->GetSpot());
		glBindTexture(GL_TEXTURE_2D, texture->GetTexture());
		unsigned int loc = glGetUniformLocation(program, prop.variableName.c_str());
		glUniform1ui(loc, texture->GetLocation());
	}
}
void Material::Bind(const Material::Property<Light::Directional>& prop, unsigned int& program)
{
	unsigned int loc = glGetUniformLocation(program, "lights.position");
	glUniform4f(loc, prop.data.direction.x, prop.data.direction.y, prop.data.direction.z, 0);
	// Light Colour + Light Intensity
	loc = glGetUniformLocation(program, "lights.colour");
	glUniform4f(loc, prop.data.colour.r, prop.data.colour.g, prop.data.colour.b, prop.data.intensity);
}

Material::~Material()
{
}

//Static Functions
const unsigned int Material::GetCurrentProgram()
{
	return sm_currentProgram;
}
void Material::SetCurrentProgram(unsigned int currentActive)
{
	sm_currentProgram = currentActive;
}
void Material::DeactivateAll()
{
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	SetCurrentProgram(0);
}
bool Material::IsCurrent(unsigned int otherProgram)
{
	if (otherProgram == sm_currentProgram)
		return true;
	else
		return false;
}