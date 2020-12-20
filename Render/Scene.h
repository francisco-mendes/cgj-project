#pragma once

#include <deque>

#include "Camera.h"
#include "Mesh.h"
#include "Object.h"
#include "Filter.h"
#include "Shader.h"
#include "../Engine/GlInit.h"

namespace render
{
    class Scene
    {
    public:
        struct Builder
        {
            std::deque<Mesh>     meshes;
            std::deque<Pipeline> shaders;

            std::vector<Filter> filters;

            std::optional<Controller> camera;
            std::unique_ptr<Object>   root;
        };

    private:
        std::deque<Mesh>     meshes_;
        std::deque<Pipeline> shaders_;
        std::vector<Filter>  filters_;

        Controller              camera_;
        std::unique_ptr<Object> root_;
    public:
        OptPtr<Filter> active_filter = nullptr;

    private:
        Scene(Builder&& builder);
    public:
        static Scene setup(engine::GlInit gl_init, config::Settings const& settings);

        void render(engine::Engine&, double elapsed_sec);
        void animate();
        void resizeFilters(callback::WindowSize size);

        [[nodiscard]] Controller const& controller() const;
        [[nodiscard]] Controller&       controller();

        [[nodiscard]] Object const& root() const;
        [[nodiscard]] Object&       root();
    };
}

namespace config::hooks
{
    void setupScene(render::Scene::Builder& builder, Settings const& settings);
    void beforeRender(render::Scene& scene, engine::Engine& engine, double elapsed_sec);
    void afterRender(render::Scene& scene, engine::Engine& engine, double elapsed_sec);
}
