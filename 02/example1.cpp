#include "../main.h"
#include <tinygl/tinygl.h>
#include <glm/vec2.hpp>
#include <random>

constexpr int numPoints = 5000;

class Window final : public tinygl::Window
{
public:
    using tinygl::Window::Window;
    void init() override;
    void processInput() override;
    void draw() override;
private:
    tinygl::ShaderProgram program;
    tinygl::Buffer vbo{tinygl::Buffer::Type::VertexBuffer, tinygl::Buffer::UsagePattern::StaticDraw};
    tinygl::VertexArrayObject vao;
};

void Window::init()
{
    glm::vec2 positions[numPoints];

    // First, initialize the corners of our gasket with three positions.
    glm::vec2 vertices[] = {
        { -1.0f, -1.0f },
        {  0.0f,  1.0f },
        {  1.0f, -1.0f }
    };

    std::random_device device;
    std::mt19937 engine(device());
    std::uniform_int_distribution<int> distribution(0, 2);

    // Specify a starting positions for our iterations - it must lie inside any set of three vertices
    auto u = vertices[0] + vertices[1];
    auto v = vertices[0] + vertices[2];
    positions[0] = 0.25f * (u + v);

    // Compute new positions
    // Each new point is located midway between last point and a randomly chosen vertex
    for (int i = 0; i < numPoints - 1; ++i) {
        auto j = distribution(engine);
        positions[i + 1] = 0.5f * (positions[i] + vertices[j]);
    }

    // Configure OpenGL
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Load shaders and initialize attribute buffers
    program.addShaderFromSourceFile(tinygl::Shader::Type::Vertex, "vshader21.glsl");
    program.addShaderFromSourceFile(tinygl::Shader::Type::Fragment, "fshader21.glsl");
    program.link();
    program.use();

    // Load the data into the GPU
    vao.bind();
    vbo.bind();
    vbo.fill(positions, sizeof(positions));

    // Associate shader variables with our data buffer
    auto vertexPositionLoc = program.attributeLocation("vertexPosition");
    vao.setAttributeArray(vertexPositionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(vertexPositionLoc);
}

void Window::processInput()
{
    if (getKey(tinygl::Key::Escape) == tinygl::KeyState::Press) {
        setShouldClose(true);
    }
}

void Window::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, numPoints);
}

MAIN
