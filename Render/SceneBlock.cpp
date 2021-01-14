#include "SceneBlock.h"

#include "../Math/Vector.h"

namespace render
{
    SceneBlock::SceneBlock(GLuint const bind_id)
    {
        glGenBuffers(1, &scene_block_id_);
        glBindBuffer(GL_UNIFORM_BUFFER, scene_block_id_);
        {
            glBufferData(GL_UNIFORM_BUFFER, sizeof(Vector3) * 2, nullptr, GL_STREAM_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, bind_id, scene_block_id_);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    SceneBlock::SceneBlock(SceneBlock&& other) noexcept
        : scene_block_id_ {std::exchange(other.scene_block_id_, 0)}
    {}

    SceneBlock& SceneBlock::operator=(SceneBlock&& other) noexcept
    {
        if (this != &other)
        {
            scene_block_id_ = std::exchange(other.scene_block_id_, 0);
        }
        return *this;
    }

    SceneBlock::~SceneBlock()
    {
        if (scene_block_id_ != 0)
        {
            glDeleteBuffers(1, &scene_block_id_);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }

    void SceneBlock::update(Vector3 camera_position, Vector3 light_position)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, scene_block_id_);
        {
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Vector3), &camera_position);
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Vector3), sizeof(Vector3), &light_position);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}
