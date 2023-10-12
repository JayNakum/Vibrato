#pragma once

#include <glm/vec3.hpp>

struct HitPayload
{
	glm::vec3 position;
	glm::vec3 normal;
	float hitDistance;
	bool frontFace;
	glm::vec3 AOV;
	int objectIndex;
};
