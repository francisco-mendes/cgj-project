#define GLEW_STATIC

#include <array>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Error.h"
#include "Shader.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"


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

    void draw(Matrix4 const& scale, Matrix4 const& rotation, Matrix4 const& translation) const;
    void cleanup();
};

ShaderProgram Shaders;
TetrisObj     Line, LLeft, T1, T2;
Camera        Cam;

/////////////////////////////////////////////////////////////////////// VAOs & VBOs

void bind_face(GLuint& vao, GLuint vbo[2], Vector4 positions[4], Vector4 colors[4], GLubyte order[6])
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    {
        glGenBuffers(2, vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(colors), nullptr, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), positions);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(colors), colors);

            glEnableVertexAttribArray(Shaders.Position);
            glVertexAttribPointer(Shaders.Position, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

            glEnableVertexAttribArray(Shaders.Colors);
            glVertexAttribPointer(Shaders.Colors, 4, GL_FLOAT, GL_FALSE, 0, (void*) sizeof(positions));
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(order), order, GL_STATIC_DRAW);
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
        glDisableVertexAttribArray(Shaders.Position);
        glDisableVertexAttribArray(Shaders.Colors);
        glDeleteBuffers(2, vbo[i]);
        glDeleteVertexArrays(1, &vao[i]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}


void TetrisObj::draw(Matrix4 const& scale, Matrix4 const& rotation, Matrix4 const& translation) const
{
    static auto const SqMargin = Matrix4::scaling(Vector3::filled(0.9f));

    for (int i = 0; i < 2; i++)
    {
        glBindVertexArray(vao[i]);

        for (const auto& sqr : shapeArray())
        {
            auto matrix = scale * rotation * translation * Matrix4::translation(sqr) * SqMargin;
            glUniformMatrix4fv(Shaders.modelId(), 1, GL_TRUE, matrix.inner);
            glDrawElements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_BYTE, 0);
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
    constexpr auto angle    = PI / 4;
    auto const     scale    = Matrix4::scaling(Vector3::filled(0.25f));
    auto const     rotation = Matrix4::rotation(Axis::Z, angle);

    glUseProgram(Shaders.programId());

    Cam.moveCam();
    Cam.camBinds();

    Line.draw(scale, rotation, Matrix4::translation({-1.5, -1.5, -3}));
    LLeft.draw(scale, rotation, Matrix4::translation({0.5, -1.5, -3}));
    T2.draw(scale, rotation, Matrix4::translation({-0.5, 0.5, -3}));
    T1.draw(scale, rotation * Matrix4::rotation(Axis::Z, PI / 2), Matrix4::translation({-1.5, -0.5, -3}));

    glBindVertexArray(0);
    glUseProgram(0);
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
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) glfwSetInputMode(win, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSwapInterval(is_vsync);
    return win;
}


void mouseCallback(GLFWwindow* win, double xpos, double ypos)
{
    std::cout << "mouse: " << xpos << " " << ypos << std::endl;
    Cam.rotateCamera(xpos, ypos);
}

void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods)
{
    //std::cout << "key: " << key << " " << scancode << " " << action << " " << mods << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(win, GLFW_TRUE);
        windowCloseCallback(win);
    }
    else if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        Cam.ortho = !Cam.ortho;
    }
    else if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        Cam.mForward = 1;
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        Cam.mRight = -1;
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        Cam.mForward = -1;
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        Cam.mRight = 1;
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        Cam.mUp = 1;
    }
    else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
    {
        Cam.mUp = -1;
    }
    else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    {
        if (Cam.mForward != -1) Cam.mForward = 0;
    }
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    {
        if (Cam.mRight != 1) Cam.mRight = 0;
    }
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
    {
        if (Cam.mForward != 1) Cam.mForward = 0;
    }
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
    {
        if (Cam.mRight != -1) Cam.mRight = 0;
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    {
        if (Cam.mUp != -1) Cam.mUp = 0;
    }
    else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
    {
        if (Cam.mUp != 1) Cam.mUp = 0;
    }
}


void setupCallbacks(GLFWwindow* win)
{
    glfwSetWindowCloseCallback(win, windowCloseCallback);
    glfwSetWindowSizeCallback(win, windowSizeCallback);

    glfwSetCursorPosCallback(win, mouseCallback);
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
    std::cout << "GLFW " << glfwGetVersionString() << std::endl;
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

    Shaders = ShaderProgram {
        Shader::fromFile(Shader::Vertex, "Shaders/vert.glsl"),
        Shader::fromFile(Shader::Fragment, "Shaders/frag.glsl")
    };
    std::cerr << "Place 6" << std::endl;

    Cam = Camera({0, 0, 1}, {0, 0, 0}, {0, 1, 0}, 0.01, Shaders.Camera);

    Line  = TetrisObj(1, 1, {0.75, 0, 0}, Shape::Line);
    LLeft = TetrisObj(1, 1, {0.75, 0, 0.75}, Shape::LLeft);
    T1    = TetrisObj(1, 1, {0.75, 0.75, 0}, Shape::T);
    T2    = TetrisObj(1, 1, {0.0, 0.75, 0.75}, Shape::T);
    std::cerr << "Place 7" << std::endl;

    return win;
}

////////////////////////////////////////////////////////////////////////// RUN

void display(GLFWwindow* win, double elapsed_sec)
{
    drawScene();
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

        GLFWwindow* win = setup(gl_major, gl_minor, 640, 480, "Tetris 2D", fullscreen, vsync);
        run(win);
        exit(EXIT_SUCCESS);
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

/////////////////////////////////////////////////////////////////////////// END
