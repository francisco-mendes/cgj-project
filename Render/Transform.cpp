#include "Transform.h"

namespace render
{
    Matrix4 Transform::toMatrix() const
    {
        return Matrix4::translation(position) * rotation.toRotationMatrix() * Matrix4::scaling(scaling);
    }

    Transform Transform::interpolate(float const scale, Transform const& start, Transform const& end)
    {
        auto const pos = start.position + (end.position - start.position) * scale;
        auto const rot = Quaternion::slerp(scale, start.rotation, end.rotation);
        auto const sca = start.scaling + (end.scaling - start.scaling) * scale;
        return {pos, rot, sca};
    }

    Animation::Animation(Transform start, Transform target)
        : start_ {std::move(start)},
          target_ {std::move(target)},
          state_ {0.0},
          active_ {false},
          reversed_ {false}
    {}

    Transform Animation::advance(double const elapsed_sec)
    {
        constexpr auto Speed = 0.5;
        if (reversed_)
        {
            state_ -= elapsed_sec * Speed;
            if (state_ <= 0)
            {
                state_    = 0;
                active_   = false;
                reversed_ = false;
                return start_;
            }
            return Transform::interpolate(state_, start_, target_);
        }

        state_ += elapsed_sec * Speed;
        if (state_ >= 1)
        {
            state_    = 1;
            active_   = false;
            reversed_ = true;
            return target_;
        }
        return Transform::interpolate(state_, start_, target_);
    }

    bool Animation::active() const { return active_; }
    bool Animation::reversed() const { return reversed_; }

    void Animation::activate() { active_ = true; }
    void Animation::reverse() { reversed_ = !reversed_; }
}
