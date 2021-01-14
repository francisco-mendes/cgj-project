#include "Scene.h"

#include <cassert>
#include <filesystem>
#include <optional>


#include "../Engine/Engine.h"

namespace render
{
    Scene::Scene(Builder&& builder)
        : meshes_ {std::move(builder.meshes)},
          shaders_ {std::move(builder.shaders)},
          textures_ {std::move(builder.textures)},
          filters_ {std::move(builder.filters)},
          root_ {std::move(builder.root)},
          default_shader_ {builder.default_shader},
          light_position {builder.light_position},
          camera_controller {*std::move(builder.camera)}
    { }

    Scene Scene::setup([[maybe_unused]] engine::GlInit gl_init, config::Settings const& settings)
    {
        Builder b;
        config::hooks::setupScene(b, settings);
        assert(b.camera.has_value());
        assert(b.default_shader != nullptr);
        return b;
    }

    void Scene::render(engine::Engine& engine, double const elapsed_sec)
    {
        config::hooks::beforeRender(*this, engine, elapsed_sec);

        camera_controller.update(engine.windowSize(), elapsed_sec);
        scene_block_.update(camera_controller.camera.position(), light_position);
        root_->update(elapsed_sec);
        glUseProgram(default_shader_->programId());
        root_->draw(Matrix4::identity(), default_shader_, *this);

        config::hooks::afterRender(*this, engine, elapsed_sec);
    }

    void Scene::animate()
    {
        root_->animate();
    }

    void Scene::resizeFilters(callback::WindowSize const size)
    {
        for (auto& filter : filters_) { filter.resize(size); }
    }

    Texture const& Scene::defaultTexture() const { return default_texture_; }
}
