#include "GlInit.h"

#include <iostream>
#include <stdexcept>

#include <GL/glew.h>

namespace engine
{
    namespace
    {
        void setupGlew()
        {
            glewExperimental = true;
            // Allow extension entry points to be loaded even if the extension isn't 
            // present in the driver's extensions string.
            if (auto const result = glewInit(); result != GLEW_OK)
            {
                std::string message = "ERROR glewInit: ";
                message += reinterpret_cast<Ptr<char const>>(glewGetString(result));
                throw std::runtime_error(message);
            }
            glGetError();
            // You might get GL_INVALID_ENUM when loading GLEW.
        }

        void logOpenGlInfo()
        {
            auto const renderer     = glGetString(GL_RENDERER);
            auto const vendor       = glGetString(GL_VENDOR);
            auto const version      = glGetString(GL_VERSION);
            auto const glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

            std::cerr << "OpenGL Renderer: " << renderer << " (" << vendor << ")" << std::endl;
            std::cerr << "OpenGL version " << version << std::endl;
            std::cerr << "GLSL version " << glsl_version << std::endl;
        }
    }

    GlInit::GlInit(config::Settings const& settings)
    {
        setupGlew();

        #ifdef _DEBUG
        logOpenGlInfo();
        #endif

        config::hooks::setupOpenGl(settings);
    }
}
