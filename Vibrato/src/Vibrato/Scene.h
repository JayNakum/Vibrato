#pragma once

#include "Hittables.h"

#include <glm/glm.hpp>

#include <vector>

namespace Vibrato
{
	struct Material
	{
		glm::vec3 albedo{ 1.0f };
		float roughness = 1.0f;
		float fuzz = 0.0f;

		glm::vec3 emissionColor{ 0.0f };
		float emissionPower = 0.0f;

		glm::vec3 emission() const { return emissionColor * emissionPower; }
	};

	struct Scene
	{
		std::vector<Sphere> spheres;
		std::vector<Material> materials;
	};
}
