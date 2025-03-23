#include "../main.h"
#include <tinygl/tinygl.h>
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
        tinyla::vec2f{ 0.0f,  1.0f},
        tinyla::vec2f{-1.0f,  0.0f},
        tinyla::vec2f{ 1.0f,  0.0f},
        tinyla::vec2f{ 0.0f, -1.0f}
    };

    // Load the data into the GPU
    vao.bind();
    vBuffer.bind();
    vBuffer.create(vertices.begin(), vertices.end());

    // Associate shader variables with our data buffer
    auto positionLoc = program.attributeLocation("aPosition");
    vao.setAttributeArray(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(positionLoc);

    thetaLoc = program.uniformLocation("uTheta");
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

    theta += 0.1f;
    program.setUniformValue(thetaLoc, theta);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

MAIN
