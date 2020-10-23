#define GLEW_STATIC

#include <array>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Matrix.h"
#include "Vector.h"

constexpr float PI = 3.1415927;

constexpr auto VERTICES = 0;
constexpr auto COLORS   = 1;

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
    GLuint vao, vbo[2];

    TetrisObj() = default;
    TetrisObj(float size_x, float size_y, Vector4 color, Shape shape);

    void                   draw(Matrix4 const& transformation) const;
    std::array<Vector2, 4> shapeArray() const;
    void                   cleanup();
};

TetrisObj Line, LLeft, T1, T2;

GLuint VertexShaderId, FragmentShaderId, ProgramId;
GLint  UniformId;

////////////////////////////////////////////////// ERROR CALLBACK (OpenGL 4.3+)

static std::string_view errorSource(GLenum source)
{
    switch (source)
    {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "window system";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "shader compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "third party";
        case GL_DEBUG_SOURCE_APPLICATION: return "application";
        case GL_DEBUG_SOURCE_OTHER: return "other";
        default: exit(EXIT_FAILURE);
    }
}

static std::string_view errorType(GLenum type)
{
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR: return "error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "deprecated behavior";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "undefined behavior";
        case GL_DEBUG_TYPE_PORTABILITY: return "portability issue";
        case GL_DEBUG_TYPE_PERFORMANCE: return "performance issue";
        case GL_DEBUG_TYPE_MARKER: return "stream annotation";
        case GL_DEBUG_TYPE_PUSH_GROUP: return "push group";
        case GL_DEBUG_TYPE_POP_GROUP: return "pop group";
        case GL_DEBUG_TYPE_OTHER_ARB: return "other";
        default: exit(EXIT_FAILURE);
    }
}

static std::string_view errorSeverity(GLenum severity)
{
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH: return "high";
        case GL_DEBUG_SEVERITY_MEDIUM: return "medium";
        case GL_DEBUG_SEVERITY_LOW: return "low";
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "notification";
        default: exit(EXIT_FAILURE);
    }
}

static void error(GLenum source, GLenum type, GLuint, GLenum severity, GLsizei, const GLchar* message, void const*)
{
    std::cerr << "GL ERROR:" << std::endl;
    std::cerr << "  source:     " << errorSource(source) << std::endl;
    std::cerr << "  type:       " << errorType(type) << std::endl;
    std::cerr << "  severity:   " << errorSeverity(severity) << std::endl;
    std::cerr << "  debug call: " << std::endl << message << std::endl;
    std::cerr << "Press <return>.";
    std::cin.ignore();
}

void setupErrorCallback()
{
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(error, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    // params: source, type, severity, count, ids, enabled
}

/////////////////////////////////////////////////////////////////////// SHADERs

const GLchar* VertexShader =
    R"(
#version 330 core

in vec4 in_Position;
in vec4 in_Color;
out vec4 ex_Color;

uniform mat4 Matrix;

void main(void)
{
	gl_Position = Matrix * in_Position;
	ex_Color = in_Color;
}
    )";

const GLchar* FragmentShader =
    R"(
#version 330 core

in vec4 ex_Color;
out vec4 out_Color;

void main(void)
{
    out_Color = ex_Color;
}
    )";

void createShaderProgram()
{
    VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShaderId, 1, &VertexShader, nullptr);
    glCompileShader(VertexShaderId);

    FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShaderId, 1, &FragmentShader, nullptr);
    glCompileShader(FragmentShaderId);

    ProgramId = glCreateProgram();
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);

    glBindAttribLocation(ProgramId, VERTICES, "in_Position");
    glBindAttribLocation(ProgramId, COLORS, "in_Color");

    glLinkProgram(ProgramId);
    UniformId = glGetUniformLocation(ProgramId, "Matrix");

    glDetachShader(ProgramId, VertexShaderId);
    glDeleteShader(VertexShaderId);
    glDetachShader(ProgramId, FragmentShaderId);
    glDeleteShader(FragmentShaderId);
}

void destroyShaderProgram()
{
    glUseProgram(0);
    glDeleteProgram(ProgramId);
}

/////////////////////////////////////////////////////////////////////// VAOs & VBOs

TetrisObj::TetrisObj(float size_x, float size_y, Vector4 color, Shape s)
    : shape {s}
{
    Vector4 vertex_positions[4] {
        {-0.5f * size_x, -0.5f * size_y, 0, 1},
        {0.5f * size_x, -0.5f * size_y, 0, 1},
        {0.5f * size_x, 0.5f * size_y, 0, 1},
        {-0.5f * size_x, 0.5f * size_y, 0, 1}
    };
    Vector4 vertex_colors[4] {color, color, color, color};

    GLubyte index[2 * 3] {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    {
        glGenBuffers(2, vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions) + sizeof(vertex_colors), nullptr, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_positions), vertex_positions);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vertex_colors), vertex_colors);


            glEnableVertexAttribArray(VERTICES);
            glVertexAttribPointer(VERTICES, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(COLORS);
            glVertexAttribPointer(COLORS, 4, GL_FLOAT, GL_FALSE, 0, (void*) sizeof(vertex_positions));
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
        }
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TetrisObj::cleanup()
{
    glBindVertexArray(vao);
    glDisableVertexAttribArray(VERTICES);
    glDisableVertexAttribArray(COLORS);
    glDeleteBuffers(2, vbo);
    glDeleteVertexArrays(1, &vao);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void TetrisObj::draw(Matrix4 const& transformation) const
{
    static auto const SqMargin = Matrix4::scaling(Vector3::filled(0.9f));

    glBindVertexArray(vao);
    glUseProgram(ProgramId);

    for (const auto& sqr : shapeArray())
    {
        auto matrix = transformation * Matrix4::translation(sqr) * SqMargin;
        glUniformMatrix4fv(UniformId, 1, GL_TRUE, matrix.inner);
        glDrawElements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_BYTE, nullptr);
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

    Line.draw(scale * rotation * Matrix4::translation({-1.5, -1.5, 0}));
    LLeft.draw(scale * rotation * Matrix4::translation({0.5, -1.5, 0}));
    T1.draw(scale * Matrix4::rotation(Axis::Z, angle + PI / 2) * Matrix4::translation({-1.5, -0.5, 0}));
    T2.draw(scale * rotation * Matrix4::translation({-0.5, 0.5, 0}));

    glUseProgram(0);
    glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////// CALLBACKS

void windowCloseCallback(GLFWwindow* win)
{
    destroyShaderProgram();

    TetrisObj objs[] {Line, LLeft, T1, T2};
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
    glfwSwapInterval(is_vsync);
    return win;
}

void setupCallbacks(GLFWwindow* win)
{
    glfwSetWindowCloseCallback(win, windowCloseCallback);
    glfwSetWindowSizeCallback(win, windowSizeCallback);
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
    createShaderProgram();

    Line  = TetrisObj(1, 1, {0.75, 0, 0}, Shape::Line);
    LLeft = TetrisObj(1, 1, {0.75, 0, 0.75}, Shape::LLeft);
    T1    = TetrisObj(1, 1, {0.75, 0.75, 0}, Shape::T);
    T2    = TetrisObj(1, 1, {0.0, 0.75, 0.75}, Shape::T);
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
    int const  gl_major   = 4, gl_minor = 3;
    bool const fullscreen = false;
    bool const vsync      = false;

    GLFWwindow* win = setup(gl_major, gl_minor, 640, 480, "Tetris 2D", fullscreen, vsync);
    run(win);
    exit(EXIT_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////// END
