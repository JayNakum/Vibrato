#include "Renderer.h"

#include "Clef/Random.h"
#include "Utils.h"

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
		uint32_t seed = x + y * m_finalImage->getWidth();
		seed *= m_frameIndex;

		glm::vec3 light(0.0f);
		glm::vec3 contribution(1.0f); // throughput

		for (size_t s = 0 ; s < samplesPerPixel ; s++)
		{
			Ray ray;

			ray.origin = m_activeCamera->getPosition();
			
			float jx = samplesPerPixel > 1 ? (float)x + Utils::randomFloat(seed) : (float)x;
			float jy = samplesPerPixel > 1 ? (float)y + Utils::randomFloat(seed) : (float)y;

			ray.direction = m_activeCamera->getRayDirection(jx, jy);

			for (int i = 0; i < bounces; i++)
			{
				seed += i;

				Renderer::HitPayload payload = traceRay(ray);
				if (payload.hitDistance < 0)
				{
					float a = 0.5 * (glm::normalize(ray.direction).y + 1.0);
					glm::vec3 skyColor = (1.0f - a) * glm::vec3(1.0) + a * glm::vec3(0.5, 0.7, 1.0);
					// glm::vec3 skyColor = CLEAR_COLOR;
					light += skyColor * contribution;
					break;
				}

				const Sphere& closestSphere = m_activeScene->spheres[payload.objectIndex];
				const Material& material = m_activeScene->materials[closestSphere.materialIndex];

				contribution *= material.albedo;
				light += material.emission() * material.albedo;

				ray.origin = payload.position + payload.normal * 0.0001f;

				// ray.direction = glm::reflect(ray.direction, 
				// 								payload.normal + material.roughness * Clef::Random::Vec3(-0.5f, 0.5f));
				// ray.direction = glm::normalize(payload.normal + Clef::Random::InUnitSphere());
				ray.direction = glm::normalize(
					glm::reflect(
						ray.direction, 
						payload.normal + material.roughness * Utils::InUnitSphere(seed)
					) + material.fuzz * Utils::InUnitSphere(seed)
				);
				
				auto z = 1e-8;
				if ((fabs(ray.direction[0]) < z) && (fabs(ray.direction[1]) < z) && (fabs(ray.direction[2]) < z))
				{
					ray.direction = payload.normal;
				}
			}
		}

		float scale = 1.0f / samplesPerPixel;
		light.r = std::sqrt(scale * light.r);
		light.g = std::sqrt(scale * light.g);
		light.b = std::sqrt(scale * light.b);

		return glm::vec4(light, 1.0f);
	}

	Renderer::HitPayload Renderer::traceRay(const Ray& ray)
	{
		float hitDistance = std::numeric_limits<float>::max();
		int closestObject = -1;

		for (size_t i = 0; i < m_activeScene->spheres.size(); i++)
		{
			const Sphere& sphere = m_activeScene->spheres[i];
			
			float closestT = sphere.hit(ray);
			if (closestT > 0.0f && closestT < hitDistance)
			{
				hitDistance = closestT;
				closestObject = (int)i;
			}
		}

		if (closestObject < 0)
			return miss(ray);

		return closestHit(ray, hitDistance, closestObject);
	}

	Renderer::HitPayload Renderer::closestHit(const Ray& ray, float hitDistance, int objectIndex)
	{
		Renderer::HitPayload payload;
		payload.hitDistance = hitDistance;
		payload.objectIndex = objectIndex;

		const Hittable& closestObject = m_activeScene->spheres[objectIndex];

		glm::vec3 origin = ray.origin - closestObject.position;

		payload.position = origin + ray.direction * hitDistance;
		payload.normal = glm::normalize(payload.position);

		payload.position += closestObject.position;

		return payload;
	}

	Renderer::HitPayload Renderer::miss(const Ray& ray)
	{
		Renderer::HitPayload payload;
		payload.hitDistance = -1.0f;
		return payload;
	}
}
