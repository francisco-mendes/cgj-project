#pragma once

#include <cstdint>
#include <cmath>
#include <limits>
#include <ostream>

constexpr float PI = 3.1415927f;
constexpr float EPSILON = std::numeric_limits<float>::epsilon();

enum class Axis: uint8_t { X, Y, Z };
