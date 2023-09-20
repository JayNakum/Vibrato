#include "Clef.h"

#include "Vibrato/Renderer.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Clef;

class VibratoLayer : public Clef::Layer
{
public:
	VibratoLayer()
		: m_camera(45.0f, 0.1f, 100.0f) 
	{
		Vibrato::Material& blueSphere = m_scene.materials.emplace_back();
		blueSphere.albedo = { 0.1f, 0.5f, 1.0f };
		blueSphere.roughness = 0.0f;

		Vibrato::Material& floorSphere = m_scene.materials.emplace_back();
		floorSphere.roughness = 0.1f;

		Vibrato::Material& orangeSphere = m_scene.materials.emplace_back();
		orangeSphere.roughness = 0.1f;
		orangeSphere.emissionColor = { 0.8f, 0.5f, 0.2f };
		orangeSphere.emissionPower = 20.0f ;

		{
			Vibrato::Sphere sphere;
			sphere.position = { 0.0f, 0.0f, 0.0f };
			sphere.radius = 1.0f;
			sphere.materialIndex = 0;
			m_scene.spheres.push_back(sphere);
		}

		{
			Vibrato::Sphere sphere;
			sphere.position = { 0.0f, -101.0f, 0.0f };
			sphere.radius = 100.0f;
			sphere.materialIndex = 1;
			m_scene.spheres.push_back(sphere);
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
		ImGui::Text("Last render: %.3fms", m_lastRenderTime);

		if (ImGui::Button("Render"))
			render();

		ImGui::Checkbox("Accumulate Frames", &(m_renderer.getSettings().accumulate));
		if (ImGui::Button("Reset"))
			m_renderer.resetFrameIndex();
		

		ImGui::End();

		ImGui::Begin("Scene");
		
		for (size_t i = 0; i < m_scene.spheres.size(); ++i)
		{
			ImGui::PushID(i);
			Vibrato::Sphere& sphere = m_scene.spheres[i];

			ImGui::Text("\nSphere %d", (i + 1));
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.position), 0.1f);
			ImGui::DragFloat("Radius", &(sphere.radius), 0.1f);
			ImGui::DragInt("Material", &(sphere.materialIndex), 1.0f, 0, (int)(m_scene.materials.size() - 1));

			ImGui::Text("");
			ImGui::Separator();

			ImGui::PopID();
		}

		for (size_t i = 0; i < m_scene.materials.size(); ++i)
		{
			ImGui::PushID(i);
			Vibrato::Material& material = m_scene.materials[i];

			ImGui::Text("\nMaterial %d", (i + 1));

			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.albedo));
			ImGui::DragFloat("Roughness", &(material.roughness), 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Metallic", &(material.metallic), 0.01f, 0.0f, 1.0f);

			ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.emissionColor));
			ImGui::DragFloat("Emission Power", &(material.emissionPower), 0.01f, 0.0f, FLT_MAX);

			ImGui::Text("");
			ImGui::Separator();

			ImGui::PopID();
		}
		
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_viewportWidth = ImGui::GetContentRegionAvail().x;
		m_viewportHeight = ImGui::GetContentRegionAvail().y;

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
