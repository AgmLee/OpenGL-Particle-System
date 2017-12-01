#include "ParticleCPUMaterial.h"
#include <iostream>

ParticleCPUMaterial::ParticleCPUMaterial()
{
	//Set variable for the particle shader series
	clippingMask.data = nullptr;
	clippingMask.variableName = "clippingMask";

	projectionView.data = glm::mat4();
	projectionView.variableName = "projectionView";

	clippingColour.data = glm::vec3();
	clippingColour.variableName = "clippingColour";

	sharpness.data = 0.1f;
	sharpness.variableName = "sharpness";

	bools[0].data = false;
	bools[0].variableName = "maskIsSet";
	bools[1].data = false;
	bools[1].variableName = "useMaskAlphaOnly";
}

void ParticleCPUMaterial::Activate()
{
	Material::Activate();
	unsigned int currentProgram = Material::GetCurrentProgram();
	Bind(bools[0], currentProgram);
	Bind(bools[1], currentProgram);
	Bind(projectionView, currentProgram);
	Bind(clippingMask, currentProgram);
	Bind(clippingColour, currentProgram);
}

bool ParticleCPUMaterial::SetProperty(const glm::mat4 & value, const std::string & variableName)
{
	if (variableName == projectionView.variableName)
	{
		projectionView.data = value;
		return true;
	}
	std::cerr << "Unkonwn Property: " << variableName.c_str() << std::endl;
	return false;
}
bool ParticleCPUMaterial::SetProperty(const bool value, const std::string & variableName)
{
	for (unsigned int i = 0; i < 2; i++)
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
bool ParticleCPUMaterial::SetProperty(MyTexture * value, const std::string & variableName)
{
	if (variableName == clippingMask.variableName)
	{
		clippingMask.data = value;
		return true;
	}
	std::cerr << "Unkonwn Property: " << variableName.c_str() << std::endl;
	return false;
}
bool ParticleCPUMaterial::SetProperty(const glm::vec3 & value, const std::string & variableName)
{
	if (variableName == clippingColour.variableName)
	{
		clippingColour.data = value;
		return true;
	}
	std::cerr << "Unkonwn Property: " << variableName.c_str() << std::endl;
	return false;
}
bool ParticleCPUMaterial::SetProperty(const float value, const std::string & variableName)
{
	if (variableName == sharpness.variableName)
	{
		sharpness.data = value;
		return true;
	}
	std::cerr << "Unkonwn Property: " << variableName.c_str() << std::endl;
	return false;
}
//Unused
bool ParticleCPUMaterial::SetProperty(const Light::Directional & value)
{
	std::cerr << "Unsed Property: " << this << std::endl;
	return true;
}
bool ParticleCPUMaterial::SetProperty(const glm::vec2 & value, const std::string & variableName)
{
	std::cerr << "Unsed Property: " << this << std::endl;
	return true;
}
bool ParticleCPUMaterial::SetProperty(const glm::vec4 & value, const std::string & variableName)
{
	std::cerr << "Unsed Property: " << this << std::endl;
	return true;
}
bool ParticleCPUMaterial::SetProperty(const int value, const std::string & variableName)
{
	std::cerr << "Unsed Property: " << this << std::endl;
	return true;
}
bool ParticleCPUMaterial::SetProperty(const unsigned int value, const std::string & variableName)
{
	std::cerr << "Unsed Property: " << this << std::endl;
	return true;
}

ParticleCPUMaterial::~ParticleCPUMaterial()
{
}