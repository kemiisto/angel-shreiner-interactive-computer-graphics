#include "../main.h"
#include <tinygl/tinygl.h>
#include <array>

constexpr auto max_num_positions  = 200;
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
    int num_polygons{0};
    std::vector<int> num_positions{0};
    std::vector<int> start{0};
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

            const auto t = tinyla::vec2f{2*x/w - 1, 2*(h-y)/h - 1};
            v_buffer.bind();
            v_buffer.update(sizeof(tinyla::vec2f) * index, sizeof(tinyla::vec2f), t.data());

            const auto& tt = colors[c_index];
            c_buffer.bind();
            c_buffer.update(sizeof(tinyla::vec4f) * index, sizeof(tinyla::vec4f), tt.data());

            num_positions[num_polygons]++;
            index++;
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
    for (int i = 0; i < num_polygons; ++i) {
        glDrawArrays(GL_TRIANGLE_FAN, start[i], num_positions[i]);
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

    if (ImGui::Button("End Polygon")) {
        num_polygons++;
        num_positions.push_back(0);
        start.push_back(index);
    }

    ImGui::End();
}

MAIN
