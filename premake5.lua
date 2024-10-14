--[[  关于premake中的token（符记）： https://premake.github.io/docs/Tokens/  ]]

workspace "Nut"                     --工作区
    architecture "x86_64"           --架构
    startproject "Nut-Editor"       --[[启动项目]]

    configurations
    {                               --配置
        "Debug",
        "Release",
        "Dist"
    }

    flags                           --设置编译器选项
	{
		"MultiProcessorCompile"     --多处理器并行编译
	}


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"                 --（方便输出和中间目录编写）

IncludeDir = {}                                                                 --创建一个名为IncludeDir的表
IncludeDir["GLFW"] = "%{wks.location}/Nut/vendor/GLFW/include"                  --将表的"GLFW"键索引到此路径
IncludeDir["Glad"] = "%{wks.location}/Nut/vendor/Glad/include"                  --将表的"Glad"键索引到此路径
IncludeDir["ImGui"] = "%{wks.location}/Nut/vendor/imgui"                        --将表的"ImGui"键索引到此路径
IncludeDir["glm"] = "%{wks.location}/Nut/vendor/glm"                            --将表的"ImGui"键索引到此路径
IncludeDir["stb_image"] = "%{wks.location}/Nut/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Nut/vendor/Entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Nut/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Nut/vendor/ImGuizmo"

---------------------------------------------------------------------------------------
--包含Nut/Nut/vendor/GLFW、Nut/Nut/vendor/Glad、Nut/Nut/vendor/imgui中的premake文件，将其作为依赖项，并合并到这里
group "Dependencies"
    include "Nut/vendor/GLFW"
    include "Nut/vendor/Glad"
    include "Nut/vendor/imgui"
    include "Nut/vendor/yaml-cpp"
group "" 

include "Nut"
include "Sandbox"
include "Nut-Editor"