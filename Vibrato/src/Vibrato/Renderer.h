#pragma once

#include "Clef/Image.h"

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <memory>

namespace Vibrato
{
	class Renderer
	{
	public:
		Renderer() = default;

		void onResize(uint32_t width, uint32_t height);
		void render(const Scene& scene, const Camera& camera);

		std::shared_ptr<Clef::Image> getFinalImage() const { return m_finalImage; }

	private:
		struct HitPayload
		{
			float hitDistance;
			glm::vec3 worldPosition;
			glm::vec3 worldNormal;

			int objectIndex;
		};

		glm::vec4 perPixel(uint32_t x, uint32_t y); // RayGen Shader

		HitPayload traceRay(const Ray& ray);
		HitPayload closestHit(const Ray& ray, float hitDistance, int objectIndex); // ClosestHit Shader
		HitPayload miss(const Ray& ray); // Miss Shader

	private:
		std::shared_ptr<Clef::Image> m_finalImage;
		uint32_t* m_imageData = nullptr;

		const Scene* m_activeScene = nullptr;
		const Camera* m_activeCamera = nullptr;

		const glm::vec4 CLEAR_COLOR = glm::vec4(0.07f, 0.07f, 0.07f, 1.0f);
	};
}
