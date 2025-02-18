------------------------- Nut Dependencies --------------------------

VULKAN_SDK = os.getenv("VULKAN_SDK")                -- s.getenv("VULKAN_SDK") 用来获取环境变量 VULKAN_SDK 的值，通常是存储 Vulkan SDK（软件开发工具包）的安装路径

IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/Nut/vendor/stb_image"
IncludeDir["glm"] = "%{wks.location}/Nut/vendor/glm"
IncludeDir["GLFW"] = "%{wks.location}/Nut/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Nut/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Nut/vendor/ImGui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Nut/vendor/ImGuizmo"
IncludeDir["entt"] = "%{wks.location}/Nut/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Nut/vendor/yaml-cpp/include"
IncludeDir["Box2D"] = "%{wks.location}/Nut/vendor/Box2D/include"
IncludeDir["shaderc"] = "%{wks.location}/Nut/vendor/VulkanSDK/Include/shaderc"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Nut/vendor/VulkanSDK/Include/spirv_cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{VULKAN_SDK}/Lib"

Library = {}

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"    --文件不存在

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"