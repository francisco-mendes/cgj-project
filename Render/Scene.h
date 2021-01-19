#pragma once

#include <deque>

#include "Camera.h"
#include "Mesh.h"
#include "Object.h"
#include "Filter.h"
#include "SceneBlock.h"
#include "Shader.h"
#include "ShadowStage.h"
#include "../Config.h"
#include "../Engine/GlInit.h"

namespace render
{
    class Scene
    {
    public:
        friend class engine::Engine;

        struct Builder
        {
            std::deque<Mesh>     meshes;
            std::deque<Pipeline> shaders;
            std::deque<Filter>   filters;
            std::deque<Texture>  textures;

            Vector3 light_position;

            std::optional<CameraController> camera;
            std::unique_ptr<Object>         root = std::make_unique<Object>(nullptr);

            Ptr<Pipeline const> default_shader;
        };

    private:
        std::deque<Mesh>     meshes_;
        std::deque<Pipeline> shaders_;
        std::deque<Texture>  textures_;
        std::deque<Filter>   filters_;

        std::unique_ptr<Object> root_;
        Ptr<Pipeline const>     default_shader_;
        Texture                 default_texture_ = Texture::white();
        SceneBlock              scene_block_     = SceneBlock(Pipeline::Scene);
        ShadowStage             shadow_stage_;
    public:
        Vector3          light_position;
        CameraController camera_controller;
    private:
        Scene(Builder&& builder, config::Settings const& settings);
    public:
        static Scene setup(engine::GlInit gl_init, config::Settings const& settings);

        void render(engine::Engine&, double elapsed_sec);
        void animate();
        void resizeFilters(callback::WindowSize size);

        [[nodiscard]] Texture const& defaultTexture() const;
    };
}

namespace config::hooks
{
    void setupScene(render::Scene::Builder& builder, Settings const& settings);
    void beforeRender(render::Scene& scene, engine::Engine& engine, double elapsed_sec);
    void afterRender(render::Scene& scene, engine::Engine& engine, double elapsed_sec);
}
