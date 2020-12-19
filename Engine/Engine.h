#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GlfwHandle.h"
#include "../Render/Scene.h"

namespace engine
{
    class Engine
    {
        GlfwHandle           glfw_;
        render::Scene        scene_;
        callback::WindowSize size_;

        std::filesystem::path snapshot_dir_;
        int                   snap_num_;
        int framebuffer_;
        int textureColorbuffer_;
        int quadVAO_;

        Engine(GlfwHandle glfw, render::Scene scene, config::Settings const& settings);
    public:
        static std::unique_ptr<Engine> init(GlfwHandle glfw, render::Scene scene, config::Settings const& settings);

        Engine(Engine const&)            = delete;
        Engine& operator=(Engine const&) = delete;
        Engine(Engine&&)                 = delete;
        Engine& operator=(Engine&&)      = delete;

        [[nodiscard]] Ptr<GLFWwindow>      window();
        [[nodiscard]] render::Scene&       scene();
        [[nodiscard]] callback::WindowSize windowSize() const;

        void resize(callback::WindowSize size);

        void snapshot();
        void run();
        void terminate();
    };
}
