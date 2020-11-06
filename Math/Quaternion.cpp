#include "Quaternion.h"

Quaternion Quaternion::identity()
{
    return {1, 0, 0, 0};
}

Quaternion Quaternion::fromComponents(Vector4 const components)
{
    auto const [t, x, y, z] = components;
    return {t, x, y, z};
}

Quaternion Quaternion::fromRotationMatrix(Matrix4 const& rotation)
{
    const auto [a, b, c, _] = rotation.trace();
    auto const w            = std::sqrt(std::max(0.f, 1 + a + b + c)) / 2;
    auto const tx           = std::sqrt(std::max(0.f, 1 + a - b - c)) / 2;
    auto const ty           = std::sqrt(std::max(0.f, 1 - a + b - c)) / 2;
    auto const tz           = std::sqrt(std::max(0.f, 1 - a - b + c)) / 2;

    auto const x = std::copysign(tx, rotation[9] - rotation[6]);
    auto const y = std::copysign(ty, rotation[2] - rotation[8]);
    auto const z = std::copysign(tz, rotation[4] - rotation[1]);

    return {w, x, y, z};
}

Quaternion Quaternion::fromParts(float const t, Vector3 const vec)
{
    auto const [x, y, z] = vec;
    return {t, x, y, z};
}

Quaternion Quaternion::fromAngleAxis(Radians const angle, Vector3 const axis)
{
    auto const t         = cos(angle / 2.f);
    auto const scale     = sin(angle / 2.f);
    auto const [x, y, z] = axis.normalized() * scale;

    Quaternion const res = {t, x, y, z};
    return res.cleaned().normalized();
}

Quaternion Quaternion::angleBetween(Vector3 const vec1, Vector3 const vec2)
{
    auto const norm      = std::sqrt(vec1.quadrance() * vec2.quadrance());
    auto const real_part = norm + vec1 * vec2;
    return fromParts(real_part, vec1 % vec2).cleaned().normalized();
}

std::pair<Radians, Vector4> Quaternion::toAngleAxis() const
{
    auto const [t, v] = normalized().toParts();
    auto const angle  = 2.f * std::acos(t);
    auto const scale  = std::sqrt(1.f - t * t);

    auto const [x, y, z] = scale < Epsilon ? Vector3 {1.f, 0.f, 0.f} : v * (1 / scale);

    return {angle, {x, y, z, 1}};
}

std::pair<float, Vector3> Quaternion::toParts() const { return {t, {x, y, z}}; }

Vector4 Quaternion::toComponents() const { return {t, x, y, z}; }

Matrix4 Quaternion::toRotationMatrix() const
{
    auto const [t, x, y, z] = normalized();

    auto const tx = 2.f * t * x;
    auto const ty = 2.f * t * y;
    auto const tz = 2.f * t * z;

    auto const xx = 2.f * x * x;
    auto const xy = 2.f * x * y;
    auto const xz = 2.f * x * z;

    auto const yy = 2.f * y * y;
    auto const yz = 2.f * y * z;
    auto const zz = 2.f * z * z;

    return {
        1.f - yy - zz, xy - tz, xz + ty, 0,
        xy + tz, 1.f - xx - zz, yz - tx, 0,
        xz - ty, yz + tx, 1.f - xx - yy, 0,
        0, 0, 0, 1
    };
}

float Quaternion::quadrance() const { return toComponents().quadrance(); }
float Quaternion::magnitude() const { return std::sqrt(quadrance()); }

Quaternion Quaternion::absolute() const { return fromComponents(toComponents().absolute()); }
Quaternion Quaternion::normalized() const { return fromComponents(toComponents().normalized()); }
Quaternion Quaternion::cleaned() const { return fromComponents(toComponents().cleaned()); }

Quaternion Quaternion::conjugated() const { return {t, -x, -y, -z}; }
Quaternion Quaternion::inverted() const { return conjugated() * (1.f / quadrance()); }

Quaternion Quaternion::lerp(float const scale, Quaternion const start, Quaternion const end)
{
    auto const angle   = start.toComponents() * end.toComponents();
    auto const k_start = 1.f - scale;
    auto const k_end   = angle > 0 ? scale : -scale;
    return (k_start * start + k_end * end).cleaned().normalized();
}

Quaternion Quaternion::slerp(float const scale, Quaternion const start, Quaternion const end)
{
    auto const angle   = start.toComponents() * end.toComponents();
    auto const k_start = std::sin((1.f - scale) * angle) / std::sin(angle);
    auto const k_end   = std::sin(scale * angle) / std::sin(angle);
    return (k_start * start + k_end * end).cleaned().normalized();
}

Quaternion operator+(Quaternion const left, Quaternion const right)
{
    return Quaternion::fromComponents(left.toComponents() + right.toComponents());
}

Quaternion operator-(Quaternion const left, Quaternion const right)
{
    return Quaternion::fromComponents(left.toComponents() - right.toComponents());
}

Quaternion operator*(Quaternion const left, Quaternion const right)
{
    return {
        left.t * right.t - left.x * right.x - left.y * right.y - left.z * right.z,
        left.t * right.x + left.x * right.t + left.y * right.z - left.z * right.y,
        left.t * right.y - left.x * right.z + left.y * right.t + left.z * right.x,
        left.t * right.z + left.x * right.y - left.y * right.x + left.z * right.t,
    };
}

Quaternion operator*(Quaternion const left, float const right)
{
    return Quaternion::fromComponents(left.toComponents() * right);
}

Quaternion operator*(float const left, Quaternion const right) { return right * left; }

bool operator==(Quaternion const left, Quaternion const right) { return left.toComponents() == right.toComponents(); }
bool operator!=(Quaternion const left, Quaternion const right) { return !(left == right); }

std::ostream& operator<<(std::ostream& os, Quaternion const q)
{
    return os << '(' << q.t << ", " << q.x << "i, " << q.y << "j, " << q.z << "k)";
}
