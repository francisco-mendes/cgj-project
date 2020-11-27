#pragma once

#include <variant>
#include <GL/glew.h>

#include "../Math/Matrix.h"
#include "../Math/Quaternion.h"
#include "../Math/Vector.h"
#include "../Callback.h"

namespace render
{
    class Controller;

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
        GLuint     cam_matrices_id_;
        Projection projection_;
        Vector3    focus_;
        float      distance_;
        Rotation   rotation_;

    public:
        Camera(float distance, Vector3 focus, GLuint view_id);

        Camera(Camera const&)            = delete;
        Camera& operator=(Camera const&) = delete;

        Camera(Camera&& other) noexcept;
        Camera& operator=(Camera&&) noexcept;

        ~Camera();
        
        void swapProjection();
        void swapRotationMode();

        void rotate(Controller const& controller);
        void rotate(double frame_delta);

        void update(Vector2 drag_delta, float zoom);

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
        Controller(Camera camera);

        void startDrag(callback::MousePosition mouse_position);
        void rotateDrag(callback::MousePosition mouse_position);
        void finishDrag();
        void scroll(double by);
        void update(double frame_delta);

        [[nodiscard]] Vector2 dragDelta() const;
        [[nodiscard]] bool    isDragging() const;
        [[nodiscard]] float   scrollDelta(double frame_delta);
        [[nodiscard]] Camera& camera();
    };
}