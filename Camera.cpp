#include "Camera.h"
#include "Math/Matrix.h"

#include <iostream>

constexpr auto AngleScale = static_cast<float>(DPi / 1000);
constexpr auto Facing     = Vector3 {0, 0, 1};

Matrix4 projection(Camera::Projection const type)
{
    switch (type)
    {
        case Camera::Orthogonal:
            return Matrix4::orthographic(-2.0f, 2.0f, -2.0f, 2.0f, 0.5f, 100.0f);
        case Camera::Perspective:
            return Matrix4::perspective(30.f, 640.f / 480.f, 0.5f, 100.f);
        default:
            throw std::invalid_argument("Invalid projection type");
    }
}

MousePosition::operator Vector2() const { return {static_cast<float>(x), static_cast<float>(y)}; }

Camera::Camera(float const distance, Vector3 const focus, GLuint const view_id)
    : projection_ {Perspective},
      focus_ {focus},
      distance_ {distance},
      rotation_ {Matrix4::identity()}
{
    glGenBuffers(1, &buffer_id_);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer_id_);
    {
        glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4) * 2, nullptr, GL_STREAM_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, view_id, buffer_id_);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::swapProjection()
{
    switch (projection_)
    {
        case Orthogonal:
            projection_ = Perspective;
            break;
        case Perspective:
            projection_ = Orthogonal;
            break;
    }
}

void Camera::swapRotationMode()
{
    if (auto const mat = std::get_if<Matrix4>(&rotation_))
    {
        rotation_ = Quaternion::fromRotationMatrix(*mat);
        #ifdef _DEBUG
        std::cerr << "swapped rotation mode to Quaternion" << std::endl;
        #endif
    }
    else if (auto const quat = std::get_if<Quaternion>(&rotation_))
    {
        rotation_ = quat->toRotationMatrix();
        #if _DEBUG
        std::cerr << "swapped rotation mode to Euler" << std::endl;
        #endif
    }
}

void Camera::rotate(Controller const& controller) { rotation_ = fullRotation(controller.dragDelta()); }

void Camera::rotate(double const frame_delta)
{
    constexpr auto Spin = Vector2 {50.f, 0};

    rotation_ = fullRotation(Spin * static_cast<float>(frame_delta));
}

void Camera::update(Vector2 const drag_delta, float const zoom)
{
    distance_ += zoom;

    auto const projection_matrix = projection(projection_);
    auto const rotation_matrix   = rotationMatrix(drag_delta);

    auto const view_matrix =
        Matrix4::translation({0, 0, -distance_}) * rotation_matrix.transposed() * Matrix4::translation(-focus_);

    glBindBuffer(GL_UNIFORM_BUFFER, buffer_id_);
    {
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4), view_matrix.transposed().inner);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix4), sizeof(Matrix4), projection_matrix.inner);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::cleanup()
{
    glDeleteBuffers(1, &buffer_id_);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

    if (auto const old_rotation = std::get_if<Matrix4>(&rotation_))
    {
        auto const new_rotation = Matrix4::rotation(Axis::X, dy) * Matrix4::rotation(Axis::Y, dx);
        return new_rotation * *old_rotation;
    }

    auto const old_rotation = std::get<Quaternion>(rotation_);
    auto const new_rotation =
        Quaternion::fromAngleAxis(dy, Axis::X) *
        Quaternion::fromAngleAxis(dx, Axis::Y);

    return old_rotation * new_rotation;
}


Controller::Controller(Camera&& camera)
    : dragging_ {},
      drag_start_ {},
      drag_now_ {},
      scroll_ {},
      camera_ {std::move(camera)} {}

void Controller::startDrag(MousePosition const mouse_position)
{
    dragging_   = true;
    drag_start_ = drag_now_ = mouse_position;
}

void Controller::rotateDrag(MousePosition const mouse_position) { drag_now_ = mouse_position; }

void Controller::finishDrag()
{
    camera_.rotate(*this);
    dragging_   = false;
    drag_start_ = drag_now_ = Vector2 {};
}

void Controller::scroll(double const by) { scroll_ += by; }

void Controller::update(double const frame_delta) { camera_.update(dragDelta(), scrollDelta(frame_delta)); }

Vector2 Controller::dragDelta() const { return drag_now_ - drag_start_; }
bool    Controller::isDragging() const { return dragging_; }

float Controller::scrollDelta(double const frame_delta)
{
    constexpr auto Min   = 1.5;
    constexpr auto Scale = 0.75;
    constexpr auto Speed = 5.0;

    auto const magnitude = std::min(std::abs(scroll_ * Speed * frame_delta), Min);
    auto const change    = std::copysign(magnitude, scroll_);
    scroll_ -= change;
    return static_cast<float>(-change * Scale);
}

Camera& Controller::camera() { return camera_; }
