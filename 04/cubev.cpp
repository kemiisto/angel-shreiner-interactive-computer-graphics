#include "../main.h"
#include <tinygl/tinygl.h>
#include <array>

constexpr int num_elements  = 36;
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

// indices of the 12 triangles that comprise the cube
constexpr std::array<GLubyte, 36> indices = {
    1, 0, 3,
    3, 2, 1,
    2, 3, 7,
    7, 6, 2,
    3, 0, 4,
    4, 7, 3,
    6, 5, 1,
    1, 2, 6,
    4, 5, 6,
    6, 7, 4,
    5, 4, 0,
    0, 1, 5
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
    tinygl::shader_program program;
    tinygl::buffer v_buffer{tinygl::buffer::type::vertex_buffer, tinygl::buffer::usage_pattern::static_draw};
    tinygl::buffer c_buffer{tinygl::buffer::type::vertex_buffer, tinygl::buffer::usage_pattern::static_draw};
    tinygl::buffer i_buffer{tinygl::buffer::type::index_buffer, tinygl::buffer::usage_pattern::static_draw};
    tinygl::vertex_array_object vao;

    tinyla::vec3f theta{0.0f, 0.0f, 0.0f};
    int axis = 0;
    int theta_loc{-1};
};

void window::init()
{
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

    // array element buffer
    i_buffer.bind();
    i_buffer.create(indices.begin(), indices.end());

    // color array atrribute buffer
    c_buffer.bind();
    c_buffer.create(vertex_colors.begin(), vertex_colors.end());

    auto colorLoc = program.attribute_location("aColor");
    vao.set_attribute_array(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enable_attribute_array(colorLoc);

    // vertex array attribute buffer
    v_buffer.bind();
    v_buffer.create(vertices.begin(), vertices.end());

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

    glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_BYTE, 0);
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

MAIN
