#pragma once
#include <glm\glm.hpp>
#include <vector>

class BoundingSphere
{
public:
	BoundingSphere();

	void fit(const std::vector<glm::vec3>& points);

	~BoundingSphere();
	float radius;
	float radOffset;
	glm::vec3 centre;
	glm::vec3 offset;
};

