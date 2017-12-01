#pragma once
#include "Material.h"
class ParticleCPUMaterial :
	public Material
{
public:
	ParticleCPUMaterial();

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

	virtual ~ParticleCPUMaterial();
private:
	Property<MyTexture*> clippingMask;
	Property<glm::mat4> projectionView;
	Property<glm::vec3> clippingColour;
	Property<float> sharpness;
	Property<bool> bools[2]; //0 = isMaskSet, 1 = useMaskAlphaOnly
};

