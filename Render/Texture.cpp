#include "Texture.h"

#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace
{
    void deleter(void* buffer)
    {
        delete[] static_cast<stbi_uc*>(buffer);
    }
}

namespace render
{
    TextureLoader TextureLoader::fromFile(std::filesystem::path const& texture_file)
    {
        if (!exists(texture_file))
            throw std::invalid_argument("File not found: " + texture_file.string());

        if (FILE* file; _wfopen_s(&file, texture_file.c_str(), L"rb") == 0)
        {
            int width, height, channels;

            stbi_set_flip_vertically_on_load(true);
            auto data = std::unique_ptr<stbi_uc[], Deleter>(
                stbi_load_from_file(file, &width, &height, &channels, 4),
                stbi_image_free
            );
            fclose(file);

            if (data.get() == nullptr)
                throw std::invalid_argument("Invalid texture file: " + texture_file.generic_string());
            assert(channels == 4);

            return TextureLoader {width, height, std::move(data)};
        }
        throw std::invalid_argument("Cannot open texture file: " + texture_file.generic_string());
    }

    TextureLoader TextureLoader::white(const int size)
    {
        auto const len = size * size * sizeof(float);

        auto buf = new stbi_uc[len];
        std::fill_n(buf, len, 0xFF);

        auto data = std::unique_ptr<stbi_uc[], Deleter>(buf, deleter);
        return {size, size, std::move(data)};
    }

    Texture::Texture(TextureLoader&& texture)
    {
        auto const [width, height, data] = std::move(texture);
        glGenTextures(1, &tex_id_);
        glBindTexture(GL_TEXTURE_2D, tex_id_);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture Texture::fromFile(std::filesystem::path const& texture_file)
    {
        return Texture {TextureLoader::fromFile(texture_file)};
    }

    Texture Texture::white()
    {
        constexpr auto Size = 16;

        return TextureLoader::white(Size);
    }

    Texture::Texture(Texture&& other) noexcept: tex_id_ {std::exchange(other.tex_id_, 0)} {}

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            std::exchange(other.tex_id_, 0);
        }
        return *this;
    }

    Texture::~Texture()
    {
        if (tex_id_ != 0)
        {
            glDeleteTextures(1, &tex_id_);
        }
    }

    GLuint Texture::texId() const { return tex_id_; }
}
