#include "Camera.h"
#include "../Math/Matrix.h"

#include <iostream>

namespace render
{
    constexpr auto AngleScale = static_cast<float>(DPi / 1000);
    constexpr auto Facing     = Vector3 {0, 0, 1};

    namespace
    {
        Matrix4 projectionMatrix(callback::WindowSize const size)
        {
            return Matrix4::perspective(30.f, size.width / static_cast<float>(size.height), 0.5f, 100.f);
        }
    }

    Camera::Camera(float const distance, Vector3 const focus, GLuint const view_id)
        : focus_ {focus},
          distance_ {distance},
          rotation_ {Matrix4::identity()}
    {
        glGenBuffers(1, &cam_matrices_id_);
        glBindBuffer(GL_UNIFORM_BUFFER, cam_matrices_id_);
        {
            glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4) * 2, nullptr, GL_STREAM_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, view_id, cam_matrices_id_);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }


    Camera::Camera(Camera&& other) noexcept
        : cam_matrices_id_ {std::exchange(other.cam_matrices_id_, 0)},
          focus_ {other.focus_},
          distance_ {other.distance_},
          rotation_ {std::exchange(other.rotation_, {})}
    {}

    Camera& Camera::operator=(Camera&& other) noexcept
    {
        if (this != &other)
        {
            cam_matrices_id_ = std::exchange(other.cam_matrices_id_, 0);
            focus_           = other.focus_;
            distance_        = other.distance_;
            rotation_        = std::exchange(other.rotation_, {});
        }
        return *this;
    }

    Camera::~Camera()
    {
        if (cam_matrices_id_ != 0)
        {
            glDeleteBuffers(1, &cam_matrices_id_);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }

    void Camera::swapRotationMode()
    {
        if (auto const mat = std::get_if<Matrix4>(&rotation_))
        {
            rotation_ = Quaternion::fromRotationMatrix(*mat);
            #ifdef _DEBUG
            std::cerr << "swapped rotation mode to Quaternion (Post Mult)" << std::endl;
            #endif
        }
        else if (auto const quat = std::get_if<Quaternion>(&rotation_))
        {
            rotation_ = quat->toRotationMatrix();
            #if _DEBUG
            std::cerr << "swapped rotation mode to Euler (Pre Mult)" << std::endl;
            #endif
        }
    }

    void Camera::rotate(CameraController const& controller) { rotation_ = fullRotation(controller.dragDelta()); }

    void Camera::rotate(double const frame_delta)
    {
        constexpr auto Spin = Vector2 {50.f, 0};

        rotation_ = fullRotation(Spin * static_cast<float>(frame_delta));
    }

    void Camera::update(callback::WindowSize const size, Vector2 const drag_delta, float const zoom)
    {
        distance_ = std::max(0.f, distance_ + zoom);

        auto const projection_matrix = projectionMatrix(size);
        auto const rotation_matrix   = rotationMatrix(drag_delta);

        auto const view_matrix =
            Matrix4::translation({0, 0, -distance_}) * rotation_matrix.transposed() * Matrix4::translation(-focus_);

        glBindBuffer(GL_UNIFORM_BUFFER, cam_matrices_id_);
        {
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4), view_matrix.transposed().inner);
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix4), sizeof(Matrix4), projection_matrix.inner);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        position_ = rotation_matrix * Vector3 {0, 0, distance_};
    }

    Matrix4 Camera::rotationMatrix(Vector2 const drag_delta) const
    {
        if (auto const rotation = fullRotation(drag_delta); auto const mat = std::get_if<Matrix4>(&rotation))
            return *mat;
        else
            return std::get<Quaternion>(rotation).toRotationMatrix();
    }

    Camera::Rotation Camera::fullRotation(Vector2 const drag_delta) const
    {
        auto const [dx, dy] = drag_delta * -AngleScale;

        if (auto const orientation = std::get_if<Matrix4>(&rotation_))
        {
            auto const drag = Matrix4::rotation(Axis::X, dy) * Matrix4::rotation(Axis::Y, dx);
            return *orientation * drag;
        }

        auto const orientation = std::get<Quaternion>(rotation_);
        auto const drag        =
            Quaternion::fromAngleAxis(dy, Axis::X) *
            Quaternion::fromAngleAxis(dx, Axis::Y);

        return drag * orientation;
    }


    CameraController::CameraController(Camera camera)
        : dragging_ {},
          drag_start_ {},
          drag_now_ {},
          scroll_ {},
          camera {std::move(camera)} {}

    void CameraController::startDrag(callback::MousePosition const mouse_position)
    {
        dragging_   = true;
        drag_start_ = drag_now_ = mouse_position;
    }

    void CameraController::rotateDrag(callback::MousePosition const mouse_position) { drag_now_ = mouse_position; }

    void CameraController::finishDrag()
    {
        camera.rotate(*this);
        dragging_   = false;
        drag_start_ = drag_now_ = Vector2 {};
    }

    void CameraController::scroll(double const by) { scroll_ += by; }

    void CameraController::update(callback::WindowSize const size, double const frame_delta)
    {
        camera.update(size, dragDelta(), scrollDelta(frame_delta));
    }

    Vector2 CameraController::dragDelta() const { return drag_now_ - drag_start_; }
    bool    CameraController::isDragging() const { return dragging_; }

    float CameraController::scrollDelta(double const frame_delta)
    {
        constexpr auto Min   = 1.5;
        constexpr auto Scale = 0.75;
        constexpr auto Speed = 5.0;

        auto const magnitude = std::min(std::abs(scroll_ * Speed * frame_delta), Min);
        auto const change    = std::copysign(magnitude, scroll_);
        scroll_ -= change;
        return static_cast<float>(-change * Scale);
    }
}
