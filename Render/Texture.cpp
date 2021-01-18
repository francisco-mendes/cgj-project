#include "Texture.h"

#include <iostream>


#include "FreeImage.h"

namespace render
{
    void TextureLoader::Deleter::operator()(FIBITMAP* data) const { FreeImage_Unload(data); }

    TextureLoader TextureLoader::fromFile(std::filesystem::path const& texture_file)
    {
        try
        {
            if (!exists(texture_file))
                throw std::invalid_argument("File not found: " + texture_file.string());

            auto const filename = texture_file.c_str();
            auto const loaded   = FreeImage_LoadU(FreeImage_GetFileTypeU(filename), filename);
            if (loaded == nullptr)
                throw std::invalid_argument("Cannot open texture file: " + texture_file.generic_string());

            auto const image = FreeImage_ConvertTo32Bits(loaded);
            if (image == nullptr)
                throw std::invalid_argument("Cannot use texture file: " + texture_file.generic_string());

            FreeImage_FlipVertical(image);
            auto const width  = FreeImage_GetWidth(image);
            auto const height = FreeImage_GetHeight(image);

            return TextureLoader {width, height, Buffer {image}};
        }
        catch (...)
        {
            std::cerr << "Error loading texture at " << texture_file << '.' << std::endl;
            throw;
        }
    }

    TextureLoader TextureLoader::white(unsigned const len)
    {
        auto const color = RGBQUAD {0xFF, 0xFF, 0xFF, 0xFF};
        auto const data  = FreeImage_AllocateEx(len, len, 32, &color);
        return {len, len, Buffer {data}};
    }

    Texture::Texture(TextureLoader&& texture)
    {
        #if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
        constexpr auto Format = GL_BGRA;
        #else
        constexpr auto Format = GL_RGBA;
        #endif
        auto const [width, height, data] = std::move(texture);
        glGenTextures(1, &tex_id_);
        glBindTexture(GL_TEXTURE_2D, tex_id_);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        auto const pixels = FreeImage_GetBits(data.get());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, Format, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture Texture::fromFile(std::filesystem::path const& texture_file)
    {
        return Texture {TextureLoader::fromFile(texture_file)};
    }

    Texture Texture::white()
    {
        constexpr auto Size = 4;

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
