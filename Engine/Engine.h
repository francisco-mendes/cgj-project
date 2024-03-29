﻿#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Controller.h"
#include "GlfwHandle.h"
#include "../Render/Object.h"
#include "../Render/Scene.h"

namespace engine
{
    class Engine
    {
        GlfwHandle           glfw_;
        callback::WindowSize size_;

        std::filesystem::path snapshot_dir_;
        int                   snap_num_;

    public:
        render::Scene      scene;
        MeshController     mesh_controller;
        TextureController  texture_controller;
        PipelineController pipeline_controller;
        FilterController   filter_controller;
        ObjectController   object_controller;
        FileController     file_controller;

    private:
        Engine(GlfwHandle glfw, render::Scene scene, config::Settings const& settings);
    public:
        static std::unique_ptr<Engine> init(GlfwHandle glfw, render::Scene scene, config::Settings const& settings);

        Engine(Engine const&)            = delete;
        Engine& operator=(Engine const&) = delete;
        Engine(Engine&&)                 = delete;
        Engine& operator=(Engine&&)      = delete;

        [[nodiscard]] Ptr<GLFWwindow> window();

        [[nodiscard]] callback::WindowSize windowSize() const;

        void resize(callback::WindowSize size);
        void resetControllers();
        void setControllers(Ptr<render::Object const> object);

        void snapshot();
        void run();
        void terminate();
    };
}
