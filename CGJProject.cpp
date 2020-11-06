#include <array>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Error.h"
#include "Shader.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"

constexpr auto ViewWidth  = 640;
constexpr auto ViewHeight = 480;

constexpr auto Offset = -3.f;

struct Bindings
{
    constexpr static GLuint Position = 0;
    constexpr static GLuint Colors   = 1;

    constexpr static GLuint Camera = 0;

    GLint  model_id;
    GLuint camera_id;

    void bindInputs(GLuint const program_id)
    {
        glBindAttribLocation(program_id, Position, "in_Position");
        glBindAttribLocation(program_id, Colors, "in_Color");
    }

    void bindUniforms(GLuint const program_id)
    {
        model_id  = glGetUniformLocation(program_id, "ModelMatrix");
        camera_id = glGetUniformBlockIndex(program_id, "CameraMatrices");
        glUniformBlockBinding(program_id, camera_id, Camera);
    }
};

enum class Shape : int
{
    LLeft = 0,
    LRight,
    SLeft,
    SRight,
    T,
    Line,
    Square
};

struct TetrisObj
{
    Shape  shape;
    GLuint vao[2], vbo[2][2];

    TetrisObj() = default;
    TetrisObj(float size_x, float size_y, Vector4 color, Shape shape);

    std::array<Vector2, 4> shapeArray() const;

    void draw(Matrix4 const& rotation, Matrix4 const& translation) const;
    void cleanup();
};

ShaderProgram<Bindings> Shaders;

TetrisObj  Line, LLeft, T1, T2;
Controller Cam;

/////////////////////////////////////////////////////////////////////// VAOs & VBOs

void bind_face(GLuint& vao, GLuint vbo[2], Vector4 positions[4], Vector4 colors[4], GLubyte order[6])
{
    constexpr auto PosSize   = sizeof(Vector4) * 4;
    constexpr auto ColorSize = sizeof(Vector4) * 4;
    constexpr auto OrderSize = sizeof(Vector4) * 6;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    {
        glGenBuffers(2, vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        {
            glBufferData(GL_ARRAY_BUFFER, PosSize + ColorSize, nullptr, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, PosSize, positions);
            glBufferSubData(GL_ARRAY_BUFFER, PosSize, ColorSize, colors);

            glEnableVertexAttribArray(Bindings::Position);
            glVertexAttribPointer(Bindings::Position, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

            glEnableVertexAttribArray(Bindings::Colors);
            glVertexAttribPointer(Bindings::Colors, 4, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(PosSize));
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, OrderSize, order, GL_STATIC_DRAW);
        }
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

TetrisObj::TetrisObj(float const size_x, float const size_y, Vector4 const color, Shape const shape)
    : shape {shape}
{
    constexpr auto back_delta = Vector4 {0.5, 0.5, 0.5, 0};

    Vector4 positions[4] {
        {-0.5f * size_x, -0.5f * size_y, 0, 1},
        {0.5f * size_x, -0.5f * size_y, 0, 1},
        {0.5f * size_x, 0.5f * size_y, 0, 1},
        {-0.5f * size_x, 0.5f * size_y, 0, 1}
    };

    Vector4 front_colors[] {color, color, color, color};
    Vector4 back_colors[] {color - back_delta, color - back_delta, color - back_delta, color - back_delta};

    GLubyte front_order[] {
        0, 1, 2,
        2, 3, 0
    };
    GLubyte back_order[] {
        0, 3, 2,
        2, 1, 0
    };

    bind_face(vao[0], vbo[0], positions, front_colors, front_order);
    bind_face(vao[1], vbo[1], positions, back_colors, back_order);
}

void TetrisObj::cleanup()
{
    for (int i = 0; i < 2; i++)
    {
        glBindVertexArray(vao[i]);
        glDisableVertexAttribArray(Bindings::Position);
        glDisableVertexAttribArray(Bindings::Colors);
        glDeleteBuffers(2, vbo[i]);
        glDeleteVertexArrays(1, &vao[i]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}


void TetrisObj::draw(Matrix4 const& rotation, Matrix4 const& translation) const
{
    static auto const Margin = Matrix4::scaling(Vector3::filled(0.9f));

    for (int i = 0; i < 2; i++)
    {
        glBindVertexArray(vao[i]);

        for (auto const sqr : shapeArray())
        {
            auto matrix = rotation * translation * Matrix4::translation(sqr) * Margin;
            glUniformMatrix4fv(Shaders.bindings().model_id, 1, GL_TRUE, matrix.inner);
            glDrawElements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_BYTE, nullptr);
        }
    }
}

std::array<Vector2, 4> TetrisObj::shapeArray() const
{
    switch (shape)
    {
        case Shape::LLeft:
            return std::array<Vector2, 4> {{{0, 0}, {1, 0}, {1, 1}, {1, 2}}};
        case Shape::LRight:
            return std::array<Vector2, 4> {{{1, 0}, {0, 0}, {0, 1}, {0, 2}}};
        case Shape::SLeft:
            return std::array<Vector2, 4> {{{1, 0}, {1, 1}, {0, 1}, {0, 2}}};
        case Shape::SRight:
            return std::array<Vector2, 4> {{{0, 0}, {0, 1}, {1, 1}, {1, 2}}};
        case Shape::T:
            return std::array<Vector2, 4> {{{0, 1}, {1, 0}, {1, 1}, {2, 1}}};
        case Shape::Line:
            return std::array<Vector2, 4> {{{0, 0}, {0, 1}, {0, 2}, {0, 3}}};
        case Shape::Square:
            return std::array<Vector2, 4> {{{0, 0}, {0, 1}, {1, 1}, {1, 0}}};
        default:
            throw std::invalid_argument("Tetris shape is invalid");
    }
}

/////////////////////////////////////////////////////////////////////// SCENE
void drawScene()
{
    constexpr auto angle    = Pi / 4;
    auto const     rotation = Matrix4::rotation(Axis::Z, angle);

    Line.draw(rotation, Matrix4::translation({-1.5, -1.5, Offset}));
    LLeft.draw(rotation, Matrix4::translation({0.5, -1.5, Offset}));
    T2.draw(rotation, Matrix4::translation({-0.5, 0.5, Offset}));
    T1.draw(rotation * Matrix4::rotation(Axis::Z, Pi / 2), Matrix4::translation({-1.5, -0.5, Offset}));
}

///////////////////////////////////////////////////////////////////// CALLBACKS

void windowCloseCallback(GLFWwindow* win)
{
    TetrisObj objs[] {Line, LLeft, T1, T2};

    Shaders.cleanup();
    for (auto& obj : objs) obj.cleanup();
}

void windowSizeCallback(GLFWwindow* win, int win_x, int win_y)
{
    glViewport(0, 0, win_x, win_y);
}

void glfwErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error: " << description << std::endl;
}


void mouseCallback(GLFWwindow* win, int button, int action, int mods)
{
    if (button != GLFW_MOUSE_BUTTON_1) return;

    if (action == GLFW_PRESS)
    {
        double x_pos, y_pos;
        glfwGetCursorPos(win, &x_pos, &y_pos);
        Cam.startDrag({x_pos, y_pos});

        #if _DEBUG
        std::cerr << "start pos: " << x_pos << ", " << y_pos << std::endl;
        #endif
    }
    else if (action == GLFW_RELEASE)
    {
        Cam.finishDrag();
    }
}

void scrollCallback([[maybe_unused]] GLFWwindow* win, double const x_pos, double const y_pos)
{
    #if _DEBUG
    std::cerr << "zoom: " << x_pos << " " << y_pos << std::endl;
    #endif

    Cam.scroll(y_pos);
}

void dragCallback(GLFWwindow* win, double const x_pos, double const y_pos)
{
    if (auto const button_state = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_1); button_state == GLFW_PRESS)
    {
        #if _DEBUG
        std::cerr << "position: " << x_pos << ", " << y_pos << std::endl;
        #endif

        Cam.rotateDrag({x_pos, y_pos});
    }
}

void keyCallback(GLFWwindow* win, int const key, int, int const action, int)
{
    if (action == GLFW_PRESS)
        switch (key)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(win, GLFW_TRUE);
                windowCloseCallback(win);
                break;
            case GLFW_KEY_P:
                Cam.camera().swapProjection();
                break;
            case GLFW_KEY_G:
                Cam.camera().swapRotationMode();
                break;
        }
}


///////////////////////////////////////////////////////////////////////// SETUP

GLFWwindow* setupWindow(
    int         win_x,
    int         win_y,
    const char* title,
    int         is_fullscreen,
    int         is_vsync
)
{
    GLFWmonitor* monitor = is_fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    GLFWwindow*  win     = glfwCreateWindow(win_x, win_y, title, monitor, nullptr);
    if (!win)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(win);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursor(win, glfwCreateStandardCursor(GLFW_HAND_CURSOR));
    glfwSwapInterval(is_vsync);
    return win;
}

void setupCallbacks(GLFWwindow* win)
{
    glfwSetWindowCloseCallback(win, windowCloseCallback);
    glfwSetWindowSizeCallback(win, windowSizeCallback);

    glfwSetMouseButtonCallback(win, mouseCallback);
    glfwSetScrollCallback(win, scrollCallback);
    glfwSetCursorPosCallback(win, dragCallback);
    glfwSetKeyCallback(win, keyCallback);
}

GLFWwindow* setupGLFW(int gl_major, int gl_minor, int win_x, int win_y, char const* title, bool fullscreen, bool vsync)
{
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_minor);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);


    GLFWwindow* win = setupWindow(win_x, win_y, title, fullscreen, vsync);
    setupCallbacks(win);

    #if _DEBUG
    std::cerr << "GLFW " << glfwGetVersionString() << std::endl;
    #endif

    return win;
}

void setupGLEW()
{
    glewExperimental = GL_TRUE;
    // Allow extension entry points to be loaded even if the extension isn't 
    // present in the driver's extensions string.
    GLenum result = glewInit();
    if (result != GLEW_OK)
    {
        std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
        exit(EXIT_FAILURE);
    }
    glGetError();
    // You might get GL_INVALID_ENUM when loading GLEW.
}

void checkOpenGLInfo()
{
    const GLubyte* renderer    = glGetString(GL_RENDERER);
    const GLubyte* vendor      = glGetString(GL_VENDOR);
    const GLubyte* version     = glGetString(GL_VERSION);
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::cerr << "OpenGL Renderer: " << renderer << " (" << vendor << ")" << std::endl;
    std::cerr << "OpenGL version " << version << std::endl;
    std::cerr << "GLSL version " << glslVersion << std::endl;
}

void setupOpenGL(int win_x, int win_y)
{
    #if _DEBUG
    checkOpenGLInfo();
    #endif

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glDepthRange(0.0, 1.0);
    glClearDepth(1.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glViewport(0, 0, win_x, win_y);
}

GLFWwindow* setup(int major, int minor, int win_x, int win_y, const char* title, bool fullscreen, bool vsync)
{
    GLFWwindow* win = setupGLFW(major, minor, win_x, win_y, title, fullscreen, vsync);
    setupGLEW();
    setupOpenGL(win_x, win_y);
    setupErrorCallback();

    Shaders = ShaderProgram<Bindings> {
        Shader::fromFile(Shader::Vertex, "Shaders/vert.glsl"),
        Shader::fromFile(Shader::Fragment, "Shaders/frag.glsl")
    };

    Cam = Camera(10, {0, 0, Offset}, Bindings::Camera);

    Line  = TetrisObj(1, 1, {0.75, 0, 0}, Shape::Line);
    LLeft = TetrisObj(1, 1, {0.75, 0, 0.75}, Shape::LLeft);
    T1    = TetrisObj(1, 1, {0.75, 0.75, 0}, Shape::T);
    T2    = TetrisObj(1, 1, {0.0, 0.75, 0.75}, Shape::T);

    return win;
}

////////////////////////////////////////////////////////////////////////// RUN

void display(GLFWwindow* win, double elapsed_sec)
{
    glUseProgram(Shaders.programId());

    if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_1) != GLFW_PRESS)
        Cam.camera().rotate(elapsed_sec);
    Cam.update(elapsed_sec);

    drawScene();

    glBindVertexArray(0);
    glUseProgram(0);
}

void run(GLFWwindow* win)
{
    double last_time = glfwGetTime();
    while (!glfwWindowShouldClose(win))
    {
        double time         = glfwGetTime();
        double elapsed_time = time - last_time;
        last_time           = time;

        // Double Buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        display(win, elapsed_time);
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    glfwDestroyWindow(win);
    glfwTerminate();
}

////////////////////////////////////////////////////////////////////////// MAIN

int main()
{
    try
    {
        int const  gl_major   = 4, gl_minor = 3;
        bool const fullscreen = false;
        bool const vsync      = true;

        GLFWwindow* win = setup(gl_major, gl_minor, ViewWidth, ViewHeight, "Tetris 2D", fullscreen, vsync);
        run(win);
        exit(EXIT_SUCCESS);
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
