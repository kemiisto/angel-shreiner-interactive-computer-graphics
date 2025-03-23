#include "../main.h"
#include <tinygl/tinygl.h>
#include <array>

constexpr auto maxNumPositions  = 200;
constexpr std::array colors = {
    tinyla::vec4f{0.0f, 0.0f, 0.0f, 1.0f},  // black
    tinyla::vec4f{1.0f, 0.0f, 0.0f, 1.0f},  // red
    tinyla::vec4f{1.0f, 1.0f, 0.0f, 1.0f},  // yellow
    tinyla::vec4f{0.0f, 1.0f, 0.0f, 1.0f},  // green
    tinyla::vec4f{0.0f, 0.0f, 1.0f, 1.0f},  // blue
    tinyla::vec4f{1.0f, 0.0f, 1.0f, 1.0f},  // magenta
    tinyla::vec4f{0.0f, 1.0f, 1.0f, 1.0f}   // cyan
};

class Window final : public tinygl::Window
{
public:
    using tinygl::Window::Window;
    void init() override;
    void processInput() override;
    void draw() override;
    void drawUi() override;
private:
    tinygl::ShaderProgram program;
    tinygl::Buffer vBuffer{tinygl::Buffer::Type::VertexBuffer, tinygl::Buffer::UsagePattern::StaticDraw};
    tinygl::Buffer cBuffer{tinygl::Buffer::Type::VertexBuffer, tinygl::Buffer::UsagePattern::StaticDraw};
    tinygl::VertexArrayObject vao;
    int index{0};
    int cIndex{0};
    int numPolygons{0};
    std::vector<int> numPositions{0};
    std::vector<int> start{0};
};

void Window::init()
{
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Load shaders and initialize attribute buffers.
    program.addShaderFromSourceFile(tinygl::Shader::Type::Vertex, "cad.vert");
    program.addShaderFromSourceFile(tinygl::Shader::Type::Fragment, "cad.frag");
    program.link();
    program.use();

    vao.bind();
    vBuffer.bind();
    vBuffer.create(sizeof(tinyla::vec2f) * maxNumPositions);

    auto positionLoc = program.attributeLocation("aPosition");
    vao.setAttributeArray(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(positionLoc);

    cBuffer.bind();
    cBuffer.create(sizeof(tinyla::vec4f) * maxNumPositions);

    auto colorLoc = program.attributeLocation("aColor");
    vao.setAttributeArray(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(colorLoc);

    setMouseButtonCallback([this](
        tinygl::mouse::Button button,
        tinygl::input::Action action,
        tinygl::input::Modifier /* modifier */) {
        if (button == tinygl::mouse::Button::Left && action == tinygl::input::Action::Press) {
            const auto [x, y] = getCursorPos<float>();
            const auto [w, h] = getWindowSize();

            const auto t = tinyla::vec2f{2*x/w - 1, 2*(h-y)/h - 1};
            vBuffer.bind();
            vBuffer.update(sizeof(tinyla::vec2f) * index, sizeof(tinyla::vec2f), t.data());

            const auto& tt = colors[cIndex];
            cBuffer.bind();
            cBuffer.update(sizeof(tinyla::vec4f) * index, sizeof(tinyla::vec4f), tt.data());

            numPositions[numPolygons]++;
            index++;
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
    for (int i = 0; i < numPolygons; ++i) {
        glDrawArrays(GL_TRIANGLE_FAN, start[i], numPositions[i]);
    }
}

void Window::drawUi()
{
    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    static const char* const items[] = {
        "black",
        "red",
        "yellow",
        "green",
        "blue",
        "magenta",
        "cyan"
    };
    ImGui::ListBox("Color", &cIndex, items, IM_ARRAYSIZE(items), 7);

    if (ImGui::Button("End Polygon")) {
        numPolygons++;
        numPositions.push_back(0);
        start.push_back(index);
    }

    ImGui::End();
}

MAIN
