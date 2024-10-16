project "OSAlgorithms"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files                          
    {
        "src/**.h",
        "src/**.cpp"
    }
    
    defines
    {
        "YAML_CPP_STATIC_DEFINE"
    }

    includedirs                      
    {
        "%{wks.location}/Nut/vendor/spdlog/include",
        "%{wks.location}/Nut/src",
        "%{wks.location}/Nut/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.ImGuizmo}"
    }

    links
    {
        "Nut"
    }

    filter "system:windows"        
        systemversion "latest"

        filter "configurations:Debug"
            defines "NUT_DEBUG"
            runtime "Debug"
            symbols "on"            

        filter "configurations:Release"
            defines "NUT_Release"
            runtime "Release"
            optimize "on"           

        filter "configurations:Dist"
            defines "NUT_DIST"
            runtime "Release"
            optimize "on"