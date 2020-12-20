#pragma once

#include <filesystem>

#include "Utils.h"

namespace config
{
    struct WindowSize
    {
        int width, height;
    };

    struct Version
    {
        int major, minor;
    };

    enum class Mode : bool
    {
        Windowed = false,
        Fullscreen = true,
    };

    enum class VSync: bool
    {
        Off = false,
        On = true,
    };

    struct Window
    {
        Ptr<char const> title;
        WindowSize      size;
        Mode            mode;
        VSync           vsync;
    };

    struct Snapshot
    {
        std::filesystem::path dir;
    };

    struct Settings
    {
        Version  version;
        Window   window;
        Snapshot snapshot;
    };
}
