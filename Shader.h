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

class ShaderProgram
{
public:
    constexpr static GLuint Position = 0;
    constexpr static GLuint Colors   = 1;

    constexpr static GLuint Camera = 0;

private:
    GLuint program_id_;
    GLint  model_id_, camera_id_;

public:
    ShaderProgram() = default;
    ShaderProgram(std::initializer_list<Shader> shaders);

    void cleanup();

    [[nodiscard]] GLuint programId() const;
    [[nodiscard]] GLint  modelId() const;
    [[nodiscard]] GLint  cameraId() const;
};

