#include "../main.h"
#include <tinygl/tinygl.h>

constexpr auto maxNumTriangles = 200;
constexpr auto maxNumPositions  = 3 * maxNumTriangles;

constexpr std::array colors = {
    tinygl::Vec4{0.0f, 0.0f, 0.0f, 1.0f},  // black
    tinygl::Vec4{1.0f, 0.0f, 0.0f, 1.0f},  // red
    tinygl::Vec4{1.0f, 1.0f, 0.0f, 1.0f},  // yellow
    tinygl::Vec4{0.0f, 1.0f, 0.0f, 1.0f},  // green
    tinygl::Vec4{0.0f, 0.0f, 1.0f, 1.0f},  // blue
    tinygl::Vec4{1.0f, 0.0f, 1.0f, 1.0f},  // magenta
    tinygl::Vec4{0.0f, 1.0f, 1.0f, 1.0f}   // cyan
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
            const auto [x, y] = getCursorPos<float>();
            const auto [w, h] = getWindowSize();
            auto p = tinygl::Vec2{static_cast<float>(2*x/w - 1), static_cast<float>(2*(h-y)/h - 1)};
            vBuffer.update(sizeof(p) * index, sizeof(p), p.data());

            cBuffer.bind();
            auto c = colors.at(index%7);
            cBuffer.update(sizeof(c) * index, sizeof(c), c.data());

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
    vBuffer.create(sizeof(tinygl::Vec2) * maxNumPositions);

    auto positionLoc = program.attributeLocation("aPosition");
    vao.setAttributeArray(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(positionLoc);

    cBuffer.bind();
    cBuffer.create(sizeof(tinygl::Vec4) * maxNumPositions);

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
