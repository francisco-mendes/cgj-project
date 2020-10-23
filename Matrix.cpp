#include "Matrix.h"

#include <cmath>
#include <iostream>
#include <limits>


//identity

Matrix2 Matrix2::identity()
{
    return {
        1, 0,
        0, 1
    };
}

Matrix3 Matrix3::identity()
{
    return {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };
}

Matrix4 Matrix4::identity()
{
    return {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

//dual

Matrix3 Matrix3::dual(Vector3 const of)
{
    return {
        0, -of.z, of.y,
        of.z, 0, -of.x,
        -of.y, of.x, 0
    };
}

//scaling

Matrix4 Matrix4::scaling(Vector3 const by)
{
    return {
        by.x, 0, 0, 0,
        0, by.y, 0, 0,
        0, 0, by.z, 0,
        0, 0, 0, 1
    };
}

//translation

Matrix4 Matrix4::translation(Vector3 const by)
{
    return {
        1, 0, 0, by.x,
        0, 1, 0, by.y,
        0, 0, 1, by.z,
        0, 0, 0, 1
    };
}

//rotation 

Matrix4 Matrix4::rotation(Axis const ax, float const angle)
{
    auto const cos = std::cos(angle);
    auto const sin = std::sin(angle);

    switch (ax)
    {
        case Axis::X:
            return {
                1, 0, 0, 0,
                0, cos, -sin, 0,
                0, sin, cos, 0,
                0, 0, 0, 1
            };
        case Axis::Y:
            return {
                cos, 0, sin, 0,
                0, 1, 0, 0,
                -sin, 0, cos, 0,
                0, 0, 0, 1
            };
        case Axis::Z:
            return {
                cos, -sin, 0, 0,
                sin, cos, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            };
    }
    std::cerr << "Invalid axis provided" << std::endl;
    exit(1);
}

//determinat

float Matrix2::determinant() const { return inner[0] * inner[3] - inner[1] * inner[2]; }

float Matrix3::determinant() const
{
    return inner[0] * (inner[4] * inner[8] - inner[5] * inner[7])
        - inner[1] * (inner[3] * inner[8] - inner[5] * inner[6])
        + inner[2] * (inner[3] * inner[7] - inner[4] * inner[6]);
}

//transpose

Matrix2 Matrix2::transposed() const
{
    Matrix2 res;
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
        {
            res[i * N + j] = inner[i + j * N];
        }
    return res;
}

Matrix3 Matrix3::transposed() const
{
    Matrix3 res;
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
        {
            res[i * N + j] = inner[i + j * N];
        }
    return res;
}

Matrix4 Matrix4::transposed() const
{
    Matrix4 res;
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
        {
            res[i * N + j] = inner[i + j * N];
        }
    return res;
}

//inverse

Matrix2 Matrix2::inverted() const
{
    auto const det = determinant();
    if (std::abs(det) < std::numeric_limits<float>::epsilon())
        throw std::invalid_argument(
            "Singular matrix can't have an inverse"
        );

    Matrix2 const r {inner[3], -inner[1], -inner[2], inner[0]};
    return r * (1 / det);
}


Matrix3 Matrix3::inverted() const
{
    auto const det = determinant();
    if (std::abs(det) < std::numeric_limits<float>::epsilon())
        throw std::invalid_argument(
            "Singular matrix can't have an inverse"
        );

    auto const trans = transposed();

    Matrix2 const m0 = {trans[4], trans[5], trans[7], trans[8]};
    Matrix2 const m1 = {trans[3], trans[5], trans[6], trans[8]};
    Matrix2 const m2 = {trans[3], trans[4], trans[6], trans[7]};

    Matrix2 const m3 = {trans[1], trans[2], trans[7], trans[8]};
    Matrix2 const m4 = {trans[0], trans[2], trans[6], trans[8]};
    Matrix2 const m5 = {trans[0], trans[1], trans[6], trans[7]};

    Matrix2 const m6 = {trans[1], trans[2], trans[4], trans[5]};
    Matrix2 const m7 = {trans[0], trans[2], trans[3], trans[5]};
    Matrix2 const m8 = {trans[0], trans[1], trans[3], trans[4]};

    Matrix3 const res = {
        m0.determinant(),
        -m1.determinant(),
        m2.determinant(),
        -m3.determinant(),
        m4.determinant(),
        -m5.determinant(),
        m6.determinant(),
        -m7.determinant(),
        m8.determinant()
    };
    return res * (1 / det);
}

//operator[]

float Matrix2::operator[](size_t const index) const { return inner[index]; }
float Matrix3::operator[](size_t const index) const { return inner[index]; }
float Matrix4::operator[](size_t const index) const { return inner[index]; }

float& Matrix2::operator[](size_t const index) { return inner[index]; }
float& Matrix3::operator[](size_t const index) { return inner[index]; }
float& Matrix4::operator[](size_t const index) { return inner[index]; }


//operator ==

inline bool eps_eq(float const a, float const b)
{
    return std::abs(a - b) < std::numeric_limits<float>::epsilon();
}

bool operator==(Matrix2 const& left, Matrix2 const& right)
{
    for (auto i = 0; i < left.Len; i++) if (!eps_eq(left[i], right[i])) return false;
    return true;
}

bool operator==(Matrix3 const& left, Matrix3 const& right)
{
    for (auto i = 0; i < left.Len; i++) if (!eps_eq(left[i], right[i])) return false;
    return true;
}

bool operator==(Matrix4 const& left, Matrix4 const& right)
{
    for (auto i = 0; i < left.Len; i++) if (!eps_eq(left[i], right[i])) return false;
    return true;
}

//operator + 

Matrix2 operator+(Matrix2 const& left, Matrix2 const& right)
{
    Matrix2 res;
    for (auto i = 0; i < left.Len; i++) { res[i] = left[i] + right[i]; }
    return res;
}

Matrix3 operator+(Matrix3 const& left, Matrix3 const& right)
{
    Matrix3 res;
    for (auto i = 0; i < left.Len; i++) { res[i] = left[i] + right[i]; }
    return res;
}


Matrix4 operator+(Matrix4 const& left, Matrix4 const& right)
{
    Matrix4 res;
    for (auto i = 0; i < left.Len; i++) { res[i] = left[i] + right[i]; }
    return res;
}

//operator -

Matrix2 operator-(Matrix2 const& left, Matrix2 const& right)
{
    Matrix2 res;
    for (auto i = 0; i < left.Len; i++) { res[i] = left[i] - right[i]; }
    return res;
}

Matrix3 operator-(Matrix3 const& left, Matrix3 const& right)
{
    Matrix3 res;
    for (auto i = 0; i < left.Len; i++) { res[i] = left[i] - right[i]; }
    return res;
}


Matrix4 operator-(Matrix4 const& left, Matrix4 const& right)
{
    Matrix4 res;
    for (auto i = 0; i < left.Len; i++) { res[i] = left[i] - right[i]; }
    return res;
}

//operator * scalar

Matrix2 operator*(Matrix2 const& left, float const right)
{
    Matrix2 res;
    for (auto i = 0; i < left.Len; i++) { res[i] = left[i] * right; }
    return res;
}

Matrix3 operator*(Matrix3 const& left, float const right)
{
    Matrix3 res;
    for (auto i = 0; i < left.Len; i++) { res[i] = left[i] * right; }
    return res;
}


Matrix4 operator*(Matrix4 const& left, float const right)
{
    Matrix4 res;
    for (auto i = 0; i < left.Len; i++) { res[i] = left[i] * right; }
    return res;
}

Matrix2 operator*(float const left, Matrix2 const& right) { return right * left; }
Matrix3 operator*(float const left, Matrix3 const& right) { return right * left; }
Matrix4 operator*(float const left, Matrix4 const& right) { return right * left; }

//operator * matrix

Matrix2 operator*(Matrix2 const& left, Matrix2 const& right)
{
    constexpr auto N = left.N;
    Matrix2 res;

    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
        {
            res[i * N + j] = left[i * N] * right[j] + left[i * N + 1] * right[j + N];
        }
    return res;
}

Matrix3 operator*(Matrix3 const& left, Matrix3 const& right)
{
    constexpr auto N = left.N;
    Matrix3 res;

    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
        {
            res[i * N + j] =
                left[i * N] * right[j] +
                left[i * N + 1] * right[j + N] +
                left[i * N + 2] * right[j + 2 * N];
        }
    return res;
}


Matrix4 operator*(Matrix4 const& left, Matrix4 const& right)
{
    constexpr auto N = left.N;
    Matrix4 res;

    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
        {
            res[i * N + j] =
                left[i * N] * right[j] +
                left[i * N + 1] * right[j + N] +
                left[i * N + 2] * right[j + 2 * N] +
                left[i * N + 3] * right[j + 3 * N];
        }
    return res;
}

//operator * Vector

Vector2 operator*(Matrix2 const& mat, Vector2 const vec)
{
    auto [x, y] = vec;

    x = mat[0] * x + mat[1] * y;
    y = mat[2] * x + mat[3] * y;
    return {x, y};
}

Vector3 operator*(Matrix3 const& left, Vector3 const right)
{
    auto [x, y, z] = right;

    x = left[0] * x + left[1] * y + left[2] * z;
    y = left[3] * x + left[4] * y + left[5] * z;
    z = left[6] * x + left[7] * y + left[8] * z;
    return {x, y, z};
}

Vector4 operator*(Matrix4 const& left, Vector4 const right)
{
    auto [x, y, z, w] = right;

    x = left[0] * x + left[1] * y + left[2] * z + left[3] * w;
    y = left[4] * x + left[5] * y + left[6] * z + left[7] * w;
    z = left[8] * x + left[9] * y + left[10] * z + left[11] * w;
    return {x, y, z, 1};
}

std::ostream& operator<<(std::ostream& os, Matrix2 const& mat)
{
    for (auto i = 0; i < mat.Len; i += mat.N)
    {
        os << '(' << mat[i] << ", " << mat[i + 1] << ")\n";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, Matrix3 const& mat)
{
    for (auto i = 0; i < mat.Len; i += mat.N)
    {
        os << '(' << mat[i] << ", " << mat[i + 1] << ", " << mat[i + 2] << ")\n";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, Matrix4 const& mat)
{
    for (auto i = 0; i < mat.Len; i += mat.N)
    {
        os << '(' << mat[i] << ", " << mat[i + 1] << ", " << mat[i + 2] << ", " << mat[i + 3] << ")\n";
    }
    return os;
}
