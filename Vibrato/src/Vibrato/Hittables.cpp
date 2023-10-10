#include "Hittables.h"

#include <iostream>

namespace Vibrato
{
	float Sphere::intersect(const Ray& ray) const
	{
		glm::vec3 origin = ray.origin - position;

		float a = glm::dot(ray.direction, ray.direction);
		float half_b = glm::dot(origin, ray.direction);
		float c = glm::dot(origin, origin) - (radius * radius);

		float discriminant = half_b * half_b - a * c;

		/*if (discriminant < 0.0f)
		 	return false;*/

		// float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
		return ((-half_b - glm::sqrt(discriminant)) / a);
	}

	void Sphere::setHitPayload(const Ray& ray, HitPayload& payload) const
	{
		glm::vec3 outwardNormal = (payload.position - position) / radius;
		payload.frontFace = glm::dot(ray.direction, outwardNormal) < 0;
		payload.normal = payload.frontFace ? outwardNormal : -outwardNormal;
	}
}

