#pragma once

#ifdef CLEF_PLATFORM_WINDOWS

extern Clef::Application* Clef::createApplication(int argc, char** argv);

namespace Clef
{
	int Main(int argc, char** argv)
	{
		Clef::Application* app = createApplication(argc, argv);
		app->run();
		delete app;

		return 0;
	}
}

#ifdef CLEF_RELEASE

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return Clef::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Clef::Main(argc, argv);
}

#endif // CLEF_RELEASE

#endif // CLEF_PLATFORM_WINDOWS
