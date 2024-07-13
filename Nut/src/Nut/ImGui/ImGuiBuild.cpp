#include "nutpch.h"

//------- Set default OpenGL loader to be gl3w -----
//#if !defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)     \
// && !defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)     \
// && !defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)     \
// && !defined(IMGUI_IMPL_OPENGL_LOADER_CUSTOM)
//#define IMGUI_IMPL_OPENGL_LOADER_GL3W
//#endif
#define IMGUI_IMPL_OPENGL_LOADER_GLAD

//------- Files you want to compile ---------------
// Cuz I'm using ImGui v1.73's docking branch asmy imgui repo's docking branch,so i must move "imgui_impl_glfw.cpp" to backends rather than examples ，This is different from the file path in previous versions
// 因为我使用ImGui库v1.73的docking分支作为我个人库的docking，所以这有别于之前版本的文件路径，我必须移动一下
// "examples/imgui_impl_glfw.cpp"    ---->    "backends/imgui_impl_glfw.cpp"
#include <backends/imgui_impl_glfw.cpp>
#include <backends/imgui_impl_opengl3.cpp>