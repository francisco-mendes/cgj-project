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
        int width_;
        int height_;
        std::filesystem::path snapshot_dir_;
        int snap_num_;

        Engine(GlfwHandle glfw, render::Scene scene, int width, int height);
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
