#include "../main.h"
#include <tinygl/tinygl.h>
#include <array>

constexpr int num_positions  = 36;
constexpr int x_axis = 0;
constexpr int y_axis = 1;
constexpr int z_axis = 2;

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

constexpr std::array vertex_colors = {
    tinyla::vec4f{0.0f, 0.0f, 0.0f, 1.0f},  // black
    tinyla::vec4f{1.0f, 0.0f, 0.0f, 1.0f},  // red
    tinyla::vec4f{1.0f, 1.0f, 0.0f, 1.0f},  // yellow
    tinyla::vec4f{0.0f, 1.0f, 0.0f, 1.0f},  // green
    tinyla::vec4f{0.0f, 0.0f, 1.0f, 1.0f},  // blue
    tinyla::vec4f{1.0f, 0.0f, 1.0f, 1.0f},  // magenta
    tinyla::vec4f{0.0f, 1.0f, 1.0f, 1.0f},  // cyan
    tinyla::vec4f{1.0f, 1.0f, 1.0f, 1.0f}   // white
};

class window final : public tinygl::window
{
public:
    using tinygl::window::window;
    void init() override;
    void process_input() override;
    void draw() override;
    void draw_ui() override;
private:
    void color_cube();
    void quad(int a, int b, int c, int d);

    std::vector<tinyla::vec4f> positions{};
    std::vector<tinyla::vec4f> colors{};

    tinygl::shader_program program;
    tinygl::buffer v_buffer{tinygl::buffer::type::vertex_buffer, tinygl::buffer::usage_pattern::static_draw};
    tinygl::buffer c_buffer{tinygl::buffer::type::vertex_buffer, tinygl::buffer::usage_pattern::static_draw};
    tinygl::vertex_array_object vao;

    tinyla::vec3f theta{0.0f, 0.0f, 0.0f};
    int axis = 0;
    int theta_loc{-1};
};

void window::init()
{
    color_cube();

    // Configure OpenGL.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);

    // Load shaders and initialize attribute buffers.
    program.add_shader_from_source_file(tinygl::shader::type::vertex, "cube.vert");
    program.add_shader_from_source_file(tinygl::shader::type::fragment, "cube.frag");
    program.link();
    program.use();

    // Load the data into the GPU
    vao.bind();

    c_buffer.bind();
    c_buffer.create(colors.begin(), colors.end());

    auto colorLoc = program.attribute_location("aColor");
    vao.set_attribute_array(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enable_attribute_array(colorLoc);

    v_buffer.bind();
    v_buffer.create(positions.begin(), positions.end());

    auto positionLoc = program.attribute_location("aPosition");
    vao.set_attribute_array(positionLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enable_attribute_array(positionLoc);

    theta_loc = program.uniform_location("uTheta");

    set_key_callback([this](tinygl::keyboard::key key, int /*scancode*/, tinygl::input::action action, tinygl::input::modifier /*mods*/) {
        if (key == tinygl::keyboard::key::x && action == tinygl::input::action::press) {
            axis = x_axis;
        }
        if (key == tinygl::keyboard::key::y && action == tinygl::input::action::press) {
            axis = y_axis;
        }
        if (key == tinygl::keyboard::key::z && action == tinygl::input::action::press) {
            axis = z_axis;
        }
    });
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

    theta[axis] += 2.0f;
    program.set_uniform_value(theta_loc, theta);

    glDrawArrays(GL_TRIANGLES, 0, num_positions);
}

void window::draw_ui()
{
    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button("Rotate X")) {
        axis = x_axis;
    }

    if (ImGui::Button("Rotate Y")) {
        axis = y_axis;
    }

    if (ImGui::Button("Rotate Z")) {
        axis = z_axis;
    }

    ImGui::End();
}

void window::color_cube()
{
    quad(1, 0, 3, 2);
    quad(2, 3, 7, 6);
    quad(3, 0, 4, 7);
    quad(6, 5, 1, 2);
    quad(4, 5, 6, 7);
    quad(5, 4, 0, 1);
}

void window::quad(int a, int b, int c, int d)
{
    // We need to partition the quad into two triangles in order for OpenGL to be able to render it.
    // In this case, we create two triangles from the quad indices.

    // vertex color assigned by the index of the vertex
    std::array indices = {a, b, c, a, c, d};

    for (int i : indices) {
        positions.push_back(vertices[i]);
        // for solid colored faces use
        colors.push_back(vertex_colors[a]);
    }
}

MAIN
