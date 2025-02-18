--[[  关于premake中的token（符记）： https://premake.github.io/docs/Tokens/  ]]

include "./vendor/premake/premake_customization/solution_items.lua"
include "premakeDependencies.lua"

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

---------------------------------------------------------------------------------------
--包含Nut/Nut/vendor/GLFW、Nut/Nut/vendor/Glad、Nut/Nut/vendor/imgui中的premake文件，将其作为依赖项，并合并到这里
group "Dependencies"
    include "vendor/premake"
    include "Nut/vendor/GLFW"
    include "Nut/vendor/Glad"
    include "Nut/vendor/imgui"
    include "Nut/vendor/yaml-cpp"
    include "Nut/vendor/Box2D"
group "" 

include "Nut"
include "Sandbox"
include "Nut-Editor"
include "OSAlgorithms"