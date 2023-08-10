#include "Clef.h"

class ExampleLayer : public Clef::Layer
{
public:
	virtual void onUIRender() override
	{
		ImGui::Begin("Window");
		ImGui::Text("Welcome to Clef");
		ImGui::End();

		ImGui::ShowDemoWindow();
	}
};

Clef::Application* Clef::createApplication(int argc, char** argv)
{
	Clef::ApplicationSpecification spec;
	spec.name = "Clef Example";

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
		}
	);

	return app;
}
