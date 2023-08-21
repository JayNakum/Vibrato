#include "Renderer.h"

namespace Utils
{
	static uint32_t convertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;

		return result;
	}
}

void Renderer::onResize(uint32_t width, uint32_t height)
{

	if (m_finalImage)
	{
		if (m_finalImage->getWidth() == width && m_finalImage->getHeight() == height)
			return;
		
		m_finalImage->resize(width, height);
	}
	else
	{
		m_finalImage = std::make_shared<Clef::Image>(width, height, Clef::ImageFormat::RGBA);
	}
	
	delete[] m_imageData;
	m_imageData = new uint32_t[width * height];
}

void Renderer::render(const Camera& camera)
{
	Ray ray;
	ray.origin = camera.getPosition();

	for (uint32_t y = 0; y < m_finalImage->getHeight(); y++)
	{
		for (uint32_t x = 0; x < m_finalImage->getWidth(); x++)
		{
			ray.direction = camera.getRayDirections()[x + y * m_finalImage->getWidth()];
			glm::vec4 color = traceRay(ray);

			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_imageData[x + y * m_finalImage->getWidth()] = Utils::convertToRGBA(color);
		}
	}

	m_finalImage->setData(m_imageData);
}


glm::vec4 Renderer::traceRay(const Ray& ray)
{
	float radius = 0.5f;

	float a = glm::dot(ray.direction, ray.direction);
	float b = 2.0f * glm::dot(ray.origin, ray.direction);
	float c = glm::dot(ray.origin, ray.origin) - (radius * radius);

	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0.0f)
		return glm::vec4(0, 0, 0, 1);
	

	float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
	float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

	glm::vec3 hitPoint = ray.origin + ray.direction * closestT;
	glm::vec3 normal = glm::normalize(hitPoint);


	glm::vec3 lightDirection(p_lightDirection);
	lightDirection = glm::normalize(lightDirection);

	float d = glm::max(glm::dot(normal, -lightDirection), 0.0f);

	glm::vec3 sphereColor(p_sphereColor);
	sphereColor *= d;
	return glm::vec4(sphereColor, 1.0f);
}
