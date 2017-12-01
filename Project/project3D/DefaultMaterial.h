#pragma once
#include "Material.h"
class DefaultMaterial :
	public Material
{
public:
	DefaultMaterial();

	virtual void Activate();

	virtual bool SetProperty(const glm::vec2& value, const std::string& variableName);
	virtual bool SetProperty(const glm::vec3& value, const std::string& variableName);
	virtual bool SetProperty(const glm::vec4& value, const std::string& variableName);
	virtual bool SetProperty(const glm::mat4& value, const std::string& variableName);
	virtual bool SetProperty(const float value, const std::string& variableName);
	virtual bool SetProperty(const int value, const std::string& variableName);
	virtual bool SetProperty(const bool value, const std::string& variableName);
	virtual bool SetProperty(const unsigned int value, const std::string& variableName);
	virtual bool SetProperty(MyTexture* value, const std::string& variableName);
	virtual bool SetProperty(const Light::Directional& value);

	virtual ~DefaultMaterial();
private:
	Property<MyTexture*> textures[3]; //0 = diffuse, 1 = normal, 2 = specular
	Property<float> floats[3]; //0 = roughness, 1 = coefficient, 2 = ambient
	Property<Light::Directional> light;
	Property<glm::mat4> matrices[2]; //0 = projection, 1 = model
	Property<glm::vec3> vector3[3]; //0 = camPos, 1 = diffCol, 2 = specCol;
	Property<bool> bools[3]; //0 = diffSet, 1 = normSet, 2 = specSet
};

