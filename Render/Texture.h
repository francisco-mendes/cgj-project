#pragma once
#include <filesystem>

#include "FreeImage.h"
#include "GL/glew.h"

namespace render
{
    struct TextureLoader
    {
        struct Deleter
        {
            void operator()(FIBITMAP* data) const;
        };

        using Buffer = std::unique_ptr<FIBITMAP, Deleter>;

        static TextureLoader fromFile(std::filesystem::path const& texture_file);
        static TextureLoader white(unsigned len);

        unsigned width, height;
        Buffer   data;
    };

    class Texture
    {
        GLuint tex_id_;

    public:
        Texture(TextureLoader&& texture);
        static Texture fromFile(std::filesystem::path const& texture_file);
        static Texture white();

        Texture(Texture const& other)            = delete;
        Texture& operator=(Texture const& other) = delete;

        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        ~Texture();

        [[nodiscard]] GLuint texId() const;
    };
}
