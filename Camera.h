#pragma once

#include <variant>
#include <GL/glew.h>


#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Math/Vector.h"

class Controller;

struct MousePosition
{
    double x, y;

    operator Vector2() const;
};

class Camera
{
public:
    using Rotation = std::variant<Matrix4, Quaternion>;

    enum Projection: bool
    {
        Orthogonal,
        Perspective
    };

private:
    GLuint     buffer_id_;
    Projection projection_;
    Vector3    focus_,    up_;
    float      distance_, change_;
    Rotation   rotation_;

public:
    Camera() = default;
    Camera(float distance, Vector3 focus, Vector3 up, GLuint view_id);

    void swapProjection();
    void swapRotationMode();

    void rotate(Controller const& controller);
    void rotate(double frame_delta);

    void update(Vector2 drag_delta, float zoom);
    void cleanup();

private:
    [[nodiscard]] Matrix4  rotationMatrix(Vector2 drag_delta) const;
    [[nodiscard]] Rotation fullRotation(Vector2 drag_delta) const;
};

class Controller
{
    bool    dragging_;
    Vector2 drag_start_, drag_now_;
    double  scroll_;
    Camera  camera_;
public:
    Controller() = default;
    Controller(Camera&& camera);

    void startDrag(MousePosition mouse_position);
    void rotateDrag(MousePosition mouse_position);
    void finishDrag();
    void scroll(double by);
    void update(double frame_delta);

    [[nodiscard]] Vector2 dragDelta() const;
    [[nodiscard]] bool    isDragging() const;
    [[nodiscard]] float   scrollDelta(double frame_delta);
    [[nodiscard]] Camera& camera();
};
