#include "../main.h"
#include <tinygl/tinygl.h>
#include <array>
#include <random>
#include <vector>

constexpr int numPositions = 5000;

class Window final : public tinygl::Window
{
public:
    using tinygl::Window::Window;
    void init() override;
    void processInput() override;
    void draw() override;
private:
    tinygl::ShaderProgram program;
    tinygl::Buffer vBuffer{tinygl::Buffer::Type::VertexBuffer, tinygl::Buffer::UsagePattern::StaticDraw};
    tinygl::VertexArrayObject vao;
};

void Window::init()
{
    auto positions = std::vector<tinygl::Vec3>{};
    positions.reserve(numPositions);

    // First, initialize the vertices of our 3D gasket.
    auto const vertices = std::array {
        tinygl::Vec3{-0.5f, -0.5f, -0.5f},
        tinygl::Vec3{ 0.5f, -0.5f, -0.5f},
        tinygl::Vec3{ 0.0f,  0.5f,  0.0f},
        tinygl::Vec3{ 0.0f, -0.5f,  0.5f}
    };

    positions.emplace_back(0.0f, 0.0f, 0.0f);

    auto device = std::random_device{};
    auto engine= std::mt19937{device()};
    auto distribution = std::uniform_int_distribution<int>{0, 3};

    // Compute new positions
    // Each new point is located midway between last point and a randomly chosen vertex
    for (int i = 0; i < numPositions; ++i) {
        auto const j = distribution(engine);
        positions.emplace_back(0.5f * (positions[i] + vertices[j]));
    }

    // Configure OpenGL
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);

    // Load shaders and initialize attribute buffers
    program.addShaderFromSourceFile(tinygl::Shader::Type::Vertex, "gasket3.vert");
    program.addShaderFromSourceFile(tinygl::Shader::Type::Fragment, "gasket3.frag");
    program.link();
    program.use();

    // Load the data into the GPU
    vao.bind();
    vBuffer.bind();
    vBuffer.create(positions.begin(), positions.end());

    // Associate shader variables with our data buffer
    auto const positionLoc = program.attributeLocation("aPosition");
    vao.setAttributeArray(positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(positionLoc);
}

void Window::processInput()
{
    if (getKey(tinygl::keyboard::Key::Escape) == tinygl::keyboard::KeyState::Press) {
        setShouldClose(true);
    }
}

void Window::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, numPositions);
}

MAIN
