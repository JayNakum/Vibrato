#pragma once

#include <glm/vec3.hpp>

struct HitPayload
{
	glm::vec3 position;
	float hitDistance;
	glm::vec3 normal;
	bool frontFace;
	glm::vec3 AOV;
	int objectIndex;
};
