#include "Clef.h"

#include "Camera.h"
#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Clef;

class Vibrato : public Clef::Layer
{
public:
	Vibrato()
		: m_camera(45.0f, 0.1f, 100.0f) 
	{
		{
			Sphere sphere;
			sphere.position = { 0.0f, 0.0f, 0.0f };
			sphere.radius = 0.5f;
			sphere.albedo = { 0.1f, 0.5f, 1.0f };
			m_scene.spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.position = { 0.0f, -9.5f, 0.0f };
			sphere.radius = 9.0f;
			sphere.albedo = { 1.0f, 0.0f, 1.0f };
			m_scene.spheres.push_back(sphere);
		}
	}

	virtual void onUpdate(float ts) override
	{
		m_camera.onUpdate(ts);
	}

	virtual void onUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_lastRenderTime);

		if (ImGui::Button("Render"))
		{
			render();
		}
		ImGui::End();

		ImGui::Begin("Scene");
		
		for (size_t i = 0; i < m_scene.spheres.size(); ++i)
		{
			ImGui::PushID(i);
			Sphere& sphere = m_scene.spheres[i];

			ImGui::Text("\nSphere %d", (i + 1));
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.position), 0.1f);
			ImGui::DragFloat("Radius", &(sphere.radius), 0.1f);
			ImGui::ColorEdit3("Albedo", glm::value_ptr(sphere.albedo));
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
	Camera m_camera;
	Renderer m_renderer;
	Scene m_scene;
	uint32_t m_viewportWidth = 0, m_viewportHeight = 0;

	float m_lastRenderTime = 0.0f;
};

Clef::Application* Clef::createApplication(int argc, char** argv)
{
	Clef::ApplicationSpecification spec;
	spec.name = "Vibrato";

	Clef::Application* app = new Clef::Application(spec);
	app->pushLayer<Vibrato>();
	return app;
}
