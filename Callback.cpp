#include "Callback.h"

namespace
{
    using namespace config::hooks;
    
    [[nodiscard]] engine::Engine* getEngine(GLFWwindow* win)
    {
        return static_cast<engine::Engine*>(glfwGetWindowUserPointer(win));
    }

    void windowCloseCallback(GLFWwindow* win)
    {
        onWindowClose(*getEngine(win));
    }

    void windowSizeCallback(GLFWwindow* win, int const width, int const height)
    {
        onWindowResize(*getEngine(win), {width, height});
    }

    void mouseCallback(GLFWwindow* win, int const button, int const action, int const mods)
    {
        onMouseButton(*getEngine(win), {button, action, mods});
    }

    void cursorCallback(GLFWwindow* win, double const x_pos, double const y_pos)
    {
        onMouseMove(*getEngine(win), {x_pos, y_pos});
    }

    void scrollCallback(GLFWwindow* win, [[maybe_unused]] double const _, double const y_pos)
    {
        onMouseScroll(*getEngine(win), y_pos);
    }

    void keyCallback(GLFWwindow* win, int const key, int const scancode, int const action, int const mods)
    {
        onKeyboardButton(*getEngine(win), {key, scancode, action, mods});
    }
}

namespace callback
{
    MousePosition::operator Vector2() const { return {static_cast<float>(x), static_cast<float>(y)}; }

    void setupCallbacks(GLFWwindow* window, engine::Engine* engine)
    {
        glfwSetWindowCloseCallback(window, windowCloseCallback);
        glfwSetWindowSizeCallback(window, windowSizeCallback);

        glfwSetMouseButtonCallback(window, mouseCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetCursorPosCallback(window, cursorCallback);

        glfwSetKeyCallback(window, keyCallback);

        glfwSetWindowUserPointer(window, engine);
    }

    void onManualWindowClose(GLFWwindow* window) { windowCloseCallback(window); }
}
