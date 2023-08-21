#include "Clef.h"

#include "Camera.h"
#include "Renderer.h"

using namespace Clef;

class Vibrato : public Clef::Layer
{
public:
	Vibrato()
		: m_camera(45.0f, 0.1f, 100.0f) {}

	virtual void onUpdate(float ts) override
	{
		m_camera.onUpdate(ts);
	}

	virtual void onUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_lastRenderTime);
		
		ImGui::Text("Sphere Color");
		ImGui::SliderFloat("R", &(m_renderer.p_sphereColor.r), 0.0f, 1.0f);
		ImGui::SliderFloat("G", &(m_renderer.p_sphereColor.g), 0.0f, 1.0f);
		ImGui::SliderFloat("B", &(m_renderer.p_sphereColor.b), 0.0f, 1.0f);

		ImGui::Text("Light Direction");
		ImGui::SliderFloat("X", &(m_renderer.p_lightDirection.x), -1.0f, 1.0f);
		ImGui::SliderFloat("Y", &(m_renderer.p_lightDirection.y), -1.0f, 1.0f);
		ImGui::SliderFloat("Z", &(m_renderer.p_lightDirection.z), -1.0f, 1.0f);

		if (ImGui::Button("Render"))
		{
			render();
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
		
		m_renderer.render(m_camera);

		m_lastRenderTime = timer.elapsedMillis();
	}

private:
	Camera m_camera;
	Renderer m_renderer;
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
