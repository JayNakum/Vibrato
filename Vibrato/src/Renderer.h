#pragma once

#include "Clef/Image.h"

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include <memory>

class Renderer
{
public:
	Renderer() = default;

	void onResize(uint32_t width, uint32_t height);
	void render(const Scene& scene, const Camera& camera);

	std::shared_ptr<Clef::Image> getFinalImage() const { return m_finalImage; }

private:
	glm::vec4 traceRay(const Scene& scene, const Ray& ray);

private:
	std::shared_ptr<Clef::Image> m_finalImage;
	uint32_t* m_imageData = nullptr;

	const glm::vec4 clearColor = glm::vec4(0.07f, 0.07f, 0.07f, 1.0f);
};
