#include "Engine.h"

#include "Error.h"
#include <FreeImage.h>
#include <string>

namespace engine
{
    Engine::Engine(GlfwHandle glfw, render::Scene scene, config::Settings const& settings)
        : glfw_ {std::move(glfw)},
          scene {std::move(scene)},
          mesh_controller {&this->scene.meshes_},
          texture_controller {&this->scene.textures_},
          pipeline_controller {&this->scene.shaders_},
          filter_controller {&this->scene.filters_},
          object_controller {this->scene.root_.get()}
    {
        auto const [width, height] = settings.window.size;

        size_         = {width, height};
        snapshot_dir_ = settings.paths.snapshot;
        snap_num_     = 1;

        create_directory(snapshot_dir_);
        glfw_.registerEngine(this);
        setupErrorCallback(this);
    }

    std::unique_ptr<Engine> Engine::init(GlfwHandle glfw, render::Scene scene, config::Settings const& settings)
    {
        return std::unique_ptr<Engine> {new Engine(std::move(glfw), std::move(scene), settings)};
    }

    Ptr<GLFWwindow>      Engine::window() { return glfw_.window_; }
    callback::WindowSize Engine::windowSize() const { return size_; }

    void Engine::resize(callback::WindowSize const size)
    {
        glViewport(0, 0, size.width, size.height);
        scene.resizeFilters(size);
        size_ = size;
    }

    void Engine::resetControllers()
    {
        mesh_controller.reset();
        texture_controller.reset();
        pipeline_controller.reset();
    }

    void Engine::setControllers(Ptr<render::Object const> const object)
    {
        if (object != nullptr)
        {
            mesh_controller.set(object->mesh);
            texture_controller.set(object->texture);
            pipeline_controller.set(object->shaders);
        }
    }

    void Engine::snapshot()
    {
        auto const [width, height] = size_;
        auto const stride          = width * 3;

        std::vector<BYTE> pixels(stride * height);

        glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());

        // Convert to FreeImage format & save to file
        auto const image    = FreeImage_ConvertFromRawBits(pixels.data(), width, height, stride, 24, 0, 0, 0, false);
        auto const filename = "snapshot" + std::to_string(snap_num_) + ".png";
        auto const path     = snapshot_dir_ / filename;

        #ifdef _DEBUG
        std::cerr << "saving snapshot to " << path << std::endl;
        auto const res = FreeImage_SaveU(FIF_PNG, image, path.c_str(), PNG_DEFAULT);
        std::cerr << (res ? "succeeded" : "failed") << std::endl;
        #else
        FreeImage_SaveU(FIF_PNG, image, path.c_str(), PNG_DEFAULT);
        #endif
        snap_num_++;
        // Free resources
        FreeImage_Unload(image);
    }

    void Engine::run()
    {
        auto last_time = glfw_.getTime();

        while (!glfw_.windowClosing())
        {
            auto const now   = glfw_.getTime();
            auto const delta = now - last_time;
            last_time        = now;

            ////////////////////////////////
            // Render scene
            // Double Buffers
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            scene.render(*this, delta);
            ///////////////////////////////

            glfw_.swapBuffers();
            glfw_.pollEvents();
        }
    }

    void Engine::terminate() { glfw_.closeWindow(); }
}
