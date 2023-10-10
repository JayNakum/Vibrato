#pragma once

#include <glm/glm.hpp>

#include "Ray.h"
#include "Vertex.h"
#include "HitPayload.h"

#include <memory>

namespace Vibrato
{
	class Hittable
	{
	public:
		glm::vec3 position{ 0.0f };
		glm::vec3 normal{ 0.0f };
		int materialIndex = 0;

		virtual ~Hittable() = default;
		virtual float intersect(const Ray& ray) const = 0;
		virtual void setHitPayload(const Ray& ray, HitPayload& payload) const = 0;
		// virtual glm::vec3 getBaryCentric(glm::vec3& p) const { return p; };
	};

	class Sphere : public Hittable
	{
	public:
		float radius = 0.5f;

	public:
		float intersect(const Ray& ray) const override;
		void setHitPayload(const Ray& ray, HitPayload& payload) const override;
	};
}