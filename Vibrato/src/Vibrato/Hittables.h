#pragma once

#include "Ray.h"

#include <glm/glm.hpp>

namespace Vibrato
{
	class Hittable
	{
	public:
		glm::vec3 position{ 0.0f };
		int materialIndex = 0;

		virtual ~Hittable() = default;
		virtual float hit(const Ray& ray) const { return -1.0f; }
	};

	class Sphere : public Hittable
	{
	public:
		float radius = 0.5f;

	public:
		float hit(const Ray& ray) const override;
	};
}