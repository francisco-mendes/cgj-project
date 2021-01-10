#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "../Utils.h"

namespace render
{
    class Scene;

    struct Object
    {
        [[nodiscard]] explicit Object(
            OptPtr<Object>         parent,
            OptPtr<Mesh const>     mesh,
            Vector4                color,
            OptPtr<Pipeline const> shaders = nullptr,
            OptPtr<Texture const>  texture = nullptr
        );

        [[nodiscard]] explicit Object(
            OptPtr<Object>         parent  = nullptr,
            OptPtr<Pipeline const> shaders = nullptr,
            OptPtr<Texture const>  texture = nullptr
        );

        void animate();
        void update(double elapsed_sec);
        void draw(Matrix4 const& parent_transform, OptPtr<Pipeline const> parent_shaders, Scene const& scene) const;

        Object& emplaceChild(
            OptPtr<Mesh const>     mesh,
            Vector4                color,
            OptPtr<Pipeline const> shaders = nullptr,
            OptPtr<Texture const>  texture = nullptr
        );

        Object& emplaceChild(
            OptPtr<Pipeline const> shaders = nullptr,
            OptPtr<Texture const>  texture = nullptr
        );

        bool removeChild(Ptr<Object> child);

        OptPtr<Object> parent;

        OptPtr<Mesh const>     mesh;
        OptPtr<Pipeline const> shaders;
        OptPtr<Texture const>  texture;

        Vector4   color;
        Transform transform;

        std::optional<Animation> animation;
        std::list<Object>        children;
    };
}
