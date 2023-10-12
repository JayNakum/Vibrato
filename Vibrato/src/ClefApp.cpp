#include "Clef.h"

#include "Vibrato/Renderer.h"
#include "Vibrato/Utils.h"

#include <memory>
#include <glm/gtc/type_ptr.hpp>

using namespace Clef;

class VibratoLayer : public Clef::Layer
{
public:
	VibratoLayer()
		: m_camera(45.0f, 0.1f, 100.0f) 
	{	
		/*
		Vibrato::Material& groundMaterial = m_scene.materials.emplace_back();
		groundMaterial.albedo = { 0.5f, 0.5f, 0.5f };
		{
			auto& sphere = std::make_shared<Vibrato::Sphere>();
			sphere->position = { 0.0f, -1000.0f, 0.0f };
			sphere->radius = 1000.0f;
			sphere->materialIndex = (int)(m_scene.materials.size() - 1);
			m_scene.objects.push_back(sphere);
		}
		
		//{
		//	const int x = 3;
		//	for (int a = -x; a < x; a++)
		//	{
		//		uint32_t seed = a * 2003;
		//		for (int b = -x; b < x; b++)
		//		{
		//			float chooseMat = Utils::randomFloat(seed);
		//			glm::vec3 pos{ a + 0.9 * Utils::randomFloat(seed), 0.2, b + 0.9 * Utils::randomFloat(seed) };
		//			if ((pos - glm::vec3(4, 0.2, 0)).length() > 0.9)
		//			{
		//				Vibrato::Material& sphereMaterial = m_scene.materials.emplace_back();

		//				if (chooseMat < 0.8)
		//				{
		//					// diffuse
		//					sphereMaterial.albedo = glm::vec3(Utils::randomFloat(seed), Utils::randomFloat(seed), Utils::randomFloat(seed));
		//				}
		//				else if (chooseMat < 0.95)
		//				{
		//					// metal
		//					sphereMaterial.albedo = glm::vec3(Utils::randomFloat(seed, 0.5, 1), Utils::randomFloat(seed, 0.5, 1), Utils::randomFloat(seed, 0.5, 1));
		//					sphereMaterial.fuzz = Utils::randomFloat(seed, 0, 0.5);
		//				}
		//				else
		//				{
		//					// glass
		//					sphereMaterial.refractiveIndex = 1.5f;
		//				}
		//				
		//				auto& sphere = std::make_shared<Vibrato::Sphere>();
		//				sphere->position = pos;
		//				sphere->radius = 0.2f;
		//				sphere->materialIndex = (int)(m_scene.materials.size() - 1);
		//				m_scene.objects.push_back(sphere);
		//			}
		//		}
		//	}
		//}
		
		Vibrato::Material& dielectric = m_scene.materials.emplace_back();
		dielectric.refractiveIndex = 1.5f;
		{
			auto& sphere = std::make_shared<Vibrato::Sphere>();
			sphere->position = { 0.0f, 0.5f, 0.0f };
			sphere->radius = 0.5f;
			sphere->materialIndex = (int)(m_scene.materials.size() - 1);
			m_scene.objects.push_back(sphere);
		}

		Vibrato::Material& lambertian = m_scene.materials.emplace_back();
		lambertian.albedo = { 0.8f, 0.5f, 0.0f };
		{
			auto& sphere = std::make_shared<Vibrato::Sphere>();
			sphere->position = { -1.0f, 0.5f, -1.0f };
			sphere->radius = 0.5f;
			sphere->materialIndex = (int)(m_scene.materials.size() - 1);
			m_scene.objects.push_back(sphere);
		}

		Vibrato::Material& metal = m_scene.materials.emplace_back();
		metal.albedo = { 0.7f, 0.6f, 0.5f };
		metal.roughness = 0.0f;
		{
			auto& sphere = std::make_shared<Vibrato::Sphere>();
			sphere->position = { 1.0f, 0.5f, 1.0f };
			sphere->radius = 0.5f;
			sphere->materialIndex = (int)(m_scene.materials.size() - 1);
			m_scene.objects.push_back(sphere);
		}
		*/

		Vibrato::Material& metal = m_scene.materials.emplace_back();
		metal.albedo = { 0.7f, 0.6f, 0.5f };
		metal.roughness = 0.0f;
		/*{
			Vibrato::Vertex v0;
			v0.P = { 0.0f, 0.5f, 0.0f };
			Vibrato::Vertex v1;
			v1.P = { 0.5f, -0.5f, 0.0f };
			Vibrato::Vertex v2;
			v2.P = { -0.5f, -0.5f, 0.0f };

			auto& triangle = std::make_shared<Vibrato::Triangle>(v0, v1, v2);
			triangle->materialIndex = (int)(m_scene.materials.size() - 1);
			m_scene.objects.push_back(triangle);
		}*/

		{
			auto& model = std::make_shared<Vibrato::TriangleMesh>("obj\\dino.obj");
			model->materialIndex = (int)(m_scene.materials.size() - 1);
			m_scene.objects.push_back(model);
		}

	}

	virtual void onUpdate(float ts) override
	{
		if (m_camera.onUpdate(ts))
			m_renderer.resetFrameIndex();
	}

	virtual void onUIRender() override
	{
		ImGui::Begin("Tunings");

		auto& settings = m_renderer.getSettings();

		ImGui::Text("Render Time: %.3fms", m_lastRenderTime); ImGui::SameLine();
		ImGui::Text("%d FPS", (int)(1000 / m_lastRenderTime));

		ImGui::Checkbox("Accumulate Frames", &(settings.accumulate));

		ImGui::InputInt("Rays Per Pixel", &(settings.samplesPerPixel));
		ImGui::InputInt("Ray Bounces", &(settings.bounces));

		if (ImGui::Button("Render"))
		{
			m_renderer.resetFrameIndex();
			render();
		}
		ImGui::SameLine();
		if (ImGui::Button("Screenshot"))
		{
			m_renderer.screenshot();
		}

		ImGui::End();

		ImGui::Begin("Scene");
		
		if (ImGui::TreeNode("Objects"))
		{
			for (size_t i = 0; i < m_scene.objects.size(); ++i)
			{
				ImGui::PushID((int)i);
				std::shared_ptr<Vibrato::Hittable> object = m_scene.objects[i];

				ImGui::Text("\nObject %d", (i + 1));
				ImGui::DragFloat3("Position", glm::value_ptr(object->position), 0.1f);
				// ImGui::DragFloat("Radius", &(object.radius), 0.1f, 0.0f);
				ImGui::DragInt("Material", &(object->materialIndex), 1.0f, 0, (int)(m_scene.materials.size() - 1));

				ImGui::Text("");
				ImGui::Separator();

				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Materials"))
		{
			for (size_t i = 0; i < m_scene.materials.size(); ++i)
			{
				ImGui::PushID((int)i);
				Vibrato::Material& material = m_scene.materials[i];

				ImGui::Text("\nMaterial Index %d", i);

				if (ImGui::Button("Diffuse"))
				{
					material.reset();
				} ImGui::SameLine();

				if (ImGui::Button("Metal"))
				{
					material.reset();
					material.roughness = 0.0f;
				} ImGui::SameLine();

				if (ImGui::Button("Glass"))
				{
					material.reset();
					material.albedo.r = 1.0f;
					material.albedo.g = 1.0f;
					material.albedo.b = 1.0f;
					material.roughness = 0.0f;
					material.refractiveIndex = 1.5f;
				}

				if (ImGui::TreeNode("Advance"))
				{

					ImGui::ColorEdit3("Albedo", glm::value_ptr(material.albedo));
					ImGui::DragFloat("Roughness", &(material.roughness), 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Metallic", &(material.fuzz), 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Refraction Index", &(material.refractiveIndex), 0.01f, 0.0f, FLT_MAX);

					ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.emissionColor));
					ImGui::DragFloat("Emission Power", &(material.emissionPower), 0.01f, 0.0f, FLT_MAX);

					if (ImGui::Button("Reset Material"))
					{
						material.reset();
						material.albedo.r = 1.0f;
						material.albedo.g = 1.0f;
						material.albedo.b = 1.0f;
					}
					ImGui::TreePop();
				}


				ImGui::Text("");
				ImGui::Separator();

				ImGui::PopID();
			}
			ImGui::TreePop();
		}
		
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_viewportWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
		m_viewportHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

		auto image = m_renderer.getFinalImage();
		if (image)
			ImGui::Image(
				image->getDescriptorSet(), 
				{ (float)image->getWidth(), (float)image->getHeight() },
				ImVec2(0, 1),
				ImVec2(1, 0)
			);

		ImGui::End();
		ImGui::PopStyleVar();

		render();
	}

	void render()
	{
		Timer timer;

		m_renderer.onResize(m_viewportWidth, m_viewportHeight);
		m_camera.onResize(m_viewportWidth, m_viewportHeight);
		
		m_renderer.render(m_scene, m_camera);

		m_lastRenderTime = timer.elapsedMillis();
	}

private:
	Vibrato::Camera m_camera;
	Vibrato::Renderer m_renderer;
	Vibrato::Scene m_scene;
	uint32_t m_viewportWidth = 0, m_viewportHeight = 0;

	float m_lastRenderTime = 0.0f;
};

Clef::Application* Clef::createApplication(int argc, char** argv)
{
	Clef::ApplicationSpecification spec;
	spec.name = "Vibrato";

	Clef::Application* app = new Clef::Application(spec);
	app->pushLayer<VibratoLayer>();
	return app;
}
