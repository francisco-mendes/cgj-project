#pragma once

#include "Shader.h"
#include "../Callback.h"
#include "../Config.h"
#include "GL/glew.h"

namespace render
{
    class Filter
    {
        GLuint fb_id_, tex_id_, rb_id_, quad_id_;

        Ptr<Pipeline const> pipeline_;

    public:
        Filter(config::Window const& window, Ptr<Pipeline const> pipeline);

        Filter(Filter const& other)            = delete;
        Filter& operator=(Filter const& other) = delete;

        Filter(Filter&& other) noexcept;
        Filter& operator=(Filter&& other) noexcept;

        ~Filter();

        void bind();
        void finish();
        void resize(callback::WindowSize size);
    };
}
