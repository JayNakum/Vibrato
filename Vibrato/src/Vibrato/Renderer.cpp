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


namespace Vibrato
{
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

	void Renderer::render(const Scene& scene, const Camera& camera)
	{
		m_activeScene = &scene;
		m_activeCamera = &camera;

		for (uint32_t y = 0; y < m_finalImage->getHeight(); y++)
		{
			for (uint32_t x = 0; x < m_finalImage->getWidth(); x++)
			{
				glm::vec4 color = perPixel(x, y);
				color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
				m_imageData[x + y * m_finalImage->getWidth()] = Utils::convertToRGBA(color);
			}
		}

		m_finalImage->setData(m_imageData);
	}

	glm::vec4 Renderer::perPixel(uint32_t x, uint32_t y)
	{
		Ray ray;
		ray.origin = m_activeCamera->getPosition();
		ray.direction = m_activeCamera->getRayDirections()[x + y * m_finalImage->getWidth()];

		glm::vec3 color(0.0f);
		float multiplier = 1.0f;

		int bounces = 2;
		for (int i = 0; i < bounces; i++)
		{
			Renderer::HitPayload payload = traceRay(ray);
			if (payload.hitDistance < 0)
			{
				glm::vec3 skyColor = CLEAR_COLOR;
				color += skyColor * multiplier;
				break;
			}

			glm::vec3 lightDirection = glm::normalize(glm::vec3(-1));
			float lightIntensity = glm::max(glm::dot(payload.worldNormal, -lightDirection), 0.0f);

			const Sphere& closestSphere = m_activeScene->spheres[payload.objectIndex];
			glm::vec3 sphereColor = closestSphere.albedo;
			sphereColor *= lightIntensity;
			color += sphereColor * multiplier;

			multiplier *= 0.7f;


			ray.origin = payload.worldPosition + payload.worldNormal * 0.0001f;
			ray.direction = glm::reflect(ray.direction, payload.worldNormal);
		}

		return glm::vec4(color, 1.0f);
	}

	Renderer::HitPayload Renderer::traceRay(const Ray& ray)
	{
		float hitDistance = std::numeric_limits<float>::max();
		int closestSphere = -1;

		for (size_t i = 0; i < m_activeScene->spheres.size(); i++)
		{
			const Sphere& sphere = m_activeScene->spheres[i];
			glm::vec3 origin = ray.origin - sphere.position;

			float a = glm::dot(ray.direction, ray.direction);
			float b = 2.0f * glm::dot(origin, ray.direction);
			float c = glm::dot(origin, origin) - (sphere.radius * sphere.radius);

			float discriminant = b * b - 4 * a * c;

			if (discriminant < 0.0f)
				continue;

			// float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
			float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
			if (closestT > 0.0f && closestT < hitDistance)
			{
				hitDistance = closestT;
				closestSphere = (int)i;
			}

		}

		if (closestSphere < 0)
			return miss(ray);

		return closestHit(ray, hitDistance, closestSphere);
	}

	Renderer::HitPayload Renderer::closestHit(const Ray& ray, float hitDistance, int objectIndex)
	{
		Renderer::HitPayload payload;
		payload.hitDistance = hitDistance;
		payload.objectIndex = objectIndex;

		const Sphere& closestSphere = m_activeScene->spheres[objectIndex];

		glm::vec3 origin = ray.origin - closestSphere.position;

		payload.worldPosition = origin + ray.direction * hitDistance;
		payload.worldNormal = glm::normalize(payload.worldPosition);

		payload.worldPosition += closestSphere.position;

		return payload;
	}

	Renderer::HitPayload Renderer::miss(const Ray& ray)
	{
		Renderer::HitPayload payload;
		payload.hitDistance = -1.0f;
		return payload;
	}
}
