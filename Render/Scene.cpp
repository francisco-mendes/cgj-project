#include "Scene.h"

#include <cassert>
#include <filesystem>

#include "../Engine/Engine.h"

namespace render
{
    Scene::Scene(Builder&& builder)
        : meshes_ {std::move(builder.meshes)},
          shaders_ {std::move(builder.shaders)},
          filters_ {std::move(builder.filters)},
          camera_ {*std::move(builder.camera)},
          root_ {std::move(builder.root)}
    { }

    Scene Scene::setup([[maybe_unused]] engine::GlInit gl_init, config::Settings const& settings)
    {
        Builder b;
        config::hooks::setupScene(b, settings);
        assert(b.root.get() != nullptr);
        assert(b.camera.has_value());
        return b;
    }

    void Scene::render(engine::Engine& engine, double const elapsed_sec)
    {
        config::hooks::beforeRender(*this, engine, elapsed_sec);

        camera_.update(engine.windowSize(), elapsed_sec);
        root_->update(elapsed_sec);
        root_->draw(Matrix4::identity(), nullptr);

        config::hooks::afterRender(*this, engine, elapsed_sec);
    }

    void Scene::animate()
    {
        root_->animate();
    }

    Controller const& Scene::camera() const { return camera_; }
    Controller&       Scene::camera() { return camera_; }

    Object const& Scene::root() const { return *root_; }
    Object&       Scene::root() { return *root_; }
}
