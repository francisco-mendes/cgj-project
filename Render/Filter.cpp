#include "Filter.h"

#include <iostream>
#include <array>

#include <GL/glew.h>


#include "../Callback.h"
#include "../Config.h"

namespace render
{
    Filter::Filter(config::Window const& window, Ptr<Pipeline const> const pipeline)
        : pipeline_ {pipeline}
    {
        std::array const quad_vertices {
            // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            // texCoords
            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        };

        auto const [width, height] = window.size;
        GLuint     quad_buffer;
        // screen quad VAO
        glGenVertexArrays(1, &quad_id_);
        glBindVertexArray(quad_id_);
        {
            glGenBuffers(1, &quad_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, quad_buffer);
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
                glEnableVertexAttribArray(Pipeline::Position);
                glVertexAttribPointer(Pipeline::Position, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
                glEnableVertexAttribArray(Pipeline::Texture);
                glVertexAttribPointer(
                    Pipeline::Texture,
                    2,
                    GL_FLOAT,
                    GL_FALSE,
                    4 * sizeof(float),
                    reinterpret_cast<void*>(2 * sizeof(float))
                );
            }
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &quad_buffer);

        glGenFramebuffers(1, &fb_id_);
        glBindFramebuffer(GL_FRAMEBUFFER, fb_id_);
        {
            // create a color attachment texture
            glGenTextures(1, &tex_id_);
            glBindTexture(GL_TEXTURE_2D, tex_id_);
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_id_, 0);
            }
            glGenRenderbuffers(1, &rb_id_);
            glBindRenderbuffer(GL_RENDERBUFFER, rb_id_);
            {
                // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
                // use a single renderbuffer object for both a depth AND stencil buffer.
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rb_id_);
                // now actually attach it
                // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
            }
            #if _DEBUG
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            #endif
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    Filter::Filter(Filter&& other) noexcept
        : fb_id_ {std::exchange(other.fb_id_, 0)},
          tex_id_ {std::exchange(other.tex_id_, 0)},
          rb_id_ {std::exchange(other.rb_id_, 0)},
          quad_id_ {std::exchange(other.quad_id_, 0)},
          pipeline_ {std::exchange(other.pipeline_, nullptr)}
    {}

    Filter& Filter::operator=(Filter&& other) noexcept
    {
        if (this != &other)
        {
            fb_id_    = std::exchange(other.fb_id_, 0);
            tex_id_   = std::exchange(other.tex_id_, 0);
            rb_id_    = std::exchange(other.rb_id_, 0);
            quad_id_  = std::exchange(other.quad_id_, 0);
            pipeline_ = std::exchange(other.pipeline_, nullptr);
        }
        return *this;
    }

    Filter::~Filter()
    {
        if (pipeline_ != nullptr)
        {
            glDeleteVertexArrays(1, &quad_id_);
            glDeleteFramebuffers(1, &fb_id_);
            glDeleteRenderbuffers(1, &rb_id_);
            glDeleteTextures(1, &tex_id_);
        }
    }

    void Filter::bind()
    {
        // render
        // ------
        // bind to framebuffer and draw scene as we normally would to color texture 
        glBindFramebuffer(GL_FRAMEBUFFER, fb_id_);
        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

        // make sure we clear the framebuffer's content
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Filter::finish()
    {
        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(pipeline_->programId());
        //usar os shaders aqui
        glBindVertexArray(quad_id_);
        glBindTexture(GL_TEXTURE_2D, tex_id_); // use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glUseProgram(0);
        glEnable(GL_DEPTH_TEST);
    }

    void Filter::resize(callback::WindowSize const size)
    {
        auto const [width, height] = size;
        glBindFramebuffer(GL_FRAMEBUFFER, fb_id_);
        glBindTexture(GL_TEXTURE_2D, tex_id_);
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_id_, 0);
        }
        glBindRenderbuffer(GL_RENDERBUFFER, rb_id_);
        {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        }
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
