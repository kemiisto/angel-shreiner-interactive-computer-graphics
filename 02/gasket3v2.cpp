#include "../main.h"
#include <tinygl/tinygl.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <random>

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
    tinygl::Buffer vboPositions{tinygl::Buffer::Type::VertexBuffer, tinygl::Buffer::UsagePattern::StaticDraw};
    tinygl::Buffer vboColors{tinygl::Buffer::Type::VertexBuffer, tinygl::Buffer::UsagePattern::StaticDraw};
    tinygl::VertexArrayObject vao;
};

void Window::init()
{
    std::vector<glm::vec3> positions;
    positions.reserve(numPositions);

    std::vector<glm::vec4> colors;
    colors.reserve(numPositions);

    // First, initialize the vertices of our 3D gasket.
    glm::vec3 vertices[] = {
        {-0.5f, -0.5f, -0.5f},
        { 0.5f, -0.5f, -0.5f},
        { 0.0f,  0.5f,  0.0f},
        { 0.0f, -0.5f,  0.5f}
    };

    positions.emplace_back(0.0f, 0.0f, 0.0f);
    colors.emplace_back(0.5f, 0.5f, 0.5f, 1.0f);

    std::random_device device;
    std::mt19937 engine(device());
    std::uniform_int_distribution<int> distribution(0, 3);

    // Compute new positions
    // Each new point is located midway between last point and a randomly chosen vertex
    for (int i = 0; i < numPositions; ++i) {
        auto j = distribution(engine);
        auto newPosition = 0.5f * (positions[i] + vertices[j]);
        positions.push_back(newPosition);
        colors.emplace_back(
            (1.0f + newPosition[0]) / 2.0f,
            (1.0f + newPosition[1]) / 2.0f,
            (1.0f + newPosition[2]) / 2.0f,
            1.0f
        );
    }

    // Configure OpenGL
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);

    // Load shaders and initialize attribute buffers
    program.addShaderFromSourceFile(tinygl::Shader::Type::Vertex, "gasket3v2.vert");
    program.addShaderFromSourceFile(tinygl::Shader::Type::Fragment, "gasket3.frag");
    program.link();
    program.use();

    // Load the data into the GPU
    vao.bind();

    vboPositions.bind();
    vboPositions.fill(positions.begin(), positions.end());

    auto vertexPositionLoc = program.attributeLocation("aPosition");
    vao.setAttributeArray(vertexPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(vertexPositionLoc);

    vboColors.bind();
    vboColors.fill(colors.begin(), colors.end());

    auto colorLoc = program.attributeLocation("aColor");
    vao.setAttributeArray(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(colorLoc);
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
