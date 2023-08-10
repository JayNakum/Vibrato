#include "Clef/Application.h"
#include "Clef/EntryPoint.h"

#include "Clef/Image.h"
#include "Clef/Random.h"
#include "Clef/Timer.h"

using namespace Clef;

class ExampleLayer : public Clef::Layer
{
public:
	virtual void onUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_lastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_viewportWidth = ImGui::GetContentRegionAvail().x;
		m_viewportHeight = ImGui::GetContentRegionAvail().y;

		if (m_image)
			ImGui::Image(m_image->getDescriptorSet(), { (float)m_image->getWidth(), (float)m_image->getHeight() });

		ImGui::End();
		ImGui::PopStyleVar();

		// Render();
	}

	void Render()
	{
		Timer timer;

		if (!m_image || m_viewportWidth != m_image->getWidth() || m_viewportHeight != m_image->getHeight())
		{
			m_image = std::make_shared<Image>(m_viewportWidth, m_viewportHeight, ImageFormat::RGBA);
			delete[] m_imageData;
			m_imageData = new uint32_t[m_viewportWidth * m_viewportHeight];
		}

		for (uint32_t i = 0; i < m_viewportWidth * m_viewportHeight; i++)
		{
			m_imageData[i] = 0xffb39373;
		}

		m_image->setData(m_imageData);

		m_lastRenderTime = timer.elapsedMillis();
	}
private:
	std::shared_ptr<Image> m_image;
	uint32_t* m_imageData = nullptr;
	uint32_t m_viewportWidth = 0, m_viewportHeight = 0;

	float m_lastRenderTime = 0.0f;
};

Clef::Application* Clef::createApplication(int argc, char** argv)
{
	Clef::ApplicationSpecification spec;
	spec.name = "Vibrato";

	Clef::Application* app = new Clef::Application(spec);
	app->pushLayer<ExampleLayer>();
	app->setMenubarCallback([app]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					app->close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}