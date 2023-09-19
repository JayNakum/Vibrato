#pragma once

#include "Clef/Image.h"

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <memory>
#include <execution>

namespace Vibrato
{
	class Renderer
	{
	public:
		struct Settings
		{
			bool accumulate = true;
		};

	public:
		Renderer() = default;

		void onResize(uint32_t width, uint32_t height);
		void render(const Scene& scene, const Camera& camera);

		std::shared_ptr<Clef::Image> getFinalImage() const { return m_finalImage; }

		void resetFrameIndex() { m_frameIndex = 1; }

		Settings& getSettings() { return m_settings; }

	private:
		struct HitPayload
		{
			glm::vec3 position;
			glm::vec3 normal;
			float hitDistance;

			int objectIndex;
		};

		glm::vec4 perPixel(uint32_t x, uint32_t y); // RayGen Shader

		HitPayload traceRay(const Ray& ray);
		HitPayload closestHit(const Ray& ray, float hitDistance, int objectIndex); // ClosestHit Shader
		HitPayload miss(const Ray& ray); // Miss Shader

	private:
		std::shared_ptr<Clef::Image> m_finalImage;
		uint32_t* m_imageData = nullptr;

		std::vector<uint32_t> m_imgHorizontalIter, m_imgVerticalIter;

		Settings m_settings;
		glm::vec4* m_accumulationData = nullptr;

		const Scene* m_activeScene = nullptr;
		const Camera* m_activeCamera = nullptr;

		const glm::vec4 CLEAR_COLOR = glm::vec4(0.07f, 0.07f, 0.07f, 1.0f);

		uint32_t m_frameIndex = 1;
	};
}
