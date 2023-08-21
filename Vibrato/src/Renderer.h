#pragma once

#include "Clef/Image.h"

#include <glm/glm.hpp>
#include <memory>

namespace Vibrato
{
	class Renderer
	{
	public:
		Renderer() = default;

		void onResize(uint32_t width, uint32_t height);
		void render();

		std::shared_ptr<Clef::Image> getFinalImage() const { return m_finalImage; }


		glm::vec3 p_sphereColor = glm::vec3(0.1f, 0.6f, 1.0f);
		glm::vec3 p_lightDirection = glm::vec3(-1, -1, -1);
	
	private:
		glm::vec4 perPixel(glm::vec2 coord);

	private:
		std::shared_ptr<Clef::Image> m_finalImage;
		uint32_t* m_imageData = nullptr;
	};
}
