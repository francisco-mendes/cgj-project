#include "Matrix.h"

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

Matrix4 Matrix4::rotation(Axis const ax, Radians const angle)
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
        default:
            throw std::invalid_argument("Invalid axis provided for rotation matrix");
    }
}

Matrix4 Matrix4::rotation(Vector3 const axis, Radians const angle)
{
    auto const [x, y, z] = axis;
    auto const sine      = sin(angle);
    auto const cosine    = cos(angle);
    auto const inv_cos   = 1 - cosine;

    return {
        cosine + pow(x, 2) * inv_cos,
        x * y * inv_cos - z * sine,
        x * z * inv_cos + y * sine,
        0,
        x * y * inv_cos + z * sine,
        cosine + pow(y, 2) * inv_cos,
        y * z * inv_cos - x * sine,
        0,
        x * z * inv_cos - y * sine,
        z * y * inv_cos + x * sine,
        cosine + pow(z, 2) * inv_cos,
        0,
        0, 0, 0, 1
    };
}

Matrix4 Matrix4::view(Vector3 const eye, Vector3 const center, Vector3 const up)
{
    auto const v = (center - eye).normalized();
    auto const s = (v % up).normalized();
    auto const u = s % v;

    Matrix4 const camera {
        s.x, s.y, s.z, 0,
        u.x, u.y, u.z, 0,
        -v.x, -v.y, -v.z, 0,
        0, 0, 0, 1
    };

    return (camera * translation(-eye)).transposed();
}

Matrix4 Matrix4::orthographic(
    float const left,
    float const right,
    float const bottom,
    float const top,
    float const near,
    float const far
)
{
    auto const scale     = scaling({2 / (right - left), 2 / (top - bottom), 2 / (far - near)});
    auto const translate = translation({-(left + right) / 2, -(bottom + top) / 2, -(near + far) / 2});

    auto res = scale * translate;
    res[10]  = 2 / (near - far);
    return res.transposed();
}

Matrix4 Matrix4::perspective(Degrees const fov, float const aspect, float const near, float const far)
{
    auto const d = 1 / tan(fov * DegToRad / 2);

    return Matrix4 {
        d / aspect, 0, 0, 0,
        0, d, 0, 0,
        0, 0, (near + far) / (near - far), (2 * near * far) / (near - far),
        0, 0, -1, 0
    }.transposed();
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

// trace

Vector4 Matrix4::trace() const { return {inner[0], inner[5], inner[10], inner[15]}; }

//inverse

Matrix2 Matrix2::inverted() const
{
    auto const det = determinant();
    if (std::abs(det) < Epsilon) throw std::invalid_argument("Singular matrix can't have an inverse");

    Matrix2 const r {inner[3], -inner[1], -inner[2], inner[0]};
    return r * (1 / det);
}


Matrix3 Matrix3::inverted() const
{
    auto const det = determinant();
    if (std::abs(det) < Epsilon) throw std::invalid_argument("Singular matrix can't have an inverse");

    auto const trans = transposed();

    Matrix2 const m0 {trans[4], trans[5], trans[7], trans[8]};
    Matrix2 const m1 {trans[3], trans[5], trans[6], trans[8]};
    Matrix2 const m2 {trans[3], trans[4], trans[6], trans[7]};

    Matrix2 const m3 {trans[1], trans[2], trans[7], trans[8]};
    Matrix2 const m4 {trans[0], trans[2], trans[6], trans[8]};
    Matrix2 const m5 {trans[0], trans[1], trans[6], trans[7]};

    Matrix2 const m6 {trans[1], trans[2], trans[4], trans[5]};
    Matrix2 const m7 {trans[0], trans[2], trans[3], trans[5]};
    Matrix2 const m8 {trans[0], trans[1], trans[3], trans[4]};

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
    return std::abs(a - b) < Epsilon;
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
    Matrix2        res;

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
    Matrix3        res;

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
    Matrix4        res;

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

Vector2 operator*(Matrix2 const& left, Vector2 const right)
{
    auto const [x, y] = right;
    return {
        left[0] * x + left[1] * y,
        left[2] * x + left[3] * y
    };
}

Vector3 operator*(Matrix3 const& left, Vector3 const right)
{
    auto const [x, y, z] = right;
    return {
        left[0] * x + left[1] * y + left[2] * z,
        left[3] * x + left[4] * y + left[5] * z,
        left[6] * x + left[7] * y + left[8] * z
    };
}

Vector4 operator*(Matrix4 const& left, Vector4 const right)
{
    auto const [x, y, z, w] = right;

    return {
        left[0] * x + left[1] * y + left[2] * z + left[3] * w,
        left[4] * x + left[5] * y + left[6] * z + left[7] * w,
        left[8] * x + left[9] * y + left[10] * z + left[11] * w,
        1
    };
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
