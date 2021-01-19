#pragma once
#include "Shader.h"
#include "../Config.h"
#include "GL/glew.h"

namespace engine
{
    class Engine;
}

namespace render
{
    struct Object;
    class Scene;

    class ShadowStage
    {
    public:
        constexpr static auto NearPlane = 0.5f, FarPlane = 100.f;
    private:
        GLuint   depth_fb_id_, depth_map_id_, shadow_map_buffer_id_;
        Pipeline shadow_shader_;
    public:
        ShadowStage(GLuint bind_id, config::Settings const& settings);

        ShadowStage(ShadowStage const& other)            = delete;
        ShadowStage& operator=(ShadowStage const& other) = delete;

        ShadowStage(ShadowStage&& other) noexcept;
        ShadowStage& operator=(ShadowStage&& other) noexcept;

        ~ShadowStage();

        void renderStage(engine::Engine const& engine, Scene const& scene, Object const& root) const;
    };
}
