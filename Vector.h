#pragma once

#include <ostream>
#include <cstdint>

enum class Axis: uint8_t { X, Y, Z };

struct Vector3;
struct Vector4;

struct Vector2
{
    static Vector2 filled(float fill);
    static Vector2 from(float const array[2]);

    operator Vector3() const;
    operator Vector4() const;

    float quadrance() const;
    float magnitude() const;

    Vector2 absolute() const;
    Vector2 normalized() const;
    Vector2 cleaned() const;

    float x, y;
};

Vector2 operator-(Vector2 vec);

Vector2 operator+(Vector2 left, Vector2 right);
Vector2 operator+(float scalar, Vector2 vec);
Vector2 operator+(Vector2 vec, float scalar);

Vector2 operator-(Vector2 left, Vector2 right);
Vector2 operator-(float scalar, Vector2 vec);
Vector2 operator-(Vector2 vec, float scalar);

Vector2 operator*(Vector2 vec, float scalar);
Vector2 operator*(float scalar, Vector2 vec);

float operator*(Vector2 left, Vector2 right);

bool operator==(Vector2 left, Vector2 right);
bool operator!=(Vector2 left, Vector2 right);

std::ostream& operator<<(std::ostream& os, Vector2 v);

struct Vector3
{
    static Vector3 filled(float fill);
    static Vector3 from(float const array[3]);

    operator Vector2() const;
    operator Vector4() const;

    float quadrance() const;
    float magnitude() const;

    Vector3 absolute() const;
    Vector3 normalized() const;
    Vector3 cleaned() const;

    float x, y, z;
};

Vector3 operator-(Vector3 vec);

Vector3 operator+(Vector3 left, Vector3 right);
Vector3 operator+(float scalar, Vector3 vec);
Vector3 operator+(Vector3 vec, float scalar);

Vector3 operator-(Vector3 left, Vector3 right);
Vector3 operator-(float scalar, Vector3 vec);
Vector3 operator-(Vector3 vec, float scalar);

Vector3 operator*(Vector3 vec, float scalar);
Vector3 operator*(float scalar, Vector3 vec);

float operator*(Vector3 left, Vector3 right);
Vector3 operator%(Vector3 left, Vector3 right);

bool operator==(Vector3 left, Vector3 right);
bool operator!=(Vector3 left, Vector3 right);

std::ostream& operator<<(std::ostream& os, Vector3 v);

struct Vector4
{
    static Vector4 filled(float fill);
    static Vector4 from(float const array[4]);

    operator Vector2() const;
    operator Vector3() const;

    float quadrance() const;
    float magnitude() const;

    Vector4 absolute() const;
    Vector4 normalized() const;
    Vector4 cleaned() const;

    float x, y, z, w;
};

Vector4 operator-(Vector4 vec);

Vector4 operator+(Vector4 left, Vector4 right);
Vector4 operator+(float scalar, Vector4 vec);
Vector4 operator+(Vector4 vec, float scalar);

Vector4 operator-(Vector4 left, Vector4 right);
Vector4 operator-(float scalar, Vector4 vec);
Vector4 operator-(Vector4 vec, float scalar);

Vector4 operator*(Vector4 vec, float scalar);
Vector4 operator*(float scalar, Vector4 vec);

float operator*(Vector4 left, Vector4 right);
Vector4 operator%(Vector4 left, Vector4 right);

bool operator==(Vector4 left, Vector4 right);
bool operator!=(Vector4 left, Vector4 right);

std::ostream& operator<<(std::ostream& os, Vector4 v);
