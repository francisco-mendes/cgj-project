#include "Engine.h"

#include "Error.h"
#include <FreeImage.h>
#include <string>

namespace engine
{
    Engine::Engine(GlfwHandle glfw, render::Scene scene, int width, int height)
        : glfw_ {std::move(glfw)},
          scene_ {std::move(scene)}
    {
        width_  = width;
        height_ = height;

        snapshot_dir_ = "./CG_Snaps";
        snap_num_     = 1;

        glfw_.registerEngine(this);
        setupErrorCallback(this);
    }

    std::unique_ptr<Engine> Engine::init(GlfwHandle glfw, render::Scene scene, int width_, int height_)
    {
        return std::unique_ptr<Engine> {new Engine(std::move(glfw), std::move(scene), width_, height_)};
    }

    Ptr<GLFWwindow> Engine::window() { return glfw_.window_; }
    render::Scene&  Engine::scene() { return scene_; }

    void Engine::snapshot()
    {
        // Make the BYTE array, factor of 3 because it's RBG.
        std::vector<BYTE> pixels;
        pixels.reserve(3 * (width_ * height_));

        glReadPixels(0, 0, width_, height_, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());

        // Convert to FreeImage format & save to file
        const auto image = FreeImage_ConvertFromRawBits(pixels.data(), width_, height_, 3 * width_, 24, 0, 0, 0, false);
        const auto filename = "snapshot" + std::to_string(snap_num_) + ".png";
        const auto path = snapshot_dir_ / filename;

        std::cerr << "path to file " << path << std::endl;
        std::cerr << FreeImage_SaveU(FIF_PNG, image, path.c_str(), PNG_DEFAULT) << std::endl;
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

            // Double Buffers
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            scene_.render(*this, delta);

            glfw_.swapBuffers();
            glfw_.pollEvents();
        }
    }

    void Engine::terminate() { glfw_.closeWindow(); }
}
