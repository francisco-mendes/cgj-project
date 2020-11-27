#pragma once

#include <cstdint>
#include <cmath>
#include <limits>
#include <ostream>
#include <istream>

constexpr double DPi = 3.141592653589793238463;
constexpr float  Pi  = 3.14159265358979f;

constexpr float DegToRad = Pi / 180;
constexpr float Epsilon  = std::numeric_limits<float>::epsilon();

enum class Axis: uint8_t { X, Y, Z };

using Radians = float;
using Degrees = float;
