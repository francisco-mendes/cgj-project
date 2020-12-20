#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Transform.h"
#include "../Utils.h"

namespace render
{
    class Scene;

    struct Object
    {
        [[nodiscard]] explicit Object(
            OptPtr<Mesh const>     mesh,
            Vector4                color,
            OptPtr<Pipeline const> shaders = nullptr
        );

        [[nodiscard]] explicit Object(OptPtr<Pipeline const> shaders = nullptr);

        void animate();
        void update(double elapsed_sec);
        void draw(Matrix4 const& parent_transform, OptPtr<Pipeline const> parent_shaders, Scene const& scene) const;

        OptPtr<Mesh const>     mesh;
        OptPtr<Pipeline const> shaders;

        Vector4           color;
        Transform         transform;
        std::list<Object> children;

        std::optional<Animation> animation;
    };
}
