#include "GlfwHandle.h"
#include <iostream>
#include <stdexcept>

#include "Error.h"

namespace
{
    GLFWwindow* setupWindow(config::Window const& window)
    {
        const auto [title, size, fullscreen, vsync] = window;

        auto const monitor = static_cast<bool>(fullscreen) ? glfwGetPrimaryMonitor() : nullptr;
        auto const win     = glfwCreateWindow(size.width, size.height, title, monitor, nullptr);
        if (!win)
            throw std::runtime_error("Unable to create Window");

        glfwMakeContextCurrent(win);
        glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursor(win, glfwCreateStandardCursor(GLFW_HAND_CURSOR));
        glfwSwapInterval(static_cast<bool>(vsync));

        return win;
    }
}

namespace engine
{
    GlfwHandle::GlfwHandle(config::Settings const& settings)
    {
        auto const& [version, window, _] = settings;

        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW");

        try
        {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version.major);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version.minor);
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

            window_ = setupWindow(window);

            #if _DEBUG
            std::cerr << "GLFW " << glfwGetVersionString() << std::endl;
            #endif
        }
        catch (...)
        {
            glfwTerminate();
            throw;
        }
    }

    GlfwHandle::GlfwHandle(GlfwHandle&& other) noexcept
        : window_ {std::exchange(other.window_, nullptr)} {}

    GlfwHandle& GlfwHandle::operator=(GlfwHandle&& other) noexcept
    {
        if (this != &other)
        {
            window_ = std::exchange(other.window_, nullptr);
        }
        return *this;
    }

    GlfwHandle::~GlfwHandle()
    {
        if (window_)
        {
            glfwDestroyWindow(window_);
            glfwTerminate();
        }
    }

    double GlfwHandle::getTime() const { return glfwGetTime(); }

    bool GlfwHandle::windowClosing() const { return glfwWindowShouldClose(window_); }


    void GlfwHandle::registerEngine(Engine* engine)
    {
        callback::setupCallbacks(window_, engine);
    }

    void GlfwHandle::closeWindow()
    {
        callback::onManualWindowClose(window_);
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }

    void GlfwHandle::swapBuffers() { glfwSwapBuffers(window_); }
    void GlfwHandle::pollEvents() { glfwPollEvents(); }
}
