#pragma once
#include <glm\glm.hpp>

class Light
{
public:
	struct Directional
	{
		Directional() {}
		glm::vec3 direction;

		glm::vec3 colour;
		float intensity;
	};
	struct Point
	{
		Point() {}
		glm::vec4 position;

		glm::vec3 colour;
		float intensity;

		float constant;
		float linear;
		float quadratic;
	};
	struct Spot
{
	Spot() {}
	glm::vec4 position;
	glm::vec3 direction;

	glm::vec3 colour;
	float intensity;

	float cutOff;
};
private:

};
