#include "../main.h"
#include <tinygl/tinygl.h>
#include <array>

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
    tinygl::vertex_array_object vao;

    float theta = 0.0f;
    int theta_loc = -1;

    bool direction = true;
    float speed_factor = 1.0f;
};

void window::init()
{
    // Configure OpenGL.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Load shaders and initialize attribute buffers.
    program.add_shader_from_source_file(tinygl::shader::type::vertex, "rotatingSquare.vert");
    program.add_shader_from_source_file(tinygl::shader::type::fragment, "rotatingSquare.frag");
    program.link();
    program.use();

    std::array vertices = {
        tinyla::vec2f{ 0.0f,  1.0f},
        tinyla::vec2f{-1.0f,  0.0f},
        tinyla::vec2f{ 1.0f,  0.0f},
        tinyla::vec2f{ 0.0f, -1.0f}
    };

    // Load the data into the GPU
    vao.bind();
    v_buffer.bind();
    v_buffer.create(vertices.begin(), vertices.end());

    // Associate shader variables with our data buffer
    auto positionLoc = program.attribute_location("aPosition");
    vao.set_attribute_array(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enable_attribute_array(positionLoc);

    theta_loc = program.uniform_location("uTheta");

    set_key_callback([this](tinygl::keyboard::key key, int /*scancode*/, tinygl::input::action action, tinygl::input::modifier /*mods*/) {
        if (key == tinygl::keyboard::key::d1 && action == tinygl::input::action::press) {
            direction = !direction;
        }
        if (key == tinygl::keyboard::key::d2 && action == tinygl::input::action::press) {
            speed_factor *= 1.5f;
        }
        if (key == tinygl::keyboard::key::d3 && action == tinygl::input::action::press) {
            speed_factor /= 1.5f;
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
    glClear(GL_COLOR_BUFFER_BIT);

    theta += (direction ? 0.1f * speed_factor : -0.1f * speed_factor);
    program.set_uniform_value(theta_loc, theta);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void window::draw_ui()
{
    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button("Change Rotation Direction")) {
        direction = !direction;
    }

    static int item = 0;
    static const char* const items[] = {
        "Toggle Rotation Direction",
        "Spin Faster",
        "Spin Slower"
    };
    if (ImGui::ListBox("Toggles", &item, items, IM_ARRAYSIZE(items), 3)) {
        switch (item) {
            case 0:
                direction = !direction;
                break;
            case 1:
                speed_factor *= 1.5f;
                break;
            case 2:
                speed_factor /= 1.5f;
                break;
        }
    }

    ImGui::End();
}

MAIN
