#include "../main.h"
#include <tinygl/tinygl.h>
#include <glm/vec2.hpp>

constexpr int numTimesToSubdivide = 5;

std::vector<glm::vec2> positions;

void triangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
{
    positions.push_back(a);
    positions.push_back(b);
    positions.push_back(c);
}

void divideTriangle(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, int count)
{
    // check for end of recursion
    if (count == 0) {
        triangle(a, b, c);
    } else {
        //bisect the sides
        auto ab = 0.5f * (a + b);
        auto ac = 0.5f * (a + c);
        auto bc = 0.5f * (b + c);
        --count;
        // three new triangles
        divideTriangle(a, ab, ac, count);
        divideTriangle(c, ac, bc, count);
        divideTriangle(b, bc, ab, count);
    }
}

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
    // First, initialize the corners of our gasket with three positions.
    glm::vec2 vertices[] = {
        { -1.0f, -1.0f },
        {  0.0f,  1.0f },
        {  1.0f, -1.0f }
    };

    divideTriangle(vertices[0], vertices[1], vertices[2], numTimesToSubdivide);

    // Configure OpenGL
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Load shaders and initialize attribute buffers
    program.addShaderFromSourceFile(tinygl::Shader::Type::Vertex, "gasket2.vert");
    program.addShaderFromSourceFile(tinygl::Shader::Type::Fragment, "gasket2.frag");
    program.link();
    program.use();

    // Load the data into the GPU
    vao.bind();
    vBuffer.bind();
    vBuffer.create(positions.begin(), positions.end());

    // Associate shader variables with our data buffer
    auto positionLoc = program.attributeLocation("aPosition");
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
    glDrawArrays(GL_TRIANGLES, 0, positions.size());
}

MAIN
