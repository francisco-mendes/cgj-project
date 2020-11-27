#pragma once

#include <deque>
#include <vector>

#include "Mesh.h"
#include "Shader.h"
#include "Transform.h"
#include "../Utils.h"

namespace render
{
    template <class Type>
    using Deque = std::pmr::deque<Type>;

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
        Deque<Object> children;

        std::optional<Animation> animation;
    };
}
