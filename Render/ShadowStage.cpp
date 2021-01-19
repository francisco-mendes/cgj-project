#include "ShadowStage.h"

#include <array>


#include "../Math/Matrix.h"
#include "../Engine/Engine.h"

namespace
{
    constexpr auto Width = 2 << 8, Height = 2 << 8;

    std::array<Matrix4, 6> createLightMatrices(Vector3 const light_position)
    {
        auto const light_proj = Matrix4::perspective(
            90.f,
            Width / Height,
            render::ShadowStage::NearPlane,
            render::ShadowStage::FarPlane
        );

        return std::array<Matrix4, 6> {
            light_proj * Matrix4::view(light_position, light_position + Vector3 {1.f, 0.f, 0.f}, {0.f, -1.f, 0.f}),
            light_proj * Matrix4::view(light_position, light_position + Vector3 {-1.f, 0.f, 0.f}, {0.f, -1.f, 0.f}),
            light_proj * Matrix4::view(light_position, light_position + Vector3 {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}),
            light_proj * Matrix4::view(light_position, light_position + Vector3 {0.f, -1.f, 0.f}, {0.f, 0.f, -1.f}),
            light_proj * Matrix4::view(light_position, light_position + Vector3 {0.f, 0.f, 1.f}, {0.f, -1.f, 0.f}),
            light_proj * Matrix4::view(light_position, light_position + Vector3 {0.f, 0.f, -1.f}, {0.f, -1.f, 0.f})
        };
    }
}

namespace render
{
    ShadowStage::ShadowStage(GLuint const bind_id, config::Settings const& settings)
        : shadow_shader_ {
            false,
            Shader::fromFile(Shader::Vertex, settings.paths.shaders / "Shadows/shadowMap_vert.glsl"),
            Shader::fromFile(Shader::Geometry, settings.paths.shaders / "Shadows/shadowMap_geo.glsl"),
            Shader::fromFile(Shader::Fragment, settings.paths.shaders / "Shadows/shadowMap_frag.glsl")
        }
    {
        glGenBuffers(1, &shadow_map_buffer_id_);
        glBindBuffer(GL_UNIFORM_BUFFER, shadow_map_buffer_id_);
        {
            glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4) * 6 + sizeof(float), nullptr, GL_STREAM_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, bind_id, shadow_map_buffer_id_);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glGenTextures(1, &depth_map_id_);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depth_map_id_);
        {
            for (auto target = GL_TEXTURE_CUBE_MAP_POSITIVE_X; target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; ++target)
                glTexImage2D(target, 0, GL_DEPTH_COMPONENT, Width, Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }
        glActiveTexture(GL_TEXTURE0);

        glGenFramebuffers(1, &depth_fb_id_);
        glBindFramebuffer(GL_FRAMEBUFFER, depth_fb_id_);
        {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map_id_, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ShadowStage::ShadowStage(ShadowStage&& other) noexcept
        : depth_fb_id_ {std::exchange(other.depth_fb_id_, 0)},
          depth_map_id_ {std::exchange(other.depth_map_id_, 0)},
          shadow_map_buffer_id_ {std::exchange(other.shadow_map_buffer_id_, 0)},
          shadow_shader_ {std::move(other.shadow_shader_)}
    {}

    ShadowStage& ShadowStage::operator=(ShadowStage&& other) noexcept
    {
        if (this != &other)
        {
            depth_fb_id_          = std::exchange(other.depth_fb_id_, 0);
            depth_map_id_         = std::exchange(other.depth_map_id_, 0);
            shadow_map_buffer_id_ = std::exchange(other.shadow_map_buffer_id_, 0);
            shadow_shader_        = std::move(other.shadow_shader_);
        }
        return *this;
    }

    ShadowStage::~ShadowStage()
    {
        if (depth_fb_id_ != 0)
        {
            glDeleteFramebuffers(1, &depth_fb_id_);
            glDeleteTextures(1, &depth_map_id_);
            glDeleteBuffers(1, &shadow_map_buffer_id_);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }

    void ShadowStage::renderStage(engine::Engine const& engine, Scene const& scene, Object const& root) const
    {
        glViewport(0, 0, Width, Height);
        glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, 1.0);
        glBindFramebuffer(GL_FRAMEBUFFER, depth_fb_id_);
        glClear(GL_DEPTH_BUFFER_BIT);
        glBindBuffer(GL_UNIFORM_BUFFER, shadow_map_buffer_id_);
        {
            auto const light_matrix = createLightMatrices(scene.light_position);
            auto const far_plane    = FarPlane;

            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4) * 6, light_matrix.data());
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix4) * 6, sizeof(float), &far_plane);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glUseProgram(shadow_shader_.programId());
        glCullFace(GL_FRONT);
        root.draw(Matrix4::identity(), &shadow_shader_, scene, true);
        glCullFace(GL_BACK);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        auto const [width, height] = engine.windowSize();
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}
