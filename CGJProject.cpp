#include <array>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Utils.h"
#include "Engine/Engine.h"
#include "Engine/GlInit.h"
#include "Render/Shader.h"

constexpr auto Offset = -3.f;

Ptr<render::Filter> Inverted;

namespace config::hooks
{
    #pragma region Callbacks

    void onWindowClose([[maybe_unused]] engine::Engine& engine) {}

    void onWindowResize([[maybe_unused]] engine::Engine& engine, callback::WindowSize const size)
    {
        engine.resize(size);
    }

    void onMouseButton(engine::Engine& engine, callback::MouseButton const button)
    {
        if (button.button != GLFW_MOUSE_BUTTON_1) return;

        if (button.action == GLFW_PRESS)
        {
            double x_pos, y_pos;
            glfwGetCursorPos(engine.window(), &x_pos, &y_pos);
            engine.scene().controller().startDrag({x_pos, y_pos});

            #if _DEBUG
            std::cerr << "start pos: " << x_pos << ", " << y_pos << std::endl;
            #endif
        }
        else if (button.action == GLFW_RELEASE)
        {
            engine.scene().controller().finishDrag();
        }
    }

    void onMouseMove(engine::Engine& engine, callback::MousePosition const position)
    {
        auto const button_state = glfwGetMouseButton(engine.window(), GLFW_MOUSE_BUTTON_1);
        if (button_state == GLFW_PRESS)
        {
            #if _DEBUG
            std::cerr << "position: " << position.x << ", " << position.y << std::endl;
            #endif

            engine.scene().controller().rotateDrag(position);
        }
    }

    void onMouseScroll([[maybe_unused]] engine::Engine& engine, double const scroll)
    {
        #if _DEBUG
        std::cerr << "zoom: " << scroll << std::endl;
        #endif

        engine.scene().controller().scroll(scroll);
    }

    void onKeyboardButton(engine::Engine& engine, callback::KeyboardButton const button)
    {
        if (button.action == GLFW_PRESS)
            switch (button.key)
            {
            case GLFW_KEY_ESCAPE:
                engine.terminate();
                break;
            case GLFW_KEY_P:
                engine.scene().controller().camera().swapProjection();
                break;
            case GLFW_KEY_G:
                engine.scene().controller().camera().swapRotationMode();
                break;
            case GLFW_KEY_M:
                engine.scene().animate();
                break;
            case GLFW_KEY_W:
                engine.scene().root().transform.position.z -= 0.5;
                break;
            case GLFW_KEY_A:
                engine.scene().root().transform.position.x -= 0.5;
                break;
            case GLFW_KEY_S:
                engine.scene().root().transform.position.z += 0.5;
                break;
            case GLFW_KEY_D:
                engine.scene().root().transform.position.x += 0.5;
                break;
            case GLFW_KEY_T:
                engine.snapshot();
                break;
            case GLFW_KEY_1:
                engine.scene().active_filter = nullptr;
                break;
            case GLFW_KEY_2:
                engine.scene().active_filter = Inverted;
                break;
            }
    }

    #pragma endregion Callbacks

    #pragma region OpenGl

    void setupOpenGl(Settings const& settings)
    {
        auto const [width, height] = settings.window.size;
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);
        glDepthRange(0.0, 1.0);
        glClearDepth(1.0);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glViewport(0, 0, width, height);
    }

    #pragma endregion OpenGl

    #pragma region Scene

    void setupScene(render::Scene::Builder& builder, Settings const& settings)
    {
        using namespace std::filesystem;
        using namespace render;

        constexpr auto CubeMargin = 0.9f;
        auto const     Scale      = Vector3::filled(CubeMargin);

        path const shaders = "Shaders/";
        path const assets  = "Assets/";

        Ptr<Pipeline const> bp_pipeline, cel_pipeline, invert_pipeline;
        Ptr<Mesh const>     plane_mesh,  piece_mesh;

        logTimeTaken(
            "Loading Assets",
            [&]()
            {
                bp_pipeline = &builder.shaders.emplace_back(
                    Shader::fromFile(Shader::Vertex, shaders / "bp_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, shaders / "bp_frag.glsl")
                );

                cel_pipeline = &builder.shaders.emplace_back(
                    Shader::fromFile(Shader::Vertex, shaders / "cel_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, shaders / "cel_frag.glsl")
                );

                invert_pipeline = &builder.shaders.emplace_back(
                    Shader::fromFile(Shader::Vertex, shaders / "Filters" / "invert_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, shaders / "Filters" / "invert_frag.glsl")
                );

                plane_mesh = &builder.meshes.emplace_back(MeshLoader::fromFile(assets / "Plane.obj"));
                &builder.meshes.emplace_back(MeshLoader::fromFile(assets / "Cube.obj"));
                piece_mesh = &builder.meshes.emplace_back(MeshLoader::fromFile(assets / "Sphere16.obj"));
            }
        );

        logTimeTaken(
            "Creating Filters",
            [&]()
            {
                Inverted = &builder.filters.emplace_back(invert_pipeline, settings.window);
            }
        );

        logTimeTaken(
            "Setting up scene",
            [&]()
            {
                builder.camera = Camera(20, Vector3::filled(0), Pipeline::Camera);

                builder.root = std::make_unique<Object>(plane_mesh, Vector4 {0.8, 0.8, 0.8, 1}, bp_pipeline);
                auto& plane  = *builder.root.get();

                auto& figure = plane.children.emplace_back(cel_pipeline);
                // auto& figure     = plane.children.emplace_back();
                figure.transform = {{0.5, 0.5, 0}};

                auto& l     = figure.children.emplace_back();
                l.transform = {{1, 0, 0}, Quaternion::fromAngleAxis(Pi, Axis::Y)};
                l.animation = {
                    l.transform, {{1, 1, 1}, Quaternion::fromAngleAxis(2, {0.7, 0.2, 0.7})}
                };
                {
                    auto const color = Vector4 {0.7, 0, 0, 1};

                    auto& c1 = l.children.emplace_back(piece_mesh, color);
                    auto& c2 = l.children.emplace_back(piece_mesh, color);
                    auto& c3 = l.children.emplace_back(piece_mesh, color);
                    auto& c4 = l.children.emplace_back(piece_mesh, color);

                    c1.transform.position = {0, 0, 0};
                    c2.transform.position = {1, 0, 0};
                    c3.transform.position = {0, 1, 0};
                    c4.transform.position = {0, 2, 0};

                    for (auto element : {&c1, &c2, &c3, &c4})
                        element->transform.scaling = Scale;
                }

                auto& t1     = figure.children.emplace_back();
                t1.transform = {{-1, 1, 0}, Quaternion::fromAngleAxis(Pi * 3 / 2, Axis::Z)};
                t1.animation = {
                    t1.transform, {{0, 0, -2}, Quaternion::fromAngleAxis(-Pi * 2, Axis::Z)}
                };

                auto& t2     = figure.children.emplace_back();
                t2.transform = {{0, 3, 0}, Quaternion::fromAngleAxis(Pi, Axis::Z)};
                t2.animation = {
                    t2.transform, {{3, 4, 0}, Quaternion::fromAngleAxis(Pi / 2, Axis::X)}
                };
                {
                    constexpr auto color1 = Vector4 {0, 0.7, 0, 1};
                    constexpr auto color2 = Vector4 {0, 0, 0.7, 1};

                    for (auto [t, color] : std::array {std::pair {&t1, color1}, std::pair {&t2, color2}})
                    {
                        auto& c1 = t->children.emplace_back(piece_mesh, color);
                        auto& c2 = t->children.emplace_back(piece_mesh, color);
                        auto& c3 = t->children.emplace_back(piece_mesh, color);
                        auto& c4 = t->children.emplace_back(piece_mesh, color);

                        c1.transform.position = {0, 0, 0};
                        c2.transform.position = {-1, 0, 0};
                        c3.transform.position = {1, 0, 0};
                        c4.transform.position = {0, 1, 0};

                        for (auto element : {&c1, &c2, &c3, &c4})
                            element->transform.scaling = Scale;
                    }
                }

                auto& line     = figure.children.emplace_back();
                line.transform = {{-2, 0, 0}};
                line.animation = {
                    line.transform, {{0, 0, 0}, Quaternion::fromAngleAxis(Pi / 4, Axis::Y)}
                };
                {
                    constexpr auto color = Vector4 {0.7, 0, 0.7, 1};

                    auto& c1 = line.children.emplace_back(piece_mesh, color);
                    auto& c2 = line.children.emplace_back(piece_mesh, color);
                    auto& c3 = line.children.emplace_back(piece_mesh, color);
                    auto& c4 = line.children.emplace_back(piece_mesh, color);

                    c1.transform.position = {0, 0, 0};
                    c2.transform.position = {0, 1, 0};
                    c3.transform.position = {0, 2, 0};
                    c4.transform.position = {0, 3, 0};

                    for (auto element : {&c1, &c2, &c3, &c4})
                        element->transform.scaling = Scale;
                }
            }
        );
    }

    void beforeRender(render::Scene& scene, engine::Engine& engine, double const elapsed_sec)
    {
        if (scene.active_filter != nullptr)
            scene.active_filter->bind();
    }

    void afterRender(render::Scene& scene, engine::Engine& engine, double const elapsed_sec)
    {
        if (scene.active_filter != nullptr)
            scene.active_filter->finish();
    }

    #pragma endregion Scene
}

int main()
{
    using namespace config;

    auto const settings = Settings {
        Version {4, 3},
        Window {
            u8"Tetris 3D",
            WindowSize {640, 480},
            Mode::Windowed,
            VSync::On
        },
        Snapshot {
            "./CG_Snaps"
        }
    };

    try
    {
        auto engine = logTimeTaken(
            "Initialization",
            [&]()
            {
                auto glfw = engine::GlfwHandle {settings};
                auto glew = engine::GlInit {settings};

                auto scene = render::Scene::setup(std::move(glew), settings);
                return engine::Engine::init(std::move(glfw), std::move(scene), settings);
            }
        );
        engine->run();
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
