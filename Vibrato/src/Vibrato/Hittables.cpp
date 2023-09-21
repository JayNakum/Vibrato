#include "Hittables.h"

float Vibrato::Sphere::hit(const Ray& ray) const
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
