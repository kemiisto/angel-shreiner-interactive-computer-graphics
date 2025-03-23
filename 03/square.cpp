#include "../main.h"
#include <tinygl/tinygl.h>

constexpr auto max_num_triangles = 200;
constexpr auto max_num_positions  = 3 * max_num_triangles;

constexpr std::array colors = {
    tinyla::vec4f{0.0f, 0.0f, 0.0f, 1.0f},  // black
    tinyla::vec4f{1.0f, 0.0f, 0.0f, 1.0f},  // red
    tinyla::vec4f{1.0f, 1.0f, 0.0f, 1.0f},  // yellow
    tinyla::vec4f{0.0f, 1.0f, 0.0f, 1.0f},  // green
    tinyla::vec4f{0.0f, 0.0f, 1.0f, 1.0f},  // blue
    tinyla::vec4f{1.0f, 0.0f, 1.0f, 1.0f},  // magenta
    tinyla::vec4f{0.0f, 1.0f, 1.0f, 1.0f}   // cyan
};

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
    int index = 0;
};

void window::init()
{
    set_mouse_button_callback([this](
            tinygl::mouse::button button,
            tinygl::input::action action,
            tinygl::input::modifier /* modifier */) {
        if (button == tinygl::mouse::button::left && action == tinygl::input::action::press) {
            v_buffer.bind();
            const auto [x, y] = get_cursor_pos<float>();
            const auto [w, h] = get_window_size();
            auto p = tinyla::vec2f{static_cast<float>(2*x/w - 1), static_cast<float>(2*(h-y)/h - 1)};
            v_buffer.update(sizeof(p) * index, sizeof(p), p.data());

            c_buffer.bind();
            auto c = colors.at(index%7);
            c_buffer.update(sizeof(c) * index, sizeof(c), c.data());

            index++;
        }
    });
    // Configure OpenGL
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Load shaders and initialize attribute buffers
    program.add_shader_from_source_file(tinygl::shader::type::vertex, "square.vert");
    program.add_shader_from_source_file(tinygl::shader::type::fragment, "square.frag");
    program.link();
    program.use();

    // Load the data into the GPU
    vao.bind();

    v_buffer.bind();
    v_buffer.create(sizeof(tinyla::vec2f) * max_num_positions);

    auto positionLoc = program.attribute_location("aPosition");
    vao.set_attribute_array(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enable_attribute_array(positionLoc);

    c_buffer.bind();
    c_buffer.create(sizeof(tinyla::vec4f) * max_num_positions);

    auto colorLoc = program.attribute_location("aColor");
    vao.set_attribute_array(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    vao.enable_attribute_array(colorLoc);
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
    glDrawArrays(GL_POINTS, 0, index);
}

MAIN
