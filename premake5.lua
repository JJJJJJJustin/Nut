--[[  关于premake中的token（符记）： https://premake.github.io/docs/Tokens/  ]]

workspace "Nut"                     --工作区
    architecture "x64"              --架构

    configurations
    {                               --配置
        "Debug",
        "Release",
        "Dist"
    }

--[[起别名]]
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"                 --（方便输出和中间目录编写）

--[[项目Nut]]
project "Nut"                       --项目
    location "Nut"                  --项目文件的输出目录（填写解决方案Nut之下的路径 "Nut/Nut"）
    kind "SharedLib"                --类型（动态库）
    language "C++"                  --语言

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")                           --输出目录(.. XX ..中 ".."是字符串连接符)
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")                          --中间目录

    pchheader "nutpch.h"            --预编译头文件
    pchsource "Nut/src/nutpch.cpp"                                              --(实际上编译器不需要 但是VS需要这个.cpp文件)
    

    files                           --将源代码文件添加到特定的项目中
    {
       "%{prj.name}/src/**.h",
       "%{prj.name}/src/**.cpp"
    }

    includedirs                     --库文件
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",                                                 --将IncludeDir表中GLFW键索引的值作为一个库文件
    }

    filter "system:windows"         --过滤器(如果系统是windows)   >>> !!!  filter, not filters  !!!
        cppdialect "C++17"          --C++标准（编译时）
        staticruntime "On"          --是否静态链接运行时库（dll属性的文件需要打开）
        systemversion "latest"      --windows SDK 版本

        defines
        {                           --宏的声明
            "NUT_PLATFORM_WINDOWS",
            "NUT_BUILD_DLL"
        }

        postbuildcommands           --构建项目完成后执行的指令
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
            --将当前项目的构建目标文件复制到 "../bin/" 下的一个名为 "Debug/Sandbox" 或 "Release/Sandbox" 的子目录中
        }

        filter "configurations:Debug"                       -->>> !!!  configurations, not configuration   !!!
            defines "NUT_DEBUG"
            symbols "On"            --编译器是否生成带有调试符号的可执行文件

        filter "configurations:Release"
            defines "NUT_Release"
            optimize "On"           --是否开启优化

        filter "configurations:Dist"
            defines "NUT_DIST"
            optimize "On"

-- -------------------------------------------------------------------------------------------------------------

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"

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
        "Nut/src"
    }

    links                           --链接
    {
        "Nut"
    }

    filter "system:windows"        
        cppdialect "C++17"          
        staticruntime "On"          
        systemversion "latest"

        defines
        {                          
            "NUT_PLATFORM_WINDOWS"
        }

        filter "configurations:Debug"
            defines "NUT_DEBUG"
            symbols "On"            

        filter "configurations:Release"
            defines "NUT_Release"
            optimize "On"           

        filter "configurations:Dist"
            defines "NUT_DIST"
            optimize "On"