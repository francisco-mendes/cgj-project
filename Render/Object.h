#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Transform.h"
#include "../Utils.h"

namespace render
{
    struct Object
    {
        [[nodiscard]] explicit Object(
            OptPtr<Mesh const>          mesh,
            Vector4                     color,
            OptPtr<ShaderProgram const> shaders = nullptr
        );

        [[nodiscard]] explicit Object(OptPtr<ShaderProgram const> shaders = nullptr);

        void animate();
        void update(double elapsed_sec);
        void draw(Matrix4 const& parent_transform, OptPtr<ShaderProgram const> parent_shaders) const;

        OptPtr<Mesh const>          mesh;
        OptPtr<ShaderProgram const> shaders;

        Vector4       color;
        Transform     transform;
        std::list<Object> children;

        std::optional<Animation> animation;
    };
}
