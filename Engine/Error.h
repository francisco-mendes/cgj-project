#pragma once
#include "Engine.h"

namespace engine
{
    void setupErrorCallback(Engine* engine);

    void glfwErrorCallback(int error, const char* description);
}
