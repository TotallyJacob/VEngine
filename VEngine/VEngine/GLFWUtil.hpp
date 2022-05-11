#pragma once
// THIS THING CAUSE A #error gl.h included... !!!!! IT NEEDED PRAGMA
#include "GLFW/glfw3.h"

namespace vengine
{

inline static void setWindowHints(const float version)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

} // namespace vengine
