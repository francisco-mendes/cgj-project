#include "Engine.h"

#include "Error.h"

namespace engine
{
    Engine::Engine(GlfwHandle glfw, render::Scene scene)
        : glfw_ {std::move(glfw)},
          scene_ {std::move(scene)}
    {
        glfw_.registerEngine(this);
        setupErrorCallback(this);
    }

    std::unique_ptr<Engine> Engine::init(GlfwHandle glfw, render::Scene scene)
    {
        return std::unique_ptr<Engine> {new Engine(std::move(glfw), std::move(scene))};
    }

    Ptr<GLFWwindow> Engine::window() { return glfw_.window_; }
    render::Scene&  Engine::scene() { return scene_; }

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
