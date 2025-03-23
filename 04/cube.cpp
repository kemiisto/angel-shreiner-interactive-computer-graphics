#include "../main.h"
#include <tinygl/tinygl.h>
#include <array>

constexpr int numPositions  = 36;
constexpr int xAxis = 0;
constexpr int yAxis = 1;
constexpr int zAxis = 2;

constexpr std::array vertices = {
    tinyla::vec4f{-0.5f, -0.5f,  0.5f, 1.0f},
    tinyla::vec4f{-0.5f,  0.5f,  0.5f, 1.0f},
    tinyla::vec4f{ 0.5f,  0.5f,  0.5f, 1.0f},
    tinyla::vec4f{ 0.5f, -0.5f,  0.5f, 1.0f},
    tinyla::vec4f{-0.5f, -0.5f, -0.5f, 1.0f},
    tinyla::vec4f{-0.5f,  0.5f, -0.5f, 1.0f},
    tinyla::vec4f{ 0.5f,  0.5f, -0.5f, 1.0f},
    tinyla::vec4f{ 0.5f, -0.5f, -0.5f, 1.0f}
};

constexpr std::array vertexColors = {
    tinyla::vec4f{0.0f, 0.0f, 0.0f, 1.0f},  // black
    tinyla::vec4f{1.0f, 0.0f, 0.0f, 1.0f},  // red
    tinyla::vec4f{1.0f, 1.0f, 0.0f, 1.0f},  // yellow
    tinyla::vec4f{0.0f, 1.0f, 0.0f, 1.0f},  // green
    tinyla::vec4f{0.0f, 0.0f, 1.0f, 1.0f},  // blue
    tinyla::vec4f{1.0f, 0.0f, 1.0f, 1.0f},  // magenta
    tinyla::vec4f{0.0f, 1.0f, 1.0f, 1.0f},  // cyan
    tinyla::vec4f{1.0f, 1.0f, 1.0f, 1.0f}   // white
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
    void colorCube();
    void quad(int a, int b, int c, int d);

    std::vector<tinyla::vec4f> positions{};
    std::vector<tinyla::vec4f> colors{};

    tinygl::ShaderProgram program;
    tinygl::Buffer vBuffer{tinygl::Buffer::Type::VertexBuffer, tinygl::Buffer::UsagePattern::StaticDraw};
    tinygl::Buffer cBuffer{tinygl::Buffer::Type::VertexBuffer, tinygl::Buffer::UsagePattern::StaticDraw};
    tinygl::VertexArrayObject vao;

    tinyla::vec3f theta{0.0f, 0.0f, 0.0f};
    int axis = 0;
    int thetaLoc{-1};
};

void Window::init()
{
    colorCube();

    // Configure OpenGL.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);

    // Load shaders and initialize attribute buffers.
    program.addShaderFromSourceFile(tinygl::Shader::Type::Vertex, "cube.vert");
    program.addShaderFromSourceFile(tinygl::Shader::Type::Fragment, "cube.frag");
    program.link();
    program.use();

    // Load the data into the GPU
    vao.bind();

    cBuffer.bind();
    cBuffer.create(colors.begin(), colors.end());

    auto colorLoc = program.attributeLocation("aColor");
    vao.setAttributeArray(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(colorLoc);

    vBuffer.bind();
    vBuffer.create(positions.begin(), positions.end());

    auto positionLoc = program.attributeLocation("aPosition");
    vao.setAttributeArray(positionLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(positionLoc);

    thetaLoc = program.uniformLocation("uTheta");

    setKeyCallback([this](tinygl::keyboard::Key key, int /*scancode*/, tinygl::input::Action action, tinygl::input::Modifier /*mods*/) {
        if (key == tinygl::keyboard::Key::X && action == tinygl::input::Action::Press) {
            axis = xAxis;
        }
        if (key == tinygl::keyboard::Key::Y && action == tinygl::input::Action::Press) {
            axis = yAxis;
        }
        if (key == tinygl::keyboard::Key::Z && action == tinygl::input::Action::Press) {
            axis = zAxis;
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    theta[axis] += 2.0f;
    program.setUniformValue(thetaLoc, theta);

    glDrawArrays(GL_TRIANGLES, 0, numPositions);
}

void Window::drawUi()
{
    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button("Rotate X")) {
        axis = xAxis;
    }

    if (ImGui::Button("Rotate Y")) {
        axis = yAxis;
    }

    if (ImGui::Button("Rotate Z")) {
        axis = zAxis;
    }

    ImGui::End();
}

void Window::colorCube()
{
    quad(1, 0, 3, 2);
    quad(2, 3, 7, 6);
    quad(3, 0, 4, 7);
    quad(6, 5, 1, 2);
    quad(4, 5, 6, 7);
    quad(5, 4, 0, 1);
}

void Window::quad(int a, int b, int c, int d)
{
    // We need to partition the quad into two triangles in order for OpenGL to be able to render it.
    // In this case, we create two triangles from the quad indices.

    // vertex color assigned by the index of the vertex
    std::array indices = {a, b, c, a, c, d};

    for (int i : indices) {
        positions.push_back(vertices[i]);
        // for solid colored faces use
        colors.push_back(vertexColors[a]);
    }
}

MAIN
