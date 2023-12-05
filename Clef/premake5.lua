project "Clef"
  kind "StaticLib"
  language "C++"
  cppdialect "C++17"
  targetdir "bin/%{cfg.buildcfg}"
  staticruntime "off"

  files { "src/**.h", "src/**.cpp" }

  includedirs
  {
    "src",

    "../vendor/imgui",
    "../vendor/glfw/include",
    "../vendor/stb_image",
    
    "%{IncludeDir.VulkanSDK}",
  }

  links
  {
    "ImGui",
    "GLFW",

    "%{Library.Vulkan}",
  }
  
  targetdir ("bin/" .. outputdir .. "/%{prj.name}")
  objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

  filter "system:windows"
    systemversion "latest"
    defines { "CLEF_PLATFORM_WINDOWS" }

  filter "configurations:Debug"
    defines { "CLEF_DEBUG" }
    runtime "Debug"
    symbols "On"

  filter "configurations:Release"
    defines { "CLEF_RELEASE" }
    runtime "Release"
    optimize "On"
    symbols "Off"
