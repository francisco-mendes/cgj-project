#pragma once

#include "../Math/Vector.h"
#include "GL/glew.h"

namespace render
{
    class SceneBlock
    {
        GLuint scene_block_id_;
    public:
        explicit SceneBlock(GLuint bind_id);

        SceneBlock(SceneBlock const& other)            = delete;
        SceneBlock& operator=(SceneBlock const& other) = delete;

        SceneBlock(SceneBlock&& other) noexcept;

        SceneBlock& operator=(SceneBlock&& other) noexcept;

        ~SceneBlock();

        void update(Vector3 camera_position, Vector3 light_position);
    };
}
