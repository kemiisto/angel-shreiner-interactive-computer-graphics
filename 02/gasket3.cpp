#include "../main.h"
#include <tinygl/tinygl.h>
#include <glm/vec3.hpp>
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
    tinygl::Buffer vbo{tinygl::Buffer::Type::VertexBuffer, tinygl::Buffer::UsagePattern::StaticDraw};
    tinygl::VertexArrayObject vao;
};

void Window::init()
{
    std::vector<glm::vec3> positions;
    positions.reserve(numPositions);

    // First, initialize the vertices of our 3D gasket.
    glm::vec3 vertices[] = {
        { -0.5f, -0.5f, -0.5f },
        {  0.5f, -0.5f, -0.5f },
        {  0.0f,  0.5f,  0.0f },
        {  0.0f, -0.5f,  0.5f }
    };

    positions.emplace_back(0.0f, 0.0f, 0.0f);

    std::random_device device;
    std::mt19937 engine(device());
    std::uniform_int_distribution<int> distribution(0, 3);

    // Compute new positions
    // Each new point is located midway between last point and a randomly chosen vertex
    for (int i = 0; i < numPositions; ++i) {
        auto j = distribution(engine);
        positions.emplace_back(0.5f * (positions[i] + vertices[j]));
    }

    // Configure OpenGL
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Load shaders and initialize attribute buffers
    program.addShaderFromSourceFile(tinygl::Shader::Type::Vertex, "vshader23.glsl");
    program.addShaderFromSourceFile(tinygl::Shader::Type::Fragment, "fshader23.glsl");
    program.link();
    program.use();

    // Load the data into the GPU
    vao.bind();
    vbo.bind();
    vbo.fill(positions.begin(), positions.end());

    // Associate shader variables with our data buffer
    auto positionLoc = program.attributeLocation("aPosition");
    vao.setAttributeArray(positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(positionLoc);
}

void Window::processInput()
{
    if (getKey(tinygl::Key::Escape) == tinygl::KeyState::Press) {
        setShouldClose(true);
    }
}

void Window::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, numPositions);
}

MAIN
