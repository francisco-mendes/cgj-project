#pragma once
#include "../Config.h"

namespace engine
{
    class GlInit
    {
    public:
        explicit GlInit(config::Settings const& settings);
    };
}

namespace config::hooks
{
    void setupOpenGl(Settings const& settings);
}