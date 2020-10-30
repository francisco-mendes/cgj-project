#include "Shader.h"


#include <iostream>
#include <sstream>

std::string readFile(std::string const& filepath)
{
    if (std::ifstream in(filepath); in)
    {
        std::string program;

        in.seekg(0, std::ios::end);
        program.resize(in.tellg());

        in.seekg(0, std::ios::beg);
        in.read(program.data(), program.size());
        return program;
    }
    throw errno;
}

Shader::Shader(Type const shader_type, std::string_view const program)
    : shader_type_ {shader_type},
      shader_id_ {glCreateShader(static_cast<GLenum>(shader_type))}
{
    auto const  src = program.data();
    GLint const len = program.length();

    glShaderSource(shader_id_, 1, &src, &len);
    glCompileShader(shader_id_);
}

Shader Shader::fromFile(Type const shader_type, std::string const& filepath)
{
    auto const program = readFile(filepath);
    return Shader {shader_type, program};
}

Shader::~Shader()
{
    glDeleteShader(shader_id_);
}

Shader::Type Shader::type() const { return shader_type_; }
GLuint       Shader::id() const { return shader_id_; }

ShaderProgram::ShaderProgram(std::initializer_list<Shader> const shaders)
    : program_id_ {glCreateProgram()}
{
    for (auto const& shader : shaders)
        glAttachShader(program_id_, shader.id());

    glBindAttribLocation(program_id_, Position, "in_Position");
    glBindAttribLocation(program_id_, Colors, "in_Color");

    glLinkProgram(program_id_);

    model_id_  = glGetUniformLocation(program_id_, "ModelMatrix");
    camera_id_ = glGetUniformBlockIndex(program_id_, "CameraMatrices");
    glUniformBlockBinding(program_id_, camera_id_, Camera);

    for (auto const& shader : shaders)
        glDetachShader(program_id_, shader.id());
}

void ShaderProgram::cleanup()
{
    if (program_id_ != 0)
    {
        glDeleteProgram(program_id_);
    }
}

GLuint ShaderProgram::programId() const { return program_id_; }
GLint  ShaderProgram::modelId() const { return model_id_; }
GLint  ShaderProgram::cameraId() const { return camera_id_; }
