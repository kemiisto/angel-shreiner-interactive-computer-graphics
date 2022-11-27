#include "../main.h"
#include <tinygl/tinygl.h>
#include <glm/vec2.hpp>
#include <array>

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

    float theta = 0.0f;
    int thetaLoc = -1;

    bool direction = true;
    float speedFactor = 1.0f;
};

void Window::init()
{
    // Configure OpenGL.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Load shaders and initialize attribute buffers.
    program.addShaderFromSourceFile(tinygl::Shader::Type::Vertex, "rotatingSquare.vert");
    program.addShaderFromSourceFile(tinygl::Shader::Type::Fragment, "rotatingSquare.frag");
    program.link();
    program.use();

    std::array vertices = {
        glm::vec2{ 0.0f,  1.0f},
        glm::vec2{-1.0f,  0.0f},
        glm::vec2{ 1.0f,  0.0f},
        glm::vec2{ 0.0f, -1.0f}
    };

    // Load the data into the GPU
    vao.bind();
    vBuffer.bind();
    vBuffer.fill(vertices.begin(), vertices.end());

    // Associate shader variables with our data buffer
    auto positionLoc = program.attributeLocation("aPosition");
    vao.setAttributeArray(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(positionLoc);

    thetaLoc = program.uniformLocation("uTheta");

    setKeyCallback([this](tinygl::keyboard::Key key, int /*scancode*/, tinygl::input::Action action, tinygl::input::Modifier /*mods*/) {
        // Direction.
        if (key == tinygl::keyboard::Key::Right && action == tinygl::input::Action::Press) {
            direction = false;
        }
        if (key == tinygl::keyboard::Key::Left && action == tinygl::input::Action::Press) {
            direction = true;
        }
        // Speed.
        if (key == tinygl::keyboard::Key::W && action == tinygl::input::Action::Press) {
            speedFactor *= 1.5f;
        }
        if (key == tinygl::keyboard::Key::S && action == tinygl::input::Action::Press) {
            speedFactor /= 1.5f;
        }
    });
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

    theta += (direction ? 0.1f * speedFactor : -0.1f * speedFactor);
    program.setUniformValue(thetaLoc, theta);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

MAIN
