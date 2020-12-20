#include "Error.h"

#include <iostream>
#include <GL/glew.h>

namespace
{
    std::string_view errorSource(GLenum const source)
    {
        switch (source)
        {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "window system";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "shader compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "third party";
            case GL_DEBUG_SOURCE_APPLICATION: return "application";
            case GL_DEBUG_SOURCE_OTHER: return "other";
            default: exit(EXIT_FAILURE);
        }
    }

    std::string_view errorType(GLenum const type)
    {
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR: return "error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "deprecated behavior";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "undefined behavior";
            case GL_DEBUG_TYPE_PORTABILITY: return "portability issue";
            case GL_DEBUG_TYPE_PERFORMANCE: return "performance issue";
            case GL_DEBUG_TYPE_MARKER: return "stream annotation";
            case GL_DEBUG_TYPE_PUSH_GROUP: return "push group";
            case GL_DEBUG_TYPE_POP_GROUP: return "pop group";
            case GL_DEBUG_TYPE_OTHER_ARB: return "other";
            default: exit(EXIT_FAILURE);
        }
    }

    std::string_view errorSeverity(GLenum const severity)
    {
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH: return "high";
            case GL_DEBUG_SEVERITY_MEDIUM: return "medium";
            case GL_DEBUG_SEVERITY_LOW: return "low";
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "notification";
            default: exit(EXIT_FAILURE);
        }
    }

    void error(
        GLenum const                 source,
        GLenum const                 type,
        [[maybe_unused]] GLuint      id,
        GLenum const                 severity,
        [[maybe_unused]] GLsizei     length,
        Ptr<char const>                  message,
        [[maybe_unused]] Ptr<void const> user_param
    )
    {
        std::cerr << "GL ERROR:" << std::endl;
        std::cerr << "  source:     " << errorSource(source) << std::endl;
        std::cerr << "  type:       " << errorType(type) << std::endl;
        std::cerr << "  severity:   " << errorSeverity(severity) << std::endl;
        std::cerr << "  debug call: " << message << std::endl;
        if (auto const engine = (engine::Engine*) user_param; engine && severity == GL_DEBUG_SEVERITY_HIGH)
        {
            engine->terminate();
        }
    }
}

namespace engine
{
    void setupErrorCallback(Engine* engine)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(error, engine);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        // params: source, type, severity, count, ids, enabled
    }

    void glfwErrorCallback(int error, const char* description)
    {
        std::cerr << "GLFW Error: " << description << std::endl;
        std::cerr << "Press <return>.";
        std::cin.ignore();
    }
}
