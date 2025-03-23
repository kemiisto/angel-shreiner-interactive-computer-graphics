#include "../main.h"
#include <tinygl/tinygl.h>
#include <array>
#include <vector>

constexpr int num_times_to_subdivide = 3;

auto const base_colors = std::array {
    tinyla::vec3f{1.0f, 0.0f, 0.0f},
    tinyla::vec3f{0.0f, 1.0f, 0.0f},
    tinyla::vec3f{0.0f, 0.0f, 1.0f},
    tinyla::vec3f{0.0f, 0.0f, 0.0f}
};

auto positions = std::vector<tinyla::vec3f>{};
auto colors = std::vector<tinyla::vec3f>{};

void triangle(const tinyla::vec3f& a, const tinyla::vec3f& b, const tinyla::vec3f& c, int color)
{
    // add colors and vertices for one triangle
    colors.push_back(base_colors.at(color));
    positions.push_back(a);
    colors.push_back(base_colors.at(color));
    positions.push_back(b);
    colors.push_back(base_colors.at(color));
    positions.push_back(c);
}

void tetra(const tinyla::vec3f& a, const tinyla::vec3f& b, const tinyla::vec3f& c, const tinyla::vec3f& d)
{
    // tetrahedron with each side using a different color
    triangle(a, c, b, 0);
    triangle(a, c, d, 1);
    triangle(a, b, d, 2);
    triangle(b, c, d, 3);
}

void divide_tetra(const tinyla::vec3f& a, const tinyla::vec3f& b, const tinyla::vec3f& c, const tinyla::vec3f& d, int count)
{
    // check for end of recursion
    if (count == 0) {
        tetra(a, b, c, d);
    } else {
        // find midpoints of sides, divide four smaller tetrahedra
        auto const ab = 0.5f * (a + b);
        auto const ac = 0.5f * (a + c);
        auto const ad = 0.5f * (a + d);
        auto const bc = 0.5f * (b + c);
        auto const bd = 0.5f * (b + d);
        auto const cd = 0.5f * (c + d);

        --count;

        divide_tetra( a, ab, ac, ad, count);
        divide_tetra(ab,  b, bc, bd, count);
        divide_tetra(ac, bc,  c, cd, count);
        divide_tetra(ad, bd, cd,  d, count);
    }
}

class window final : public tinygl::window
{
public:
    using tinygl::window::window;
    void init() override;
    void process_input() override;
    void draw() override;
private:
    tinygl::shader_program program;
    tinygl::buffer v_buffer{tinygl::buffer::type::vertex_buffer, tinygl::buffer::usage_pattern::static_draw};
    tinygl::buffer c_buffer{tinygl::buffer::type::vertex_buffer, tinygl::buffer::usage_pattern::static_draw};
    tinygl::vertex_array_object vao;
};

void window::init()
{
    // First, initialize the corners of our gasket with three positions.
    auto vertices = std::array {
        tinyla::vec3f{ 0.0000f,  0.0000f, -1.0000f},
        tinyla::vec3f{ 0.0000f,  0.9428f,  0.3333f},
        tinyla::vec3f{-0.8165f, -0.4714f,  0.3333f},
        tinyla::vec3f{ 0.8165f, -0.4714f,  0.3333f}
    };

    divide_tetra(vertices[0], vertices[1], vertices[2], vertices[3], num_times_to_subdivide);

    // Configure OpenGL
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);

    // Load shaders and initialize attribute buffers
    program.add_shader_from_source_file(tinygl::shader::type::vertex, "gasket4.vert");
    program.add_shader_from_source_file(tinygl::shader::type::fragment, "gasket4.frag");
    program.link();
    program.use();

    // Load the data into the GPU
    vao.bind();

    v_buffer.bind();
    v_buffer.create(positions.begin(), positions.end());

    auto const position_loc = program.attribute_location("aPosition");
    vao.set_attribute_array(position_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enable_attribute_array(position_loc);

    c_buffer.bind();
    c_buffer.create(colors.begin(), colors.end());

    auto const color_loc = program.attribute_location("aColor");
    vao.set_attribute_array(color_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enable_attribute_array(color_loc);
}

void window::process_input()
{
    if (get_key(tinygl::keyboard::key::escape) == tinygl::keyboard::key_state::press) {
        set_should_close(true);
    }
}

void window::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(positions.size()));
}

MAIN
