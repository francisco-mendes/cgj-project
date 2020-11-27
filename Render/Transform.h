#pragma once
#include "../Math/Matrix.h"
#include "../Math/Quaternion.h"
#include "../Math/Vector.h"

namespace render
{
    struct Transform
    {
        Matrix4 toMatrix() const;

        Vector3    position = Vector3::filled(0);
        Quaternion rotation = Quaternion::identity();
        Vector3    scaling  = Vector3::filled(1);

        [[nodiscard]] static Transform interpolate(float scale, Transform const& start, Transform const& end);
    };

    class Animation
    {
        Transform start_;
        Transform target_;

        double state_;

        bool active_;
        bool reversed_;
    public:
        Animation(Transform start, Transform target);

        Transform advance(double elapsed_sec);

        [[nodiscard]] bool active() const;
        [[nodiscard]] bool reversed() const;

        void activate();
        void reverse();
    };
}
