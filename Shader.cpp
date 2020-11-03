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
    auto const src = program.data();
    auto const len = static_cast<GLint>(program.length());

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