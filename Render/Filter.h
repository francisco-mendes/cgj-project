#pragma once
#include "Shader.h"
#include "../Config.h"
#include "GL/glew.h"

namespace render
{
    class Filter
    {
        GLuint fb_id_;
        GLuint tex_id_;
        GLuint quad_id_, quad_buffer_;

        Ptr<Pipeline const> pipeline_;

    public:
        Filter(Ptr<Pipeline const> pipeline, config::Window const& window);

        void bind();
        void finish();
    };
}
