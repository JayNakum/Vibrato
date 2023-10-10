#pragma once

#include <glm/glm.hpp>

// #include "tiny_obj_loader.h"
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
		virtual bool intersect(const Ray& ray, HitPayload& payload) const = 0;
	};

	class Sphere : public Hittable
	{
	public:
		float radius = 0.5f;

	public:
		bool intersect(const Ray& ray, HitPayload& payload) const override;
	};

	class Triangle : public Hittable
	{
	public:
		glm::vec3 v0, v1, v2;
		glm::vec3 e1, e2;
		glm::vec3 n;
		glm::vec3 N[3];
		glm::vec2 uv[3];

	public:
		Triangle();
		Triangle(Vertex _v0, Vertex _v1, Vertex _v2);
		bool intersect(const Ray& ray, HitPayload& payload) const override;
		glm::vec3 getBaryCentric(glm::vec3& p) const;
	};

	/*class TriangleMesh : public Hittable
	{
	public:
		TriangleMesh(const char* filepath);
		bool intersect(const Ray& r, HitPayload& payload) const override;
	public:
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::vector<std::shared_ptr<Triangle>> tris;
	};*/
}