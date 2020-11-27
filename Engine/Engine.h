#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GlfwHandle.h"
#include "../Render/Scene.h"

namespace engine
{
    class Engine
    {
        GlfwHandle    glfw_;
        render::Scene scene_;

        Engine(GlfwHandle glfw, render::Scene scene);
    public:
        static std::unique_ptr<Engine> init(GlfwHandle glfw, render::Scene scene);

        Engine(Engine const&)            = delete;
        Engine& operator=(Engine const&) = delete;
        Engine(Engine&&)                 = delete;
        Engine& operator=(Engine&&)      = delete;

        [[nodiscard]] Ptr<GLFWwindow> window();
        [[nodiscard]] render::Scene&  scene();

        void run();
        void terminate();
    };
}
