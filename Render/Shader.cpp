#include "Shader.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace render
{
    namespace
    {
        void checkCompilation(GLuint const shader_id)
        {
            GLint compiled;
            glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
            if (compiled == GL_FALSE)
            {
                GLint length;
                glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

                std::string log = "Failed to compile shader: ";
                log.resize(length + log.size());
                glGetShaderInfoLog(shader_id, length, &length, log.data());
                throw std::runtime_error(log);
            }
        }

        void checkLinkage(const GLuint program_id)
        {
            GLint linked;
            glGetProgramiv(program_id, GL_LINK_STATUS, &linked);
            if (linked == GL_FALSE)
            {
                GLint length;
                glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);

                std::string log = "Failed to link program: ";
                log.resize(length + log.size());
                glGetProgramInfoLog(program_id, length, &length, log.data());
                throw std::runtime_error(log);
            }
        }

        std::string readShaderFile(std::filesystem::path const& shader_path)
        {
            if (!exists(shader_path))
                throw std::invalid_argument("File not found: " + shader_path.string());

            auto const size = file_size(shader_path);

            std::ifstream in;
            in.exceptions(std::ios_base::badbit);
            in.open(shader_path);

            std::string program;
            program.resize(size);

            in.read(program.data(), size);
            return program;
        }
    }

    Shader::Shader(Type const shader_type, std::string_view const program)
        : shader_type_ {shader_type},
          shader_id_ {glCreateShader(static_cast<GLenum>(shader_type))}
    {
        auto const src = program.data();
        auto const len = static_cast<GLint>(program.length());

        glShaderSource(shader_id_, 1, &src, &len);
        glCompileShader(shader_id_);
        checkCompilation(shader_id_);
    }

    Shader Shader::fromFile(Type const shader_type, std::filesystem::path const& shader_file)
    {
        try
        {
            auto const program = readShaderFile(shader_file);
            return Shader {shader_type, program};
        }
        catch (...)
        {
            std::cerr << "Error loading shader at " << absolute(shader_file) << '.' << std::endl;
            throw;
        }
    }

    Shader::Shader(Shader&& other) noexcept
        : shader_type_ {other.shader_type_},
          shader_id_ {std::exchange(other.shader_id_, 0)} {}

    Shader& Shader::operator=(Shader&& other) noexcept
    {
        if (this != &other)
        {
            shader_type_ = other.shader_type_;
            shader_id_   = std::exchange(other.shader_id_, 0);
        }
        return *this;
    }

    Shader::~Shader()
    {
        if (shader_id_ != 0)
        {
            glDeleteShader(shader_id_);
        }
    }

    Shader::Type Shader::type() const { return shader_type_; }
    GLuint       Shader::id() const { return shader_id_; }


    ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
        : program_id_ {std::exchange(other.program_id_, 0)},
          model_id_ {std::exchange(other.model_id_, 0)},
          color_id_ {std::exchange(other.color_id_, 0)} {}

    ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
    {
        if (this != &other)
        {
            program_id_ = std::exchange(other.program_id_, 0);
            model_id_   = std::exchange(other.model_id_, 0);
            color_id_   = std::exchange(other.color_id_, 0);
        }
        return *this;
    }

    ShaderProgram::~ShaderProgram()
    {
        if (program_id_ != 0)
        {
            glDeleteProgram(program_id_);
        }
    }

    ShaderProgram::ShaderProgram(std::initializer_list<Shader> shaders)
        : program_id_ {glCreateProgram()}
    {
        for (auto const& shader : shaders)
        {
            glAttachShader(program_id_, shader.id());
        }

        glBindAttribLocation(program_id_, Position, "in_Position");
        glBindAttribLocation(program_id_, Colors, "in_Color");

        glLinkProgram(program_id_);
        checkLinkage(program_id_);

        model_id_ = glGetUniformLocation(program_id_, "ModelMatrix");
        color_id_ = glGetUniformLocation(program_id_, "Color");

        auto const camera_id = glGetUniformBlockIndex(program_id_, "CameraMatrices");
        glUniformBlockBinding(program_id_, camera_id, Camera);

        for (auto const& shader : shaders)
        {
            glDetachShader(program_id_, shader.id());
        }
    }

    GLuint ShaderProgram::programId() const { return program_id_; }
    GLuint ShaderProgram::modelId() const { return model_id_; }
    GLuint ShaderProgram::colorId() const { return color_id_; }
}

bool operator==(render::ShaderProgram const& lhs, render::ShaderProgram const& rhs)
{
    return lhs.programId() == rhs.programId();
}

bool operator!=(render::ShaderProgram const& lhs, render::ShaderProgram const& rhs) { return !(lhs == rhs); }
