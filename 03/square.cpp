#include "../main.h"
#include <tinygl/tinygl.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr auto maxNumTriangles = 200;
constexpr auto maxNumPositions  = 3 * maxNumTriangles;

constexpr std::array colors = {
    glm::vec4{0.0, 0.0, 0.0, 1.0},  // black
    glm::vec4{1.0, 0.0, 0.0, 1.0},  // red
    glm::vec4{1.0, 1.0, 0.0, 1.0},  // yellow
    glm::vec4{0.0, 1.0, 0.0, 1.0},  // green
    glm::vec4{0.0, 0.0, 1.0, 1.0},  // blue
    glm::vec4{1.0, 0.0, 1.0, 1.0},  // magenta
    glm::vec4{0.0, 1.0, 1.0, 1.0}   // cyan
};

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
    tinygl::Buffer cBuffer{tinygl::Buffer::Type::VertexBuffer, tinygl::Buffer::UsagePattern::StaticDraw};
    tinygl::VertexArrayObject vao;
    int index = 0;
};

void Window::init()
{
    setMouseButtonCallback([this](
            tinygl::mouse::Button button,
            tinygl::input::Action action,
            tinygl::input::Modifier /* modifier */) {
        if (button == tinygl::mouse::Button::Left && action == tinygl::input::Action::Press) {
            vBuffer.bind();
            const auto [x, y] = getCursorPos();
            const auto [width, height] = getWindowSize();
            auto p = glm::vec2{2 * x / width - 1, 2 * (height - y) / height - 1};
            vBuffer.update(sizeof(p) * index, sizeof(p), glm::value_ptr(p));

            cBuffer.bind();
            auto c = colors.at(index%7);
            cBuffer.update(sizeof(c) * index, sizeof(c), glm::value_ptr(c));

            index++;
        }
    });
    // Configure OpenGL
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Load shaders and initialize attribute buffers
    program.addShaderFromSourceFile(tinygl::Shader::Type::Vertex, "square.vert");
    program.addShaderFromSourceFile(tinygl::Shader::Type::Fragment, "square.frag");
    program.link();
    program.use();

    // Load the data into the GPU
    vao.bind();

    vBuffer.bind();
    vBuffer.create(sizeof(glm::vec2) * maxNumPositions);

    auto positionLoc = program.attributeLocation("aPosition");
    vao.setAttributeArray(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(positionLoc);

    cBuffer.bind();
    cBuffer.create(sizeof(glm::vec4) * maxNumPositions);

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
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, index);
}

MAIN
