#pragma once

#include <filesystem>
#include <vector>

#include <GL/glew.h>

#include "../Math/Vector.h"

namespace render
{
    struct MeshLoader
    {
        std::vector<Vector4> vertices;
        std::vector<Vector2> tex_coords;
        std::vector<Vector4> normals;

        std::vector<GLsizei> vert_ids;
        std::vector<GLsizei> tex_ids;
        std::vector<GLsizei> norm_ids;

        bool has_normals, has_textures;

        static MeshLoader fromFile(std::filesystem::path const& mesh_file);

        GLsizei size() const;

        void parseVec(std::istream& in);
        void parseTex(std::istream& in);
        void parseNorm(std::istream& in);
        void parseFace(std::istream& in);
    };

    class Mesh
    {
        GLuint  vao_id_;
        GLsizei vertex_count_;

    public:
        Mesh(MeshLoader const& loaded);
        static Mesh fromFile(std::filesystem::path const& mesh_file);

        Mesh(Mesh const& other)            = delete;
        Mesh& operator=(Mesh const& other) = delete;

        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        ~Mesh();

        [[nodiscard]] GLuint  vaoId() const;
        [[nodiscard]] GLsizei vertexCount() const;
    };
}
