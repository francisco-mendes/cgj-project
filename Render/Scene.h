#pragma once

#include <deque>

#include "Camera.h"
#include "Mesh.h"
#include "Object.h"
#include "Shader.h"
#include "../Engine/GlInit.h"

namespace render
{
    class Scene
    {
    public:
        struct Builder
        {
            Deque<Mesh>          meshes;
            Deque<ShaderProgram> shaders;

            std::optional<Controller> camera;
            std::unique_ptr<Object>   root;
        };

    private:
        Deque<Mesh>          meshes_;
        Deque<ShaderProgram> shaders_;

        Controller              camera_;
        std::unique_ptr<Object> root_;

        Scene(Builder&& builder);
    public:
        static Scene setup(engine::GlInit gl_init);

        void render(engine::Engine&, double elapsed_sec);
        void animate();

        [[nodiscard]] Controller const& camera() const;
        [[nodiscard]] Controller&       camera();

        [[nodiscard]] Object const& root() const;
        [[nodiscard]] Object&       root();
    };
}

namespace config::hooks
{
    void setupScene(render::Scene::Builder& builder);
    void beforeRender(render::Scene& scene, engine::Engine& engine, double elapsed_sec);
    void afterRender(render::Scene& scene, engine::Engine& engine, double elapsed_sec);
}
