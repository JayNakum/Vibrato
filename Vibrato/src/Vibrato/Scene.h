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

		float refractiveIndex = 0.0f;

		glm::vec3 emissionColor{ 1.0f };
		float emissionPower = 0.0f;

		glm::vec3 emission() const { return emissionColor * emissionPower; }

		void reset()
		{
			roughness = 1.0f;
			fuzz = 0.0f;

			refractiveIndex = 0.0f;

			emissionColor.r = 1.0f;
			emissionColor.g = 1.0f;
			emissionColor.b = 1.0f;

			emissionPower = 0.0f;
		}
	};

	struct Scene
	{
		std::vector<Sphere> spheres;
		std::vector<Material> materials;
	};
}
