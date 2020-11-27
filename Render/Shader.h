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

    class ShaderProgram
    {
    public:
        constexpr static GLuint Position = 0;
        constexpr static GLuint Colors   = 1;

        constexpr static GLuint Camera = 0;
    private:
        GLuint program_id_, model_id_, color_id_;
    public:
        ShaderProgram(ShaderProgram const&)            = delete;
        ShaderProgram& operator=(ShaderProgram const&) = delete;

        ShaderProgram(ShaderProgram&& other) noexcept;
        ShaderProgram& operator=(ShaderProgram&&) noexcept;

        ShaderProgram(std::initializer_list<Shader> shaders);

        ~ShaderProgram();

        template <class... Shaders>
        explicit ShaderProgram(Shaders ...shaders) : ShaderProgram({std::move(shaders) ...})
        {
            static_assert((std::is_same_v<Shader, Shaders> && ...));
        }

        [[nodiscard]] GLuint programId() const;
        [[nodiscard]] GLuint modelId() const;
        [[nodiscard]] GLuint colorId() const;
    };
}

bool operator==(render::ShaderProgram const& lhs, render::ShaderProgram const& rhs);
bool operator!=(render::ShaderProgram const& lhs, render::ShaderProgram const& rhs);