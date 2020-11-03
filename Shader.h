#pragma once

#include <fstream>
#include <optional>
#include <string>

#include <GL/glew.h>

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
    static Shader fromFile(Type shader_type, std::string const& filepath);

    Shader(Shader const&) = delete;
    Shader(Shader&&)      = default;

    Shader& operator=(Shader const&) = delete;
    Shader& operator=(Shader&&)      = default;

    ~Shader();

    [[nodiscard]] Type   type() const;
    [[nodiscard]] GLuint id() const;
};

template <class Bindings>
class ShaderProgram
{
    GLuint   program_id_;
    Bindings bindings_;
public:
    ShaderProgram() = default;

    ShaderProgram(std::initializer_list<Shader> shaders)
        : program_id_ {glCreateProgram()},
          bindings_ {}
    {
        for (auto const& shader : shaders)
        {
            glAttachShader(program_id_, shader.id());
        }

        bindings_.bindInputs(program_id_);

        glLinkProgram(program_id_);

        bindings_.bindUniforms(program_id_);

        for (auto const& shader : shaders)
        {
            glDetachShader(program_id_, shader.id());
        }
    }

    void cleanup()
    {
        if (program_id_ != 0)
        {
            glDeleteProgram(program_id_);
        }
    }

    [[nodiscard]] GLuint          programId() const { return program_id_; }
    [[nodiscard]] Bindings const& bindings() const { return bindings_; }
};
