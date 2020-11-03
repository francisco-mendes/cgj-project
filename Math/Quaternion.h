#pragma once

#include "Vector.h"
#include "Matrix.h"

struct Quaternion
{
    static Quaternion fromVector(Vector4 components);
    static Quaternion fromParts(float t, Vector3 vec);
    static Quaternion fromAngleAxis(Radians angle, Vector3 axis);

    std::pair<Radians, Vector4> toAngleAxis() const;
    std::pair<float, Vector3>   toParts() const;
    Vector4                     toVector() const;
    Matrix4                     toMatrix() const;

    float quadrance() const;
    float magnitude() const;

    Quaternion absolute() const;
    Quaternion normalized() const;
    Quaternion cleaned() const;

    Quaternion conjugated() const;
    Quaternion inverted() const;

    static Quaternion lerp(float scale, Quaternion start, Quaternion end);
    static Quaternion slerp(float scale, Quaternion start, Quaternion end);

    float t, x, y, z;
};

Quaternion operator+(Quaternion left, Quaternion right);
Quaternion operator-(Quaternion left, Quaternion right);
Quaternion operator*(Quaternion left, Quaternion right);

Quaternion operator*(Quaternion left, float right);
Quaternion operator*(float left, Quaternion right);

bool operator==(Quaternion left, Quaternion right);
bool operator!=(Quaternion left, Quaternion right);

std::ostream& operator<<(std::ostream& os, Quaternion q);
