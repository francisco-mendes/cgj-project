#pragma once
#include <filesystem>

#include "stb_image.h"
#include "GL/glew.h"

namespace render
{
    struct TextureLoader
    {
        using Deleter = void(*)(void*);

        static TextureLoader fromFile(std::filesystem::path const& mesh_file);
        static TextureLoader white(int size);

        int width, height;

        std::unique_ptr<stbi_uc[], Deleter> data;
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
