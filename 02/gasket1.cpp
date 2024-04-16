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
    auto positions = std::vector<tinygl::Vec2>{};
    positions.reserve(numPositions);

    // First, initialize the corners of our gasket with three positions.
    auto const vertices = std::array {
        tinygl::Vec2{-1.0f, -1.0f},
        tinygl::Vec2{ 0.0f,  1.0f},
        tinygl::Vec2{ 1.0f, -1.0f}
    };

    auto device = std::random_device{};
    auto engine= std::mt19937{device()};
    auto distribution = std::uniform_int_distribution<int>{0, 2};

    // Specify a starting positions for our iterations - it must lie inside any set of three vertices
    auto const u = vertices[0] + vertices[1];
    auto const v = vertices[0] + vertices[2];
    positions.emplace_back(0.25f * (u + v));

    // Compute new positions
    // Each new point is located midway between last point and a randomly chosen vertex
    for (int i = 0; i < numPositions; ++i) {
        auto const j = distribution(engine);
        positions.emplace_back(0.5f * (positions[i] + vertices[j]));
    }

    // Configure OpenGL
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Load shaders and initialize attribute buffers
    program.addShaderFromSourceFile(tinygl::Shader::Type::Vertex, "gasket1.vert");
    program.addShaderFromSourceFile(tinygl::Shader::Type::Fragment, "gasket1.frag");
    program.link();
    program.use();

    // Load the data into the GPU
    vao.bind();
    vBuffer.bind();
    vBuffer.create(positions.begin(), positions.end());

    // Associate shader variables with our data buffer
    auto const positionLoc = program.attributeLocation("aPosition");
    vao.setAttributeArray(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
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
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, numPositions);
}

MAIN
