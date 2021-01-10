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

        void checkLinkage(GLuint const program_id)
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


    Pipeline::Pipeline(Pipeline&& other) noexcept
        : program_id_ {std::exchange(other.program_id_, 0)},
          model_id_ {std::exchange(other.model_id_, 0)},
          color_id_ {std::exchange(other.color_id_, 0)},
          eye_id_ {std::exchange(other.eye_id_, 0)},
          light_id_ {std::exchange(other.light_id_, 0)},
          texture_id_ {std::exchange(other.texture_id_, 0)},
          is_filter_ {std::exchange(other.is_filter_, false)}
    {}

    Pipeline& Pipeline::operator=(Pipeline&& other) noexcept
    {
        if (this != &other)
        {
            program_id_ = std::exchange(other.program_id_, 0);
            model_id_   = std::exchange(other.model_id_, 0);
            color_id_   = std::exchange(other.color_id_, 0);
            eye_id_     = std::exchange(other.eye_id_, 0);
            light_id_   = std::exchange(other.light_id_, 0);
            texture_id_ = std::exchange(other.texture_id_, 0);
            is_filter_  = std::exchange(other.is_filter_, false);
        }
        return *this;
    }

    Pipeline::~Pipeline()
    {
        if (program_id_ != 0)
        {
            glDeleteProgram(program_id_);
        }
    }

    Pipeline::Pipeline(std::initializer_list<Shader> shaders, bool const is_filter)
        : program_id_ {glCreateProgram()},
          is_filter_ {is_filter}
    {
        for (auto const& shader : shaders)
        {
            glAttachShader(program_id_, shader.id());
        }

        glBindAttribLocation(program_id_, Position, "in_Position");
        glBindAttribLocation(program_id_, Texture, "in_Texcoord");
        glBindAttribLocation(program_id_, Normal, "in_Normal");

        glLinkProgram(program_id_);
        checkLinkage(program_id_);

        model_id_   = glGetUniformLocation(program_id_, "ModelMatrix");
        color_id_   = glGetUniformLocation(program_id_, "Color");
        eye_id_     = glGetUniformLocation(program_id_, "Eye");
        light_id_   = glGetUniformLocation(program_id_, "Light");
        texture_id_ = glGetUniformLocation(program_id_, "Texture");

        auto const camera_id = glGetUniformBlockIndex(program_id_, "CameraMatrices");
        glUniformBlockBinding(program_id_, camera_id, Camera);

        for (auto const& shader : shaders)
        {
            glDetachShader(program_id_, shader.id());
        }
    }

    bool   Pipeline::isFilter() const { return is_filter_; }
    GLuint Pipeline::programId() const { return program_id_; }
    GLuint Pipeline::modelId() const { return model_id_; }
    GLuint Pipeline::colorId() const { return color_id_; }
    GLuint Pipeline::eyeId() const { return eye_id_; }
    GLuint Pipeline::lightId() const { return light_id_; }
    GLuint Pipeline::textureId() const { return texture_id_; }
}

bool operator==(render::Pipeline const& lhs, render::Pipeline const& rhs)
{
    return lhs.programId() == rhs.programId();
}

bool operator!=(render::Pipeline const& lhs, render::Pipeline const& rhs) { return !(lhs == rhs); }
