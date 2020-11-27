#pragma once

#include "../Config.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace engine
{
    class GlfwHandle
    {
        friend class Engine;

        GLFWwindow* window_;

    public:
        [[nodiscard]] explicit GlfwHandle(config::Settings const& settings);

        GlfwHandle(GlfwHandle const&)            = delete;
        GlfwHandle& operator=(GlfwHandle const&) = delete;

        GlfwHandle(GlfwHandle&& other) noexcept;
        GlfwHandle& operator=(GlfwHandle&& other) noexcept;

        ~GlfwHandle();

        [[nodiscard]] double getTime() const;
        [[nodiscard]] bool   windowClosing() const;

    private:
        void registerEngine(Engine* engine);
        void closeWindow();
        void swapBuffers();
        void pollEvents();
    };
}
