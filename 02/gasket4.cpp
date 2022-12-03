#include "../main.h"
#include <tinygl/tinygl.h>
#include <array>

constexpr int numTimesToSubdivide = 3;
const std::array baseColors = {
    tinygl::Vec3{1.0f, 0.0f, 0.0f},
    tinygl::Vec3{0.0f, 1.0f, 0.0f},
    tinygl::Vec3{0.0f, 0.0f, 1.0f},
    tinygl::Vec3{0.0f, 0.0f, 0.0f}
};

std::vector<tinygl::Vec3> positions;
std::vector<tinygl::Vec3> colors;

void triangle(const tinygl::Vec3& a, const tinygl::Vec3& b, const tinygl::Vec3& c, int color)
{
    // add colors and vertices for one triangle
    colors.push_back(baseColors.at(color));
    positions.push_back(a);
    colors.push_back(baseColors.at(color));
    positions.push_back(b);
    colors.push_back(baseColors.at(color));
    positions.push_back(c);
}

void tetra(const tinygl::Vec3& a, const tinygl::Vec3& b, const tinygl::Vec3& c, const tinygl::Vec3& d)
{
    // tetrahedron with each side using a different color
    triangle(a, c, b, 0);
    triangle(a, c, d, 1);
    triangle(a, b, d, 2);
    triangle(b, c, d, 3);
}

void divideTetra(const tinygl::Vec3& a, const tinygl::Vec3& b, const tinygl::Vec3& c, const tinygl::Vec3& d, int count)
{
    // check for end of recursion
    if (count == 0) {
        tetra(a, b, c, d);
    } else {
        // find midpoints of sides, divide four smaller tetrahedra
        auto ab = 0.5f * (a + b);
        auto ac = 0.5f * (a + c);
        auto ad = 0.5f * (a + d);
        auto bc = 0.5f * (b + c);
        auto bd = 0.5f * (b + d);
        auto cd = 0.5f * (c + d);

        --count;

        divideTetra(a, ab, ac, ad, count);
        divideTetra(ab,  b, bc, bd, count);
        divideTetra(ac, bc,  c, cd, count);
        divideTetra(ad, bd, cd,  d, count);
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
    tinygl::Buffer cBuffer{tinygl::Buffer::Type::VertexBuffer, tinygl::Buffer::UsagePattern::StaticDraw};
    tinygl::VertexArrayObject vao;
};

void Window::init()
{
    // First, initialize the corners of our gasket with three positions.
    tinygl::Vec3 vertices[] = {
        {  0.0000,  0.0000, -1.0000 },
        {  0.0000,  0.9428,  0.3333 },
        { -0.8165, -0.4714,  0.3333 },
        {  0.8165, -0.4714,  0.3333 }
    };

    divideTetra(vertices[0], vertices[1], vertices[2], vertices[3], numTimesToSubdivide);

    // Configure OpenGL
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);

    // Load shaders and initialize attribute buffers
    program.addShaderFromSourceFile(tinygl::Shader::Type::Vertex, "gasket4.vert");
    program.addShaderFromSourceFile(tinygl::Shader::Type::Fragment, "gasket4.frag");
    program.link();
    program.use();

    // Load the data into the GPU
    vao.bind();

    vBuffer.bind();
    vBuffer.create(positions.begin(), positions.end());

    auto positionLoc = program.attributeLocation("aPosition");
    vao.setAttributeArray(positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enableAttributeArray(positionLoc);

    cBuffer.bind();
    cBuffer.create(colors.begin(), colors.end());

    auto colorLoc = program.attributeLocation("aColor");
    vao.setAttributeArray(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, positions.size());
}

MAIN
