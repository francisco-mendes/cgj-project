#pragma once

#include <filesystem>
#include <optional>

#include <GL/glew.h>

#include "Shader.h"

namespace render
{
    class Shader
    {
    public:
        enum Type : GLenum
        {
            Vertex = GL_VERTEX_SHADER,
            TesselationControl = GL_TESS_CONTROL_SHADER,
            TesselationEvaluation = GL_TESS_EVALUATION_SHADER,
            Geometry = GL_GEOMETRY_SHADER,
            Fragment = GL_FRAGMENT_SHADER,
            Compute = GL_COMPUTE_SHADER
        };

    private:
        Type   shader_type_;
        GLuint shader_id_;

    public:
        Shader(Type shader_type, std::string_view);
        static Shader fromFile(Type shader_type, std::filesystem::path const& shader_file);

        Shader(Shader const&)            = delete;
        Shader& operator=(Shader const&) = delete;

        Shader(Shader&& other) noexcept;
        Shader& operator=(Shader&&) noexcept;

        ~Shader();

        [[nodiscard]] Type   type() const;
        [[nodiscard]] GLuint id() const;
    };

    class Pipeline
    {
    public:
        constexpr static GLuint Position = 0;
        constexpr static GLuint Texture  = 1;
        constexpr static GLuint Normal   = 2;

        constexpr static GLuint Camera  = 0;
        constexpr static GLuint Scene   = 1;
        constexpr static GLuint Shadows = 2;
    private:
        GLuint program_id_, model_id_,    color_id_, texture_id_;
        GLuint ambient_id_, specular_id_, shininess_id_;
        GLuint shadow_map_id_;
        
        bool   is_filter_;
    public:
        Pipeline(Pipeline const&)            = delete;
        Pipeline& operator=(Pipeline const&) = delete;

        Pipeline(Pipeline&& other) noexcept;
        Pipeline& operator=(Pipeline&&) noexcept;

        Pipeline(std::initializer_list<Shader> shaders, bool is_filter = true);

        ~Pipeline();

        template <class... Shaders>
        explicit Pipeline(bool is_filter, Shaders ...shaders) : Pipeline({std::move(shaders) ...}, is_filter)
        {
            static_assert((std::is_same_v<Shader, Shaders> && ...));
        }

        [[nodiscard]] bool   isFilter() const;
        [[nodiscard]] GLuint programId() const;

        [[nodiscard]] GLuint modelId() const;
        [[nodiscard]] GLuint colorId() const;
        [[nodiscard]] GLuint textureId() const;

        [[nodiscard]] GLuint ambientId() const;
        [[nodiscard]] GLuint specularId() const;
        [[nodiscard]] GLuint shininessId() const;

        [[nodiscard]] GLuint shadowMapId() const;
    };
}

bool operator==(render::Pipeline const& lhs, render::Pipeline const& rhs);
bool operator!=(render::Pipeline const& lhs, render::Pipeline const& rhs);
