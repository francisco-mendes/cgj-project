#include <array>
#include <iostream>
#include <ostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Utils.h"
#include "Engine/Engine.h"
#include "Engine/GlInit.h"
#include "Render/Object.h"
#include "Render/Shader.h"

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
            engine.scene.camera_controller.startDrag({x_pos, y_pos});

            #if _DEBUG
            std::cerr << "start pos: " << x_pos << ", " << y_pos << std::endl;
            #endif
        }
        else if (button.action == GLFW_RELEASE)
        {
            engine.scene.camera_controller.finishDrag();
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

            engine.scene.camera_controller.rotateDrag(position);
        }
    }

    void onMouseScroll([[maybe_unused]] engine::Engine& engine, double const scroll)
    {
        #if _DEBUG
        std::cerr << "zoom: " << scroll << std::endl;
        #endif

        engine.scene.camera_controller.scroll(scroll);
    }

    void onKeyboardButton(engine::Engine& engine, callback::KeyboardButton const button)
    {
        if (button.action == GLFW_PRESS)
            switch (button.key)
            {
            case GLFW_KEY_ESCAPE:
                engine.terminate();
                break;

            case GLFW_KEY_F1:
                engine.snapshot();
                break;
            case GLFW_KEY_F11:
                engine.scene.animate();
                break;
            case GLFW_KEY_F12:
                engine.scene.camera_controller.camera.swapRotationMode();
                break;

            case GLFW_KEY_W:
                if (auto const obj = engine.object_controller.get(); obj)
                    obj->transform.position.z -= 0.5;
                break;
            case GLFW_KEY_S:
                if (auto const obj = engine.object_controller.get(); obj)
                    obj->transform.position.z += 0.5;
                break;
            case GLFW_KEY_A:
                if (auto const obj = engine.object_controller.get(); obj)
                    obj->transform.position.x -= 0.5;
                break;
            case GLFW_KEY_D:
                if (auto const obj = engine.object_controller.get(); obj)
                    obj->transform.position.x += 0.5;
                break;
            case GLFW_KEY_Q:
                if (auto const obj = engine.object_controller.get(); obj)
                    obj->transform.position.y -= 0.5;
                break;
            case GLFW_KEY_E:
                if (auto const obj = engine.object_controller.get(); obj)
                    obj->transform.position.y += 0.5;
                break;

            case GLFW_KEY_R:
                engine.object_controller.recurse();
                engine.resetControllers();
                break;
            case GLFW_KEY_T:
                engine.setControllers(engine.object_controller.parent());
                break;
            case GLFW_KEY_F:
                engine.setControllers(engine.object_controller.prev());
                break;
            case GLFW_KEY_G:
                engine.setControllers(engine.object_controller.next());
                break;
            case GLFW_KEY_X:
                engine.object_controller.remove();
                engine.resetControllers();
                break;
            case GLFW_KEY_C:
                engine.object_controller.create();
                engine.resetControllers();
                break;

            case GLFW_KEY_I:
                engine.filter_controller.prev();
                break;
            case GLFW_KEY_O:
                engine.filter_controller.next();
                break;

            case GLFW_KEY_Y:
                if (auto const obj = engine.object_controller.get(); obj != nullptr)
                    engine.object_controller.get()->shaders = engine.pipeline_controller.prev();
                break;
            case GLFW_KEY_U:
                if (auto const obj = engine.object_controller.get(); obj != nullptr)
                    engine.object_controller.get()->shaders = engine.pipeline_controller.next();
                break;

            case GLFW_KEY_H:
                if (auto const obj = engine.object_controller.get(); obj != nullptr)
                    engine.object_controller.get()->texture = engine.texture_controller.prev();
                break;
            case GLFW_KEY_J:
                if (auto const obj = engine.object_controller.get(); obj != nullptr)
                    engine.object_controller.get()->texture = engine.texture_controller.next();
                break;

            case GLFW_KEY_N:
                if (auto const obj = engine.object_controller.get(); obj != nullptr)
                    engine.object_controller.get()->mesh = engine.mesh_controller.prev();
                break;
            case GLFW_KEY_M:
                if (auto const obj = engine.object_controller.get(); obj != nullptr)
                    engine.object_controller.get()->mesh = engine.mesh_controller.next();
                break;
            }
    }

    #pragma endregion Callbacks

    #pragma region OpenGl

    void setupOpenGl(Settings const& settings)
    {
        auto const [width, height] = settings.window.size;
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

        auto const& [meshes, textures, shaders, filters, _] = settings.paths;

        Ptr<Pipeline const> bp_pipeline, cel_pipeline, invert_pipeline, grayscale_pipeline, sharpen_pipeline,
                            blur_pipeline, edge_pipeline, sobel_pipeline, sepia_pipeline, red_pipeline, green_pipeline,
                            blue_pipeline, high_saturation_pipeline, emboss_pipeline, sketch_pipeline,
                            oil_painting_pipeline, swirl_pipeline;
        Ptr<Mesh const> plane_mesh, piece_mesh;
        logTimeTaken(
            "Loading Assets",
            [&]()
            {
                plane_mesh = &builder.meshes.emplace_back(MeshLoader::fromFile(meshes / "Plane.obj"));
                &builder.meshes.emplace_back(MeshLoader::fromFile(meshes / "Cube.obj"));
                piece_mesh = &builder.meshes.emplace_back(MeshLoader::fromFile(meshes / "Sphere16.obj"));

                &builder.textures.emplace_back(TextureLoader::fromFile(textures / "awesomeface.png"));
            }
        );
        logTimeTaken(
            "Loading Shaders",
            [&]()
            {
                bp_pipeline = &builder.shaders.emplace_back(
                    false,
                    Shader::fromFile(Shader::Vertex, shaders / "bp_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, shaders / "bp_frag.glsl")
                );
                cel_pipeline = &builder.shaders.emplace_back(
                    false,
                    Shader::fromFile(Shader::Vertex, shaders / "cel_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, shaders / "cel_frag.glsl")
                );
                invert_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "invert_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "invert_frag.glsl")
                );
                grayscale_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "grayscale_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "grayscale_frag.glsl")
                );
                sharpen_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "sharpen_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "sharpen_frag.glsl")
                );
                blur_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "blur_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "blur_frag.glsl")
                );
                edge_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "edge_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "edge_frag.glsl")
                );
                sobel_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "sobel_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "sobel_frag.glsl")
                );
                sepia_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "sepia_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "sepia_frag.glsl")
                );
                red_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "red_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "red_frag.glsl")
                );
                green_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "green_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "green_frag.glsl")
                );
                blue_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "blue_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "blue_frag.glsl")
                );
                high_saturation_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "highSaturation_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "highSaturation_frag.glsl")
                );
                emboss_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "emboss_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "emboss_frag.glsl")
                );
                sketch_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "sketch_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "sketch_frag.glsl")
                );
                oil_painting_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "oilPainting_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "oilPainting_frag.glsl")
                );
                swirl_pipeline = &builder.shaders.emplace_back(
                    true,
                    Shader::fromFile(Shader::Vertex, filters / "swirl_vert.glsl"),
                    Shader::fromFile(Shader::Fragment, filters / "swirl_frag.glsl")
                );
            }
        );

        logTimeTaken(
            "Creating Filters",
            [&]()
            {
                builder.filters.emplace_back(settings.window, invert_pipeline);
                builder.filters.emplace_back(settings.window, grayscale_pipeline);
                builder.filters.emplace_back(settings.window, sharpen_pipeline);
                builder.filters.emplace_back(settings.window, blur_pipeline);
                builder.filters.emplace_back(settings.window, edge_pipeline);
                builder.filters.emplace_back(settings.window, sobel_pipeline);
                builder.filters.emplace_back(settings.window, sepia_pipeline);
                builder.filters.emplace_back(settings.window, red_pipeline);
                builder.filters.emplace_back(settings.window, green_pipeline);
                builder.filters.emplace_back(settings.window, blue_pipeline);
                builder.filters.emplace_back(settings.window, high_saturation_pipeline);
                builder.filters.emplace_back(settings.window, emboss_pipeline);
                builder.filters.emplace_back(settings.window, sketch_pipeline);
                builder.filters.emplace_back(settings.window, oil_painting_pipeline);
                builder.filters.emplace_back(settings.window, swirl_pipeline);
            }
        );

        logTimeTaken(
            "Setting up scene",
            [&]()
            {
                builder.light_position = Vector3 {4, 4, 4};
                builder.camera         = Camera(20, Vector3::filled(0), Pipeline::Camera);
                builder.default_shader = bp_pipeline;

                auto& plane = builder.root->emplaceChild(plane_mesh, Vector4 {0.8, 0.8, 0.8, 1});

                auto& figure     = plane.emplaceChild(cel_pipeline);
                figure.transform = {{0.5, 0.5, 0}};

                auto& l     = figure.emplaceChild();
                l.transform = {{1, 0, 0}, Quaternion::fromAngleAxis(Pi, Axis::Y)};
                l.animation = {
                    l.transform, {{1, 1, 1}, Quaternion::fromAngleAxis(2, {0.7, 0.2, 0.7})}
                };
                {
                    auto const color = Vector4 {0.7, 0, 0, 1};

                    auto& c1 = l.emplaceChild(piece_mesh, color);
                    auto& c2 = l.emplaceChild(piece_mesh, color);
                    auto& c3 = l.emplaceChild(piece_mesh, color);
                    auto& c4 = l.emplaceChild(piece_mesh, color);

                    c1.transform.position = {0, 0, 0};
                    c2.transform.position = {1, 0, 0};
                    c3.transform.position = {0, 1, 0};
                    c4.transform.position = {0, 2, 0};

                    for (auto element : {&c1, &c2, &c3, &c4})
                        element->transform.scaling = Scale;
                }

                auto& t1     = figure.emplaceChild();
                t1.transform = {{-1, 1, 0}, Quaternion::fromAngleAxis(Pi * 3 / 2, Axis::Z)};
                t1.animation = {
                    t1.transform, {{0, 0, -2}, Quaternion::fromAngleAxis(-Pi * 2, Axis::Z)}
                };

                auto& t2     = figure.emplaceChild();
                t2.transform = {{0, 3, 0}, Quaternion::fromAngleAxis(Pi, Axis::Z)};
                t2.animation = {
                    t2.transform, {{3, 4, 0}, Quaternion::fromAngleAxis(Pi / 2, Axis::X)}
                };
                {
                    constexpr auto color1 = Vector4 {0, 0.7, 0, 1};
                    constexpr auto color2 = Vector4 {0, 0, 0.7, 1};

                    for (auto [t, color] : std::array {std::pair {&t1, color1}, std::pair {&t2, color2}})
                    {
                        auto& c1 = t->emplaceChild(piece_mesh, color);
                        auto& c2 = t->emplaceChild(piece_mesh, color);
                        auto& c3 = t->emplaceChild(piece_mesh, color);
                        auto& c4 = t->emplaceChild(piece_mesh, color);

                        c1.transform.position = {0, 0, 0};
                        c2.transform.position = {-1, 0, 0};
                        c3.transform.position = {1, 0, 0};
                        c4.transform.position = {0, 1, 0};

                        for (auto element : {&c1, &c2, &c3, &c4})
                            element->transform.scaling = Scale;
                    }
                }

                auto& line     = figure.emplaceChild();
                line.transform = {{-2, 0, 0}};
                line.animation = {
                    line.transform, {{0, 0, 0}, Quaternion::fromAngleAxis(Pi / 4, Axis::Y)}
                };
                {
                    constexpr auto color = Vector4 {0.7, 0, 0.7, 1};

                    auto& c1 = line.emplaceChild(piece_mesh, color);
                    auto& c2 = line.emplaceChild(piece_mesh, color);
                    auto& c3 = line.emplaceChild(piece_mesh, color);
                    auto& c4 = line.emplaceChild(piece_mesh, color);

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
        if (engine.filter_controller.get() != nullptr)
            engine.filter_controller.get()->bind();
    }

    void afterRender(render::Scene& scene, engine::Engine& engine, double const elapsed_sec)
    {
        if (engine.filter_controller.get() != nullptr)
            engine.filter_controller.get()->finish();
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
        Paths {
            "Assets/Meshes",
            "Assets/Textures",
            "Assets/Shaders",
            "Assets/Shaders/Filters",
            "Snapshots"
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
