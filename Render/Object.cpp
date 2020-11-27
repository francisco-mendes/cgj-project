#include "Object.h"

#include <cassert>


namespace render
{
    Object::Object(OptPtr<Mesh const> const mesh, Vector4 const color, OptPtr<ShaderProgram const> const shaders)
        : mesh {mesh},
          shaders {shaders},
          color {color}
    {}

    Object::Object(OptPtr<ShaderProgram const> const shaders)
        : mesh {nullptr},
          shaders {shaders},
          color {}
    {}

    void Object::animate()
    {
        if (animation)
        {
            if (animation->active()) { animation->reverse(); }
            else { animation->activate(); }
        }
    }

    void Object::update(double const elapsed_sec)
    {
        if (animation && animation->active())
            transform = animation->advance(elapsed_sec);

        for (auto& child : children) { child->update(elapsed_sec); }
    }

    void Object::draw(Matrix4 const& parent_transform, OptPtr<ShaderProgram const> const parent_shaders) const
    {
        auto const shaders = this->shaders != nullptr ? this->shaders : parent_shaders;
        assert(shaders != nullptr);

        if (shaders != parent_shaders) { glUseProgram(shaders->programId()); }
        {
            auto const model_matrix = parent_transform * transform.toMatrix();

            if (auto const [r, g, b, alpha] = color; mesh != nullptr && alpha != 0)
            {
                glBindVertexArray(mesh->vaoId());
                glUniform4f(shaders->colorId(), r, g, b, alpha);
                glUniformMatrix4fv(shaders->modelId(), 1, GL_TRUE, model_matrix.inner);
                glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount());
            }
            for (auto const& child : children) { child->draw(model_matrix, shaders); }
        }
        if (shaders != parent_shaders && parent_shaders != nullptr) { glUseProgram(parent_shaders->programId()); }
    }
}
