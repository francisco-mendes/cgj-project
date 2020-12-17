#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <FreeImage.h>
#include <string>

#include "GlfwHandle.h"
#include "../Render/Scene.h"

namespace engine
{
    class Engine
    {
        GlfwHandle    glfw_;
        render::Scene scene_;
        int width;
        int height;
        const char* snapsPath;
        int snapNum;

        Engine(GlfwHandle glfw, render::Scene scene, int width_, int height_);
    public:
        static std::unique_ptr<Engine> init(GlfwHandle glfw, render::Scene scene, int width_, int height_);

        Engine(Engine const&)            = delete;
        Engine& operator=(Engine const&) = delete;
        Engine(Engine&&)                 = delete;
        Engine& operator=(Engine&&)      = delete;

        [[nodiscard]] Ptr<GLFWwindow> window();
        [[nodiscard]] render::Scene&  scene();

        void snapshot();
        void run();
        void terminate();
    };
}
