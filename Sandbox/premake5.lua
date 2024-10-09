project "Sandbox"
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

    includedirs                      
    {
        "%{wks.location}/Nut/vendor/spdlog/include",
        "%{wks.location}/Nut/src",
        "%{wks.location}/Nut/vendor",
        "%{IncludeDir.glm}"
    }

    links                           --链接
    {
        "Nut"
    }

    filter "system:windows"        
        systemversion "latest"

        defines
        {
            
        }

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
