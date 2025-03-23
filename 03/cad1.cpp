#include "../main.h"
#include <tinygl/tinygl.h>
#include <array>

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
    void draw_ui() override;
private:
    tinygl::shader_program program;
    tinygl::buffer v_buffer{tinygl::buffer::type::vertex_buffer, tinygl::buffer::usage_pattern::static_draw};
    tinygl::buffer c_buffer{tinygl::buffer::type::vertex_buffer, tinygl::buffer::usage_pattern::static_draw};
    tinygl::vertex_array_object vao;
    int index{0};
    int c_index{0};
    bool first{true};
    std::array<tinyla::vec2f, 4> t{
        tinyla::vec2f{tinyla::vec_init::uninitialized},
        tinyla::vec2f{tinyla::vec_init::uninitialized},
        tinyla::vec2f{tinyla::vec_init::uninitialized},
        tinyla::vec2f{tinyla::vec_init::uninitialized}
    };
};

void window::init()
{
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Load shaders and initialize attribute buffers.
    program.add_shader_from_source_file(tinygl::shader::type::vertex, "cad.vert");
    program.add_shader_from_source_file(tinygl::shader::type::fragment, "cad.frag");
    program.link();
    program.use();

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

    set_mouse_button_callback([this](
        tinygl::mouse::button button,
        tinygl::input::action action,
        tinygl::input::modifier /* modifier */) {
        if (button == tinygl::mouse::button::left && action == tinygl::input::action::press) {
            const auto [x, y] = get_cursor_pos<float>();
            const auto [w, h] = get_window_size();
            if (first) {
                first = false;
                t[0] = tinyla::vec2f{2*x/w - 1, 2*(h-y)/h - 1};
            } else {
                first = true;
                t[2] = tinyla::vec2f{2*x/w - 1, 2*(h-y)/h - 1};
                t[1] = tinyla::vec2f{t[0][0], t[2][1]};
                t[3] = tinyla::vec2f{t[2][0], t[0][1]};

                v_buffer.bind();
                v_buffer.update(sizeof(tinyla::vec2f) * index, t.begin(), t.end());
                index += 4;

                c_buffer.bind();
                const auto& tt = colors[c_index];
                for (int i = 0; i < 4; ++i) {
                    c_buffer.update(sizeof(tinyla::vec4f) * (index - 4 + i), sizeof(tinyla::vec4f), tt.data());
                }
            }
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
    for(int i = 0; i < index; i += 4) {
        glDrawArrays(GL_TRIANGLE_FAN, i, 4);
    }
}

void window::draw_ui()
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
    ImGui::ListBox("Color", &c_index, items, IM_ARRAYSIZE(items), 7);

    ImGui::End();
}

MAIN
