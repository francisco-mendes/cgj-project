#include "Engine.h"

#include "Error.h"

namespace engine
{
    Engine::Engine(GlfwHandle glfw, render::Scene scene, int width_, int height_)
        : glfw_ {std::move(glfw)},
          scene_ {std::move(scene)}
    {
        width = width_;
        height = height_;
        snapsPath = "C:\\CG_Snaps";
        snapNum = 1;
        glfw_.registerEngine(this);
        setupErrorCallback(this);
    }

    std::unique_ptr<Engine> Engine::init(GlfwHandle glfw, render::Scene scene, int width_, int height_)
    {
        return std::unique_ptr<Engine> {new Engine(std::move(glfw), std::move(scene), width_, height_)};
    }

    Ptr<GLFWwindow> Engine::window() { return glfw_.window_; }
    render::Scene&  Engine::scene() { return scene_; }

    void engine::Engine::snapshot()
    {
        // Make the BYTE array, factor of 3 because it's RBG.
        BYTE* pixels = new BYTE[3 * (width * height)];

        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

        // Convert to FreeImage format & save to file
        FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);
        std::string path = std::string(snapsPath) + "\\snapshot" + std::to_string(snapNum) + ".png";
        char* pathPointer = strcpy(new char[path.length() + 1], path.c_str());
        FreeImage_Save(FIF_PNG, image, pathPointer, 0);
        snapNum++;
        // Free resources
        FreeImage_Unload(image);
        delete[] pixels;
        free(pathPointer);
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
