#pragma once

#include <variant>
#include <GL/glew.h>

#include "../Math/Matrix.h"
#include "../Math/Quaternion.h"
#include "../Math/Vector.h"
#include "../Callback.h"

namespace render
{
    class CameraController;

    class Camera
    {
    public:
        using Rotation = std::variant<Matrix4, Quaternion>;

    private:
        GLuint cam_matrices_id_;

        Vector3  focus_;
        float    distance_;
        Rotation rotation_;

        Vector3 position_ = {};

    public:
        Camera(float distance, Vector3 focus, GLuint view_id);

        Camera(Camera const&)            = delete;
        Camera& operator=(Camera const&) = delete;

        Camera(Camera&& other) noexcept;
        Camera& operator=(Camera&&) noexcept;

        ~Camera();

        void swapRotationMode();

        void rotate(CameraController const& controller);
        void rotate(double frame_delta);

        void update(callback::WindowSize size, Vector2 drag_delta, float zoom);


        [[nodiscard]] Vector3 position() const { return position_; }
    private:
        [[nodiscard]] Matrix4  rotationMatrix(Vector2 drag_delta) const;
        [[nodiscard]] Rotation fullRotation(Vector2 drag_delta) const;
    };

    class CameraController
    {
        bool    dragging_;
        Vector2 drag_start_, drag_now_;
        double  scroll_;
    public:
        Camera camera;

        CameraController(Camera camera);

        void startDrag(callback::MousePosition mouse_position);
        void rotateDrag(callback::MousePosition mouse_position);
        void finishDrag();
        void scroll(double by);
        void update(callback::WindowSize size, double frame_delta);

        [[nodiscard]] Vector2 dragDelta() const;
        [[nodiscard]] bool    isDragging() const;
        [[nodiscard]] float   scrollDelta(double frame_delta);
    };
}
