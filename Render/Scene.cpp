#include "Scene.h"

#include <cassert>
#include <filesystem>
#include <optional>


#include "../Engine/Engine.h"

namespace render
{
    Scene::Scene(Builder&& builder): 
          meshes_ {std::move(builder.meshes)},
          shaders_ {std::move(builder.shaders)},
          textures_ {std::move(builder.textures)},
          filters_ {std::move(builder.filters)},
          root_ {std::move(builder.root)},
          default_shader_ {builder.default_shader},
          light_position {builder.light_position},
          camera_controller {*std::move(builder.camera)},
          shadowShader { "Assets/Shaders/Shadows/shadowMap_vert.glsl", "Assets/Shaders/Shadows/shadowMap_frag.glsl" }
    { 
        genShadowBuffers();
    }
 

    Scene Scene::setup([[maybe_unused]] engine::GlInit gl_init, config::Settings const& settings)
    {
        Builder b;
        config::hooks::setupScene(b, settings);
        assert(b.camera.has_value());
        assert(b.default_shader != nullptr);
        return b;
    }

    void Scene::render(engine::Engine& engine, double const elapsed_sec)
    {
        config::hooks::beforeRender(*this, engine, elapsed_sec);

        camera_controller.update(engine.windowSize(), elapsed_sec);
        scene_block_.update(camera_controller.camera.position(), light_position);
        root_->update(elapsed_sec);
        renderWithShadow(engine);
        glUseProgram(default_shader_->programId());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        root_->draw(Matrix4::identity(), default_shader_, *this);

        config::hooks::afterRender(*this, engine, elapsed_sec);
    }

    void Scene::animate()
    {
        root_->animate();
    }

    void Scene::resizeFilters(callback::WindowSize const size)
    {
        for (auto& filter : filters_) { filter.resize(size); }
    }

    Texture const& Scene::defaultTexture() const { return default_texture_; }



    void Scene::genShadowBuffers() {
        glGenFramebuffers(1, &depthMapFB);

        const unsigned int S_Width = 1024, S_Height = 1024;
        
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, S_Width, S_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glGenBuffers(1, &shadowMapBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, shadowMapBuffer);
        {
            glBufferData(GL_UNIFORM_BUFFER, sizeof(Vector3) * 2, nullptr, GL_STREAM_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, Pipeline::LightSpace, shadowMapBuffer);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFB);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    Matrix4 Scene::createLigthMatrix() {
        float nplane = 1, fplane = 7.5f;
        Matrix4 lightProj = Matrix4::orthographic(-10, 10, -10, 10, nplane, fplane);
        Matrix4 lightView = Matrix4::view(light_position, Vector3(), Vector3({ 0, 1, 0 }));
        Matrix4 lightSpaceMatrix = lightProj * lightView;
        return lightSpaceMatrix;
    }
    void Scene::renderWithShadow(engine::Engine& engine) {
        const unsigned int S_Width = 1024, S_Height = 1024;
        glViewport(0, 0, S_Width, S_Width);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFB);
        glClear(GL_DEPTH_BUFFER_BIT);

        glBindBuffer(GL_UNIFORM_BUFFER, depthMapFB);
        {
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4), createLigthMatrix().inner);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glUseProgram(default_shader_->programId()); 
        glCullFace(GL_FRONT);
        root_->draw(Matrix4::identity(), &shadowShader, *this,true);
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        auto const [width, height] = engine.windowSize();
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

}
