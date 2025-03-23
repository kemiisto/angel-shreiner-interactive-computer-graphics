#include "../main.h"
#include <tinygl/tinygl.h>
#include <array>
#include <vector>

constexpr int num_times_to_subdivide = 5;

auto positions = std::vector<tinyla::vec2f>{};

void triangle(const tinyla::vec2f& a, const tinyla::vec2f& b, const tinyla::vec2f& c)
{
    positions.push_back(a);
    positions.push_back(b);
    positions.push_back(c);
}

void divide_triangle(const tinyla::vec2f& a, const tinyla::vec2f& b, const tinyla::vec2f& c, int count)
{
    // check for end of recursion
    if (count == 0) {
        triangle(a, b, c);
    } else {
        // bisect the sides
        auto const ab = 0.5f * (a + b);
        auto const ac = 0.5f * (a + c);
        auto const bc = 0.5f * (b + c);
        --count;
        // three new triangles
        divide_triangle(a, ab, ac, count);
        divide_triangle(c, ac, bc, count);
        divide_triangle(b, bc, ab, count);
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
    tinygl::vertex_array_object vao;
};

void window::init()
{
    // First, initialize the corners of our gasket with three positions.
    auto const vertices = std::array {
        tinyla::vec2f{-1.0f, -1.0f},
        tinyla::vec2f{ 0.0f,  1.0f},
        tinyla::vec2f{ 1.0f, -1.0f}
    };

    divide_triangle(vertices[0], vertices[1], vertices[2], num_times_to_subdivide);

    // Configure OpenGL
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Load shaders and initialize attribute buffers
    program.add_shader_from_source_file(tinygl::shader::type::vertex, "gasket2.vert");
    program.add_shader_from_source_file(tinygl::shader::type::fragment, "gasket2.frag");
    program.link();
    program.use();

    // Load the data into the GPU
    vao.bind();
    v_buffer.bind();
    v_buffer.create(positions.begin(), positions.end());

    // Associate shader variables with our data buffer
    auto const position_loc = program.attribute_location("aPosition");
    vao.set_attribute_array(position_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enable_attribute_array(position_loc);
}

void window::process_input()
{
    if (get_key(tinygl::keyboard::key::escape) == tinygl::keyboard::key_state::press) {
        set_should_close(true);
    }
}

void window::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(positions.size()));
}

MAIN
