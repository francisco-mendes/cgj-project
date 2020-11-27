#pragma once

#include "Utils.h"
#include "Vector.h"

struct Matrix2
{
    static constexpr auto N = 2;
    static constexpr auto Len = N * N;

    static Matrix2 identity();

    float determinant() const;
    Matrix2 transposed() const;
    Matrix2 inverted() const;

    float operator[](size_t index) const;
    float& operator[](size_t index);

    float inner[Len];
};

bool operator==(Matrix2 const& left, Matrix2 const& right);

Matrix2 operator+(Matrix2 const& left, Matrix2 const& right);
Matrix2 operator-(Matrix2 const& left, Matrix2 const& right);

Matrix2 operator*(Matrix2 const& left, float right);
Matrix2 operator*(Matrix2 const& left, Matrix2 const& right);
Vector2 operator*(Matrix2 const& left, Vector2 right);

std::ostream& operator<<(std::ostream& os, Matrix2 const& mat);

struct Matrix3
{
    static constexpr auto N = 3;
    static constexpr auto Len = N * N;

    static Matrix3 identity();
    static Matrix3 dual(Vector3 of);

    float determinant() const;
    Matrix3 transposed() const;
    Matrix3 inverted() const;

    float operator[](size_t index) const;
    float& operator[](size_t index);

    float inner[Len];
};

bool operator==(Matrix3 const& left, Matrix3 const& right);

Matrix3 operator+(Matrix3 const& left, Matrix3 const& right);
Matrix3 operator-(Matrix3 const& left, Matrix3 const& right);

Matrix3 operator*(Matrix3 const& left, float right);
Matrix3 operator*(float left, Matrix3 const& right);
Matrix3 operator*(Matrix3 const& left, Matrix3 const& right);
Vector3 operator*(Matrix3 const& left, Vector3 right);

std::ostream& operator<<(std::ostream& os, Matrix3 const& mat);

struct Matrix4
{
    static constexpr auto N = 4;
    static constexpr auto Len = N * N;

    static Matrix4 identity();
    static Matrix4 scaling(Vector3 by);
    static Matrix4 translation(Vector3 by);
    static Matrix4 rotation(Axis ax, Radians angle);
    static Matrix4 rotation(Vector3 axis, Radians angle);

    static Matrix4 view(Vector3 eye, Vector3 center, Vector3 up);
    static Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far);
    static Matrix4 perspective(Degrees fov, float aspect, float near, float far);

    Matrix4 transposed() const;
    Vector4 trace() const;

    float operator[](size_t index) const;
    float& operator[](size_t index);

    float inner[Len];
};

bool operator==(Matrix4 const& left, Matrix4 const& right);

Matrix4 operator+(Matrix4 const& left, Matrix4 const& right);
Matrix4 operator-(Matrix4 const& left, Matrix4 const& right);

Matrix4 operator*(Matrix4 const& left, float right);
Matrix4 operator*(Matrix4 const& left, Matrix4 const& right);
Vector4 operator*(Matrix4 const& left, Vector4 right);

std::ostream& operator<<(std::ostream& os, Matrix4 const& mat);