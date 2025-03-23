#include "../main.h"
#include <tinygl/tinygl.h>
#include <array>

constexpr auto maxNumTriangles = 200;
constexpr auto maxNumPositions  = 3 * maxNumTriangles;
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
    bool first{true};
    std::array<tinyla::vec2f, 4> t{
        tinyla::vec2f{tinyla::vec_init::uninitialized},
        tinyla::vec2f{tinyla::vec_init::uninitialized},
        tinyla::vec2f{tinyla::vec_init::uninitialized},
        tinyla::vec2f{tinyla::vec_init::uninitialized}
    };
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
            if (first) {
                first = false;
                t[0] = tinyla::vec2f{2*x/w - 1, 2*(h-y)/h - 1};
            } else {
                first = true;
                t[2] = tinyla::vec2f{2*x/w - 1, 2*(h-y)/h - 1};
                t[1] = tinyla::vec2f{t[0][0], t[2][1]};
                t[3] = tinyla::vec2f{t[2][0], t[0][1]};

                vBuffer.bind();
                vBuffer.update(sizeof(tinyla::vec2f) * index, t.begin(), t.end());
                index += 4;

                cBuffer.bind();
                const auto& tt = colors[cIndex];
                for (int i = 0; i < 4; ++i) {
                    cBuffer.update(sizeof(tinyla::vec4f) * (index - 4 + i), sizeof(tinyla::vec4f), tt.data());
                }
            }
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
    for(int i = 0; i < index; i += 4) {
        glDrawArrays(GL_TRIANGLE_FAN, i, 4);
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

    ImGui::End();
}

MAIN
