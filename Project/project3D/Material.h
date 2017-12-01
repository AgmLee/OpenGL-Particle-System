#pragma once
#include "Light.h"
#include <vector>

class MyTexture;
class Shader;
class Transform;

class Material
{
public:
	Material(Shader* shader = nullptr);

	template<typename T>
	struct Property
	{
		Property() {}
		T data;
		std::string variableName;
	};

	virtual bool SetProperty(const glm::vec2& value, const std::string& variableName) = 0;
	virtual bool SetProperty(const glm::vec3& value, const std::string& variableName) = 0;
	virtual bool SetProperty(const glm::vec4& value, const std::string& variableName) = 0;
	virtual bool SetProperty(const glm::mat4& value, const std::string& variableName) = 0;
	virtual bool SetProperty(const float value, const std::string& variableName) = 0;
	virtual bool SetProperty(const int value, const std::string& variableName) = 0;
	virtual bool SetProperty(const bool value, const std::string& variableName) = 0;
	virtual bool SetProperty(const unsigned int value, const std::string& variableName) = 0;
	virtual bool SetProperty(MyTexture* value, const std::string& variableName) = 0;
	virtual bool SetProperty(const Light::Directional& value) = 0;

	void SetShader(Shader* shader);
	const Shader* GetShader() const;

	virtual void Activate();

	static const unsigned int GetCurrentProgram();
	static void SetCurrentProgram(unsigned int currentActive);
	static bool IsCurrent(unsigned int otherProgram);
	static void DeactivateAll();

	virtual ~Material();
protected:
	void Bind(const Material::Property<glm::vec2>& properties, unsigned int& program);
	void Bind(const Material::Property<glm::vec3>& properties, unsigned int& program);
	void Bind(const Material::Property<glm::vec4>& properties, unsigned int& program);
	void Bind(const Material::Property<glm::mat4>& properties, unsigned int& program);
	void Bind(const Material::Property<float>& properties, unsigned int& program);
	void Bind(const Material::Property<int>& properties, unsigned int& program);
	void Bind(const Material::Property<bool>& properties, unsigned int& program);
	void Bind(const Material::Property<unsigned int>& properties, unsigned int& program);
	void Bind(const Material::Property<MyTexture*>& properties, unsigned int& program);
	void Bind(const Material::Property<Light::Directional>& properties, unsigned int& program);

	Shader* m_shader;
	
	static unsigned int sm_currentProgram;
};