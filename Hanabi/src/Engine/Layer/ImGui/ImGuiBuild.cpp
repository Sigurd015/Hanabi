#include "hnbpch.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <backends/imgui_impl_opengl3.cpp>
#include <backends/imgui_impl_glfw.cpp>

#if defined(HNB_PLATFORM_WINDOWS)
#include <backends/imgui_impl_dx11.cpp>
#endif