#include "Scene.h"

#include <cassert>
#include <filesystem>
#include <optional>


#include "../Config.h"
#include "../Engine/Engine.h"

namespace
{
    template <class DrawCall>
    void withFilter(render::Filter* filter, DrawCall draw)
    {
        if (filter) filter->bind();
        draw();
        if (filter) filter->finish();
    }
}

namespace render
{
    Scene::Scene(Builder&& builder, config::Settings const& settings):
        meshes_ {std::move(builder.meshes)},
        shaders_ {std::move(builder.shaders)},
        textures_ {std::move(builder.textures)},
        filters_ {std::move(builder.filters)},
        root_ {std::move(builder.root)},
        default_shader_ {builder.default_shader},
        shadow_stage_ {Pipeline::Shadows, settings},
        light_position {builder.light_position},
        camera_controller {*std::move(builder.camera)}
    { }


    Scene Scene::setup([[maybe_unused]] engine::GlInit gl_init, config::Settings const& settings)
    {
        Builder builder;
        config::hooks::setupScene(builder, settings);
        assert(builder.camera.has_value());
        assert(builder.default_shader != nullptr);
        return Scene {std::move(builder), settings};
    }

    void Scene::render(engine::Engine& engine, double const elapsed_sec)
    {
        config::hooks::beforeRender(*this, engine, elapsed_sec);

        camera_controller.update(engine.windowSize(), elapsed_sec);
        scene_block_.update(camera_controller.camera.position(), light_position);
        root_->update(elapsed_sec);
        
        shadow_stage_.renderStage(engine, *this, *root_);

        withFilter(
            engine.filter_controller.get(),
            [&]()
            {
                glUseProgram(default_shader_->programId());
                root_->draw(Matrix4::identity(), default_shader_, *this);
            }
        );

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
