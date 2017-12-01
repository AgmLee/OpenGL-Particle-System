#include "DefaultMaterial.h"
#include <iostream>

DefaultMaterial::DefaultMaterial()
{
	//Set variable for the default shader series
	//Textures
	textures[0].variableName = "diffuseMap.textureMap";
	textures[0].data = nullptr;
	textures[1].variableName = "normalMap.textureMap";
	textures[1].data = nullptr;
	textures[2].variableName = "specularMap.textureMap";
	textures[2].data = nullptr;
	//Floats
	floats[0].variableName = "roughness";
	floats[0].data = 0.5f;
	floats[1].variableName = "reflectionCoefficient";
	floats[1].data = 0.5f;
	floats[2].variableName = "ambientValue";
	floats[2].data = 0.5f;
	//Light
	light.variableName = "lights";
	//Matrix4s
	matrices[0].variableName = "projectionMatrix";
	matrices[0].data = glm::mat4();
	matrices[1].variableName = "modelMatrix";
	matrices[1].data = glm::mat4();
	//Vector3s
	vector3[0].variableName = "camPos";
	vector3[0].data = glm::vec3(0);
	vector3[1].variableName = "diffuseMap.colour";
	vector3[1].data = glm::vec3(1);
	vector3[2].variableName = "specularMap.colour";
	vector3[2].data = glm::vec3(1);
	//Bools
	bools[0].variableName = "diffuseMap.isSet";
	bools[0].data = false;
	bools[1].variableName = "normalMap.isSet";
	bools[1].data = false;
	bools[2].variableName = "specularMap.isSet";
	bools[2].data = false;
}

void DefaultMaterial::Activate()
{
	Material::Activate();
	//Get Current active program
	unsigned int currentProgram = Material::GetCurrentProgram();
	//Bind properties
	for (unsigned int i = 0; i < 3; i++)
	{
		Bind(textures[i], currentProgram);
		Bind(floats[i], currentProgram);
		Bind(vector3[i], currentProgram);
		Bind(bools[i], currentProgram);
		if (i > 1)
			Bind(light, currentProgram);
		if (i > 0)
			Bind(matrices[i - 1], currentProgram);
	}
}

//Used Properties
bool DefaultMaterial::SetProperty(const glm::vec3 & value, const std::string & variableName)
{
	for (unsigned int i = 0; i < 3; i++)
	{
		if (variableName == vector3[i].variableName)
		{
			vector3[i].data = value;
			return true;
		}
	}
	std::cerr << "Unkonwn Property: " << variableName.c_str() << std::endl;
	return false;
}
bool DefaultMaterial::SetProperty(const glm::mat4 & value, const std::string & variableName)
{
	for (unsigned int i = 0; i < 2; i++)
	{
		if (variableName == matrices[i].variableName)
		{
			matrices[i].data = value;
			return true;
		}
	}
	std::cerr << "Unkonwn Property: " << variableName.c_str() << std::endl;
	return false;
}
bool DefaultMaterial::SetProperty(const float value, const std::string & variableName)
{
	for (unsigned int i = 0; i < 3; i++)
	{
		if (variableName == floats[i].variableName)
		{
			floats[i].data = value;
			return true;
		}
	}
	std::cerr << "Unkonwn Property: " << variableName.c_str() << std::endl;
	return false;
}
bool DefaultMaterial::SetProperty(const bool value, const std::string & variableName)
{
	for (unsigned int i = 0; i < 3; i++)
	{
		if (variableName == bools[i].variableName)
		{
			bools[i].data = value;
			return true;
		}
	}
	std::cerr << "Unkonwn Property: " << variableName.c_str() << std::endl;
	return false;
}
bool DefaultMaterial::SetProperty(MyTexture * value, const std::string & variableName)
{
	for (unsigned int i = 0; i < 3; i++)
	{
		if (variableName == textures[i].variableName)
		{
			textures[i].data = value;
			return true;
		}
	}
	std::cerr << "Unkonwn Property: " << variableName.c_str() << std::endl;
	return false;
}
bool DefaultMaterial::SetProperty(const Light::Directional & value)
{
	light.data = value;
	return true;
}

//Unused Properties
bool DefaultMaterial::SetProperty(const glm::vec2 & value, const std::string & variableName)
{
	std::cerr << "Unsed Property: " << this << std::endl;
	return true;
}
bool DefaultMaterial::SetProperty(const glm::vec4 & value, const std::string & variableName)
{
	std::cerr << "Unsed Property: " << this << std::endl;
	return true;
}
bool DefaultMaterial::SetProperty(const int value, const std::string & variableName)
{
	std::cerr << "Unsed Property: " << this << std::endl;
	return true;
}
bool DefaultMaterial::SetProperty(const unsigned int value, const std::string & variableName)
{
	std::cerr << "Unsed Property: " << this << std::endl;
	return true;
}

DefaultMaterial::~DefaultMaterial()
{
}
