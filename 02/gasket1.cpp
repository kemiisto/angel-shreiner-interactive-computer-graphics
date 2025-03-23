#include "../main.h"
#include <tinygl/tinygl.h>
#include <array>
#include <random>
#include <vector>

constexpr int num_positions = 5000;

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
    auto positions = std::vector<tinyla::vec2f>{};
    positions.reserve(num_positions);

    // First, initialize the corners of our gasket with three positions.
    constexpr auto vertices = std::array {
        tinyla::vec2f{-1.0f, -1.0f},
        tinyla::vec2f{ 0.0f,  1.0f},
        tinyla::vec2f{ 1.0f, -1.0f}
    };

    auto device = std::random_device{};
    auto engine = std::mt19937{device()};
    auto distribution = std::uniform_int_distribution<int>{0, 2};

    // Specify a starting positions for our iterations - it must lie inside any set of three vertices
    auto const u = vertices[0] + vertices[1];
    auto const v = vertices[0] + vertices[2];
    positions.emplace_back(0.25f * (u + v));

    // Compute new positions
    // Each new point is located midway between last point and a randomly chosen vertex
    for (int i = 0; i < num_positions; ++i) {
        auto const j = distribution(engine);
        positions.emplace_back(0.5f * (positions[i] + vertices[j]));
    }

    // Configure OpenGL
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Load shaders and initialize attribute buffers
    program.add_shader_from_source_file(tinygl::shader::type::vertex, "gasket1.vert");
    program.add_shader_from_source_file(tinygl::shader::type::fragment, "gasket1.frag");
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
    glDrawArrays(GL_POINTS, 0, num_positions);
}

MAIN
