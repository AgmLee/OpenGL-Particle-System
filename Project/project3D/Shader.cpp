#include "Shader.h"
#include <iostream>
#include <fstream>
#include <glm\gtc\type_ptr.hpp>

static GLuint CreateShader(const std::string& text, GLenum shaderType);
static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, std::string errorMsg);
static std::string LoadShader(const std::string& fileName);

Shader::Shader(const std::string & fileName)
{
	m_program = glCreateProgram();

	m_shaders[0] = CreateShader(LoadShader(fileName + ".vert"), GL_VERTEX_SHADER);
	m_shaders[1] = CreateShader(LoadShader(fileName + ".frag"), GL_FRAGMENT_SHADER);

	for (unsigned int i = 0; i < NUM_SHADERS; i++)
		glAttachShader(m_program, m_shaders[i]);

	glBindAttribLocation(m_program, 0, "position");

	glLinkProgram(m_program);
	CheckShaderError(m_program, GL_LINK_STATUS, true, "Error: Program linking failed.");

	CheckShaderError(m_program, GL_VALIDATE_STATUS, true, "Error: Program invalid.");
}

Shader::~Shader()
{
	for (unsigned int i = 0; i < NUM_SHADERS; i++)
	{
		glDetachShader(m_program, m_shaders[i]);
		glDeleteShader(m_shaders[i]);
	}
	glDeleteProgram(m_program);
}


GLuint CreateShader(const std::string & text, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);

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

void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, std::string errorMsg)
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
