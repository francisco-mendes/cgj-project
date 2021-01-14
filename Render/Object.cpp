#include "Object.h"

#include <cassert>

#include "Scene.h"

namespace render
{
    Object::Object(
        OptPtr<Object>               parent,
        OptPtr<Mesh const> const     mesh,
        Vector4 const                color,
        OptPtr<Pipeline const> const shaders,
        OptPtr<Texture const> const  texture
    ) : parent {parent},
        mesh {mesh},
        shaders {shaders},
        texture {texture},
        color {color}
    {}

    Object::Object(
        OptPtr<Object> const         parent,
        OptPtr<Pipeline const> const shaders,
        OptPtr<Texture const> const  texture
    ) : parent {parent},
        mesh {nullptr},
        shaders {shaders},
        texture {texture},
        color {Vector4::filled(1.0f)}
    {}

    void Object::animate()
    {
        if (animation)
        {
            if (animation->active()) { animation->reverse(); }
            else { animation->activate(); }
        }
        for (auto& child : children) { child.animate(); }
    }

    void Object::update(double const elapsed_sec)
    {
        if (animation && animation->active())
            transform = animation->advance(elapsed_sec);

        for (auto& child : children) { child.update(elapsed_sec); }
    }

    void Object::draw(
        Matrix4 const&               parent_transform,
        OptPtr<Pipeline const> const parent_shaders,
        Scene const&                 scene
    ) const
    {
        auto const shaders = this->shaders != nullptr ? this->shaders : parent_shaders;
        assert(shaders != nullptr);

        if (shaders != parent_shaders) { glUseProgram(shaders->programId()); }
        {
            auto const model_matrix = parent_transform * transform.toMatrix();

            if (auto const [r, g, b, alpha] = color; mesh != nullptr && alpha != 0)
            {
                glUniform4f(shaders->colorId(), r, g, b, alpha);

                if (auto const [r, g, b] = ambient_color; shaders->ambientId() != -1)
                {
                    glUniform3f(shaders->ambientId(), r, g, b);
                }

                if (auto const [r, g, b] = specular_color; shaders->specularId() != -1)
                {
                    glUniform3f(shaders->specularId(), r, g, b);
                    glUniform1f(shaders->shininessId(), shininess);
                }
                auto const texture_bind = texture != nullptr ? texture->texId() : scene.defaultTexture().texId();
                glUniform1i(shaders->textureId(), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture_bind);

                glUniformMatrix4fv(shaders->modelId(), 1, GL_TRUE, model_matrix.inner);
                glBindVertexArray(mesh->vaoId());
                glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount());
            }
            for (auto const& child : children) { child.draw(model_matrix, shaders, scene); }
        }
        if (shaders != parent_shaders && parent_shaders != nullptr) { glUseProgram(parent_shaders->programId()); }
    }

    Object& Object::emplaceChild(
        OptPtr<Mesh const>     mesh,
        Vector4                color,
        OptPtr<Pipeline const> shaders,
        OptPtr<Texture const>  texture
    )
    {
        return children.emplace_back(this, mesh, color, shaders, texture);
    }

    Object& Object::emplaceChild(OptPtr<Pipeline const> shaders, OptPtr<Texture const> texture)
    {
        return children.emplace_back(this, shaders, texture);
    }

    bool Object::removeChild(Ptr<Object> const child)
    {
        for (auto iter = children.begin(), end = children.end(); iter == end; ++iter)
        {
            if (&*iter == child)
            {
                children.erase(iter);
                return true;
            }
        }
        return false;
    }
}
