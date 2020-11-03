#include "Vector.h"
#include "Matrix.h"

Vector2 Vector2::filled(float const fill) { return {fill, fill}; }
Vector3 Vector3::filled(float const fill) { return {fill, fill, fill}; }
Vector4 Vector4::filled(float const fill) { return {fill, fill, fill, fill}; }


Vector2 Vector2::from(float const array[2]) { return {array[0], array[1]}; }
Vector3 Vector3::from(float const array[3]) { return {array[0], array[1], array[2]}; }
Vector4 Vector4::from(float const array[4]) { return {array[0], array[1], array[2], array[3]}; }


Vector3::operator Vector2() const { return {x, y}; }
Vector4::operator Vector2() const { return {x, y}; }

Vector2::operator Vector3() const { return {x, y, 0}; }
Vector4::operator Vector3() const { return {x, y, z}; }

Vector2::operator Vector4() const { return {x, y, 0, 0}; }
Vector3::operator Vector4() const { return {x, y, z, 0}; }


Vector2 Vector2::absolute() const { return {std::abs(x), std::abs(y)}; }
Vector3 Vector3::absolute() const { return {std::abs(x), std::abs(y), std::abs(z)}; }
Vector4 Vector4::absolute() const { return {std::abs(x), std::abs(y), std::abs(z), std::abs(w)}; }


Vector2 Vector2::normalized() const
{
    auto const m = magnitude();
    return {x / m, y / m};
}

Vector3 Vector3::normalized() const
{
    auto const m = magnitude();
    return {x / m, y / m, z / m};
}

Vector4 Vector4::normalized() const
{
    auto const m = magnitude();
    return {x / m, y / m, z / m, w / m};
}


Vector2 Vector2::cleaned() const
{
    auto const abs = absolute();
    auto const nx  = abs.x > Epsilon ? x : 0;
    auto const ny  = abs.y > Epsilon ? y : 0;
    return {nx, ny};
}

Vector3 Vector3::cleaned() const
{
    auto const abs = absolute();
    auto const nx  = abs.x > Epsilon ? x : 0;
    auto const ny  = abs.y > Epsilon ? y : 0;
    auto const nz  = abs.z > Epsilon ? z : 0;
    return {nx, ny, nz};
}

Vector4 Vector4::cleaned() const
{
    auto const abs = absolute();
    auto const nx  = abs.x > Epsilon ? x : 0;
    auto const ny  = abs.y > Epsilon ? y : 0;
    auto const nz  = abs.z > Epsilon ? z : 0;
    auto const nw  = abs.w > Epsilon ? w : 0;
    return {nx, ny, nz, nw};
}


float Vector2::quadrance() const { return *this * *this; }
float Vector3::quadrance() const { return *this * *this; }
float Vector4::quadrance() const { return *this * *this; }


float Vector2::magnitude() const { return std::sqrt(quadrance()); }
float Vector3::magnitude() const { return std::sqrt(quadrance()); }
float Vector4::magnitude() const { return std::sqrt(quadrance()); }


Vector2 operator-(Vector2 const vec) { return {-vec.x, -vec.y}; }
Vector3 operator-(Vector3 const vec) { return {-vec.x, -vec.y, -vec.z}; }
Vector4 operator-(Vector4 const vec) { return {-vec.x, -vec.y, -vec.z, -vec.w}; }


Vector2 operator+(Vector2 const left, Vector2 const right) { return {left.x + right.x, left.y + right.y}; }

Vector3 operator+(Vector3 const left, Vector3 const right)
{
    return {left.x + right.x, left.y + right.y, left.z + right.z};
}

Vector4 operator+(Vector4 const left, Vector4 const right)
{
    return {left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w};
}


Vector2 operator+(Vector2 const vec, float const scalar) { return vec + Vector2::filled(scalar); }
Vector3 operator+(Vector3 const vec, float const scalar) { return vec + Vector3::filled(scalar); }
Vector4 operator+(Vector4 const vec, float const scalar) { return vec + Vector4::filled(scalar); }

Vector2 operator+(float const scalar, Vector2 const vec) { return vec + scalar; }
Vector3 operator+(float const scalar, Vector3 const vec) { return vec + scalar; }
Vector4 operator+(float const scalar, Vector4 const vec) { return vec + scalar; }


Vector2 operator-(Vector2 const left, Vector2 const right) { return {left.x - right.x, left.y - right.y}; }

Vector3 operator-(Vector3 const left, Vector3 const right)
{
    return {left.x - right.x, left.y - right.y, left.z - right.z};
}

Vector4 operator-(Vector4 const left, Vector4 const right)
{
    return {left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w};
}


Vector2 operator-(Vector2 const vec, float const scalar) { return vec - Vector2::filled(scalar); }
Vector3 operator-(Vector3 const vec, float const scalar) { return vec - Vector3::filled(scalar); }
Vector4 operator-(Vector4 const vec, float const scalar) { return vec - Vector4::filled(scalar); }

Vector2 operator-(float const scalar, Vector2 const vec) { return Vector2::filled(scalar) - vec; }
Vector3 operator-(float const scalar, Vector3 const vec) { return Vector3::filled(scalar) - vec; }
Vector4 operator-(float const scalar, Vector4 const vec) { return Vector4::filled(scalar) - vec; }


Vector2 operator*(Vector2 const vec, float const scalar) { return {vec.x * scalar, vec.y * scalar}; }
Vector3 operator*(Vector3 const vec, float const scalar) { return {vec.x * scalar, vec.y * scalar, vec.z * scalar}; }

Vector4 operator*(Vector4 const vec, float const scalar)
{
    return {vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar};
}


Vector2 operator*(float const scalar, Vector2 const vec) { return vec * scalar; }
Vector3 operator*(float const scalar, Vector3 const vec) { return vec * scalar; }
Vector4 operator*(float const scalar, Vector4 const vec) { return vec * scalar; }


float operator*(Vector2 const left, Vector2 const right) { return left.x * right.x + left.y * right.y; }

float operator*(Vector3 const left, Vector3 const right)
{
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

float operator*(Vector4 const left, Vector4 const right)
{
    return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
}


Vector3 operator%(Vector3 const left, Vector3 const right) { return Matrix3::dual(left) * right; }
Vector4 operator%(Vector4 const left, Vector4 const right) { return Matrix3::dual(left) * Vector3(right); }


bool operator==(Vector2 const left, Vector2 const right)
{
    auto const [x, y] = (left - right).absolute();
    return x < Epsilon && y < Epsilon;
}

bool operator==(Vector3 const left, Vector3 const right)
{
    auto const [x, y, z] = (left - right).absolute();
    return x < Epsilon && y < Epsilon && z < Epsilon;
}

bool operator==(Vector4 const left, Vector4 const right)
{
    auto const [x, y, z, w] = (left - right).absolute();
    return x < Epsilon && y < Epsilon && z < Epsilon && w < Epsilon;
}


bool operator!=(Vector2 const left, Vector2 const right) { return !(left == right); }
bool operator!=(Vector3 const left, Vector3 const right) { return !(left == right); }
bool operator!=(Vector4 const left, Vector4 const right) { return !(left == right); }


std::ostream& operator<<(std::ostream& os, Vector2 const v)
{
    os << '{' << v.x << ", " << v.y << '}';
    return os;
}

std::ostream& operator<<(std::ostream& os, Vector3 const v)
{
    os << '{' << v.x << ", " << v.y << ", " << v.z << '}';
    return os;
}

std::ostream& operator<<(std::ostream& os, Vector4 const v)
{
    os << '{' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << '}';
    return os;
}
