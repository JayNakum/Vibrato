#include "Renderer.h"

#include "Clef/Random.h"

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

		delete[] m_accumulationData;
		m_accumulationData = new glm::vec4[width * height];

		m_imgHorizontalIter.resize(width);
		m_imgVerticalIter.resize(height);

		for (uint32_t i = 0; i < width; i++)
			m_imgHorizontalIter[i] = i;

		for (uint32_t i = 0; i < height; i++)
			m_imgVerticalIter[i] = i;
	}

	void Renderer::render(const Scene& scene, const Camera& camera)
	{
		m_activeScene = &scene;
		m_activeCamera = &camera;

		if (m_frameIndex == 1)
			memset(m_accumulationData, 0, m_finalImage->getWidth() * m_finalImage->getHeight() * sizeof(glm::vec4));

#define MT 1
#if MT

		std::for_each(std::execution::par, m_imgVerticalIter.begin(), m_imgVerticalIter.end(),
		[this](uint32_t y)
		{
	#if 0
			std::for_each(std::execution::par, m_imgHorizontalIter.begin(), m_imgHorizontalIter.end(),
			[this, y](uint32_t x)
			{
				glm::vec4 color = perPixel(x, y);
				m_accumulationData[x + y * m_finalImage->getWidth()] += color;

				glm::vec4 accumulatedColor = m_accumulationData[x + y * m_finalImage->getWidth()];
				accumulatedColor /= (float)m_frameIndex;

				accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
				m_imageData[x + y * m_finalImage->getWidth()] = Utils::convertToRGBA(accumulatedColor);
			});
	#else
			for (uint32_t x = 0; x < m_finalImage->getWidth(); x++)
			{
				glm::vec4 color = perPixel(x, y);
				m_accumulationData[x + y * m_finalImage->getWidth()] += color;

				glm::vec4 accumulatedColor = m_accumulationData[x + y * m_finalImage->getWidth()];
				accumulatedColor /= (float)m_frameIndex;

				accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
				m_imageData[x + y * m_finalImage->getWidth()] = Utils::convertToRGBA(accumulatedColor);
			}
		});
	#endif

#else

		for (uint32_t y = 0; y < m_finalImage->getHeight(); y++)
		{
			for (uint32_t x = 0; x < m_finalImage->getWidth(); x++)
			{
				glm::vec4 color = perPixel(x, y);
				m_accumulationData[x + y * m_finalImage->getWidth()] += color;

				glm::vec4 accumulatedColor = m_accumulationData[x + y * m_finalImage->getWidth()];
				accumulatedColor /= (float)m_frameIndex;

				accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
				m_imageData[x + y * m_finalImage->getWidth()] = Utils::convertToRGBA(accumulatedColor);
			}
		}

#endif


		m_finalImage->setData(m_imageData);

		if (m_settings.accumulate)
			m_frameIndex++;
		else
			m_frameIndex = 1;
	}

	glm::vec4 Renderer::perPixel(uint32_t x, uint32_t y)
	{
		Ray ray;
		ray.origin = m_activeCamera->getPosition();
		ray.direction = m_activeCamera->getRayDirections()[x + y * m_finalImage->getWidth()];

		glm::vec3 color(0.0f);
		float multiplier = 1.0f;

		int bounces = 5;
		for (int i = 0; i < bounces; i++)
		{
			Renderer::HitPayload payload = traceRay(ray);
			if (payload.hitDistance < 0)
			{
				glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
				color += skyColor * multiplier;
				break;
			}

			glm::vec3 lightDirection = glm::normalize(glm::vec3(-1));
			float lightIntensity = glm::max(glm::dot(payload.worldNormal, -lightDirection), 0.0f);

			const Sphere& closestSphere = m_activeScene->spheres[payload.objectIndex];
			const Material& material = m_activeScene->materials[closestSphere.materialIndex];


			glm::vec3 sphereColor = material.albedo;
			sphereColor *= lightIntensity;
			color += sphereColor * multiplier;

			multiplier *= 0.5f;

			ray.origin = payload.worldPosition + payload.worldNormal * 0.0001f;
			ray.direction = glm::reflect(ray.direction, 
											payload.worldNormal + material.roughness * Clef::Random::Vec3(-0.5f, 0.5f));
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
