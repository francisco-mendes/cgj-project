#include "Mesh.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace render
{
    namespace
    {
        constexpr auto Vertices = 0;
        constexpr auto Textures = 1;
        constexpr auto Normals  = 2;

        void parseLine(std::istringstream& line, MeshLoader& mesh)
        {
            std::string s;
            line >> s;
            if (s == "v") mesh.parseVec(line);
            else if (s == "vt") mesh.parseTex(line);
            else if (s == "vn") mesh.parseNorm(line);
            else if (s == "f") mesh.parseFace(line);
        }

        MeshLoader parseMeshFile(std::ifstream& in)
        {
            MeshLoader  mesh;
            std::string line;
            while (std::getline(in, line))
            {
                std::istringstream line_in(line);
                parseLine(line_in, mesh);
            }
            return mesh;
        }

        GLuint createVao(MeshLoader const& mesh)
        {
            auto const vert_count = mesh.size();

            GLuint vao_id, vbo_vt, vbo_tx, vbo_n;

            std::vector<Vector3> vs;
            std::vector<Vector2> ts;
            std::vector<Vector3> ns;

            vs.reserve(vert_count);
            for (auto const vert_id : mesh.vert_ids)
                vs.push_back(mesh.vertices[vert_id - 1]);

            if (mesh.has_textures)
            {
                ts.reserve(vert_count);
                for (auto const tex_id : mesh.tex_ids)
                    ts.push_back(mesh.tex_coords[tex_id - 1]);
            }

            if (mesh.has_normals)
            {
                ns.reserve(vert_count);
                for (auto const norm_id : mesh.norm_ids)
                    ns.push_back(mesh.normals[norm_id - 1]);
            }

            glGenVertexArrays(1, &vao_id);
            glBindVertexArray(vao_id);
            {
                glGenBuffers(1, &vbo_vt);
                glBindBuffer(GL_ARRAY_BUFFER, vbo_vt);
                glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(Vector3), vs.data(), GL_STATIC_DRAW);
                glEnableVertexAttribArray(Vertices);
                glVertexAttribPointer(Vertices, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);

                if (mesh.has_textures)
                {
                    glGenBuffers(1, &vbo_tx);
                    glBindBuffer(GL_ARRAY_BUFFER, vbo_tx);
                    glBufferData(GL_ARRAY_BUFFER, ts.size() * sizeof(Vector2), ts.data(), GL_STATIC_DRAW);
                    glEnableVertexAttribArray(Textures);
                    glVertexAttribPointer(Textures, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), nullptr);
                }
                if (mesh.has_normals)
                {
                    glGenBuffers(1, &vbo_n);
                    glBindBuffer(GL_ARRAY_BUFFER, vbo_n);
                    glBufferData(GL_ARRAY_BUFFER, ns.size() * sizeof(Vector3), ns.data(), GL_STATIC_DRAW);
                    glEnableVertexAttribArray(Normals);
                    glVertexAttribPointer(Normals, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
                }
            }
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDeleteBuffers(1, &vbo_vt);
            glDeleteBuffers(1, &vbo_tx);
            glDeleteBuffers(1, &vbo_n);
            return vao_id;
        }
    }

    MeshLoader MeshLoader::fromFile(std::filesystem::path const& mesh_file)
    {
        if (!exists(mesh_file))
            throw std::invalid_argument("File not found: " + mesh_file.string());

        try
        {
            std::ifstream in;
            in.exceptions(std::ios_base::badbit);
            in.open(mesh_file);

            return parseMeshFile(in);
        }
        catch (...)
        {
            std::cerr << "Error loading mesh at " << mesh_file << '.' << std::endl;
            throw;
        }
    }

    GLsizei MeshLoader::size() const { return vert_ids.size(); }

    void MeshLoader::parseVec(std::istream& in)
    {
        Vector3 v;
        in >> v;
        vertices.push_back(v);
    }

    void MeshLoader::parseTex(std::istream& in)
    {
        Vector2 v;
        in >> v;
        tex_coords.push_back(v);
    }

    void MeshLoader::parseNorm(std::istream& in)
    {
        Vector3 v;
        in >> v;
        normals.push_back(v);
    }

    void MeshLoader::parseFace(std::istream& in)
    {
        if (normals.empty()) has_normals = true;
        if (tex_coords.empty()) has_textures = true;

        if (std::string token; !has_normals && !has_textures)
        {
            for (int i = 0; i < 3; i++)
            {
                in >> token;
                vert_ids.push_back(std::stoi(token));
            }
        }
        else
        {
            for (int i = 0; i < 3; i++)
            {
                std::getline(in, token, '/');
                if (!token.empty()) vert_ids.push_back(std::stoi(token));

                std::getline(in, token, '/');
                if (!token.empty()) tex_ids.push_back(std::stoi(token));

                std::getline(in, token, ' ');
                if (!token.empty()) norm_ids.push_back(std::stoi(token));
            }
        }
    }

    Mesh Mesh::fromFile(std::filesystem::path const& mesh_file)
    {
        return MeshLoader::fromFile(mesh_file);
    }

    Mesh::Mesh(MeshLoader const& loaded)
        : vao_id_ {createVao(loaded)},
          vertex_count_ {loaded.size()}
    {}

    Mesh::Mesh(Mesh&& other) noexcept
        : vao_id_ {std::exchange(other.vao_id_, 0)},
          vertex_count_ {other.vertex_count_}
    {}

    Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
        if (this != &other)
        {
            vao_id_       = std::exchange(other.vao_id_, 0);
            vertex_count_ = other.vertex_count_;
        }
        return *this;
    }

    Mesh::~Mesh()
    {
        if (vao_id_ != 0)
        {
            glDeleteVertexArrays(1, &vao_id_);
        }
    }

    GLuint  Mesh::vaoId() const { return vao_id_; }
    GLsizei Mesh::vertexCount() const { return vertex_count_; }
}
