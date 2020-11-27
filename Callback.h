#pragma once

#include "Math/Vector.h"

#include <Gl/glew.h>
#include <GLFW/glfw3.h>

namespace engine
{
    class Engine; // Forward declaration to avoid circular dependencies
}

namespace callback
{
    struct WindowSize
    {
        int width, height;
    };

    struct MouseButton
    {
        int button, action, mods;
    };

    struct MousePosition
    {
        double x, y;

        operator Vector2() const;
    };

    struct KeyboardButton
    {
        int key, scancode, action, mods;
    };

    void setupCallbacks(GLFWwindow* window, engine::Engine* engine);
    void onManualWindowClose(GLFWwindow* window);
}

namespace config::hooks
{
    void onWindowClose(engine::Engine& engine);
    void onWindowResize(engine::Engine& engine, callback::WindowSize size);

    void onMouseButton(engine::Engine& engine, callback::MouseButton button);
    void onMouseMove(engine::Engine& engine, callback::MousePosition position);
    void onMouseScroll(engine::Engine& engine, double scroll);

    void onKeyboardButton(engine::Engine& engine, callback::KeyboardButton button);
}
