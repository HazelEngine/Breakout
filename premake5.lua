workspace "Breakout"
	startproject "Breakout"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
BreakoutInclude = {}
BreakoutInclude["spdlog"]    = "Breakout/vendor/Hazel/Hazel/vendor/spdlog/include"
BreakoutInclude["GLM"]       = "Breakout/vendor/Hazel/Hazel/vendor/GLM"
BreakoutInclude["ImGui"]     = "Breakout/vendor/Hazel/Hazel/vendor/ImGui"
BreakoutInclude["irrKlang"]  = "Breakout/vendor/irrKlang/include/"

-- Lib directories relative to root folder (solution directory)
BreakoutLib = {}
BreakoutLib["irrKlang"] = "Breakout/vendor/irrKlang/lib/x64/"

-- Include Engine code
include "Breakout/vendor/Hazel/Hazel"
project("Hazel").group = "Engine"

project "Breakout"
	location "Breakout"
	kind "ConsoleApp"
	language "C++"
	staticruntime "Off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}/")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}/")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{BreakoutInclude.spdlog}",
		"%{BreakoutInclude.GLM}",
		"%{BreakoutInclude.ImGui}",
		"%{BreakoutInclude.irrKlang}",
		"Breakout/vendor/Hazel/Hazel/src/"
	}

	libdirs
	{
		"%{BreakoutLib.irrKlang}"
	}

	links
	{
		"Hazel",
		"irrKlang.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"HZ_PLATFORM_WINDOWS"
		}

		postbuildcommands
		{
			(
				"{COPY} "                                                                                 -- copy
				.. "%{wks.location}Breakout/vendor/Hazel/Hazel/bin/" .. outputdir .. "/Hazel/Hazel.dll "  -- from
				.. "%{cfg.buildtarget.directory}"                                                         -- to
			)
		}

	filter { "system:windows", "action:vs*" }
		linkoptions
		{
			"/FORCE:MULTIPLE"
		}

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "HZ_DIST"
		runtime "Release"
		optimize "On"