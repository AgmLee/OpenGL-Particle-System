#pragma once
#include <glm\glm.hpp>
#include <gl_core_4_4.h>
#include <string>

//This macro is the path of the basicShaders.
#define BASIC_SHADER_LOCATION	"..\\bin\\shaders\\basicShader"

class Shader
{
public:
	Shader(const std::string& fileName);

	unsigned int GetProgram() { return m_program; }

	virtual ~Shader();
private:
	static const unsigned int NUM_SHADERS = 2;

	GLuint m_program;
	GLuint m_shaders[NUM_SHADERS];
};

