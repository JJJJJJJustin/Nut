--[[  关于premake中的token（符记）： https://premake.github.io/docs/Tokens/  ]]

workspace "Nut"                     --工作区
    architecture "x64"              --架构
    startproject "Sandbox"          --[[启动项目]]

    configurations
    {                               --配置
        "Debug",
        "Release",
        "Dist"
    }

--[[起别名]]
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"                 --（方便输出和中间目录编写）
IncludeDir = {}                                                                 --创建一个名为IncludeDir的表
IncludeDir["GLFW"] = "Nut/vendor/GLFW/include"                                  --将表的"GLFW"键索引到此路径
IncludeDir["Glad"] = "Nut/vendor/Glad/include"                                  --将表的"Glad"键索引到此路径
IncludeDir["ImGui"] = "Nut/vendor/imgui"                                        --将表的"ImGui"键索引到此路径
IncludeDir["glm"] = "Nut/vendor/glm"                                            --将表的"ImGui"键索引到此路径

---------------------------------------------------------------------------------------
--包含Nut/Nut/vendor/GLFW、Nut/Nut/vendor/Glad、Nut/Nut/vendor/imgui中的premake文件，将其作为依赖项，并合并到这里
group "Dependencies"
    include "Nut/vendor/GLFW"
    include "Nut/vendor/Glad"
    include "Nut/vendor/imgui"

group ""                                                                        --包含所有未分组的文件

--[[
    XXXX
]]
---------------------------------------------------------------------------------------

--[[项目Nut]]
project "Nut"                       --项目
    location "Nut"                  --项目文件的输出目录（填写解决方案Nut之下的路径 "Nut/Nut"）
    kind "StaticLib"                --类型（动态库）
    language "C++"                  --语言
    cppdialect "C++17"              --C++标准（编译时）
    staticruntime "on"              --是否将运行时库静态链接运行时库（dll属性的文件需要关闭）

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")                           --输出目录(.. XX ..中 ".."是字符串连接符)
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")                          --中间目录

    pchheader "nutpch.h"            --预编译头文件
    pchsource "Nut/src/nutpch.cpp"                                              --(实际上编译器不需要 但是VS需要这个.cpp文件)
    

    files                           --将源代码文件包含到项目中
    {
       "%{prj.name}/src/**.h",
       "%{prj.name}/src/**.cpp",
       "%{prj.name}/vendor/glm/glm/**.hpp",
       "%{prj.name}/vendor/glm/glm/**.inl"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    includedirs                     --库文件（包含库目录）
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",                                                   --将IncludeDir表中GLFW键索引的值（地址）作为一个库文件路径
        "%{IncludeDir.Glad}",                                                   --将IncludeDir表中Glad键索引的值（地址）作为一个库文件路径
        "%{IncludeDir.ImGui}",                                                  --将IncludeDir表中ImGui键索引的值（地址）作为一个库文件路径
        "%{IncludeDir.glm}"                                                     --将IncludeDir表中glm键索引的值（地址）作为一个库文件路径
    }

    links                           --为Nut项目(.dll)链接文件
    {
        "GLFW",                                                                 --链接上方项目GLFW
        "Glad",                                                                 --链接上方项目Glad
        "ImGui",                                                                --链接上方项目ImGui
        "opengl32.lib"
    }

    filter "system:windows"         --过滤器(如果系统是windows)   >>> !!!  filter, not filters  !!!
        systemversion "latest"      --windows SDK 版本

        defines
        {                           --宏的声明
            "NUT_PLATFORM_WINDOWS",
            "NUT_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

        --  //////////////////////////////////////////////////////////////////////
        --  ////  NOW WE USE NUT AS A STATIC LIB, SO DON'T NEED THIS COMMAND  ////
        --  //////////////////////////////////////////////////////////////////////
        --[[postbuildcommands           --构建项目完成后执行的指令
        {
            ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
            --将当前项目的构建目标文件复制到 "../bin/" 下的一个名为 "Debug/Sandbox" 或 "Release/Sandbox" 的子目录中
        }   ]]

    filter "configurations:Debug"   -->>> !!!  configurations, not configuration  !!!
        defines "NUT_DEBUG"
        runtime "Debug"
        symbols "on"                --编译器是否生成带有调试符号的可执行文件

    filter "configurations:Release"
        defines "NUT_Release"
        runtime "Release"
        optimize "on"               --是否开启代码优化

    filter "configurations:Dist"
        defines "NUT_DIST"
        runtime "Release"
        optimize "on"

---------------------------------------------------------------------------------------------------------------

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files                          
    {
       "%{prj.name}/src/**.h",
       "%{prj.name}/src/**.cpp"
    }

    includedirs                      
    {
        "Nut/vendor/spdlog/include",
        "Nut/src",
        "Nut/vendor",
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
            "NUT_PLATFORM_WINDOWS"
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