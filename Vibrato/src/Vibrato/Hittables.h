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
		float hit(const Ray& ray) const override
		{
			glm::vec3 origin = ray.origin - position;

			float a = glm::dot(ray.direction, ray.direction);
			float half_b = glm::dot(origin, ray.direction);
			float c = glm::dot(origin, origin) - (radius * radius);

			float discriminant = half_b * half_b - a * c;

			// if (discriminant < 0.0f)
			// 	continue;

			// float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
			return ((-half_b - glm::sqrt(discriminant)) / a);
		}
	};
}