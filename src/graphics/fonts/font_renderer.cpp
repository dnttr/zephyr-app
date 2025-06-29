//
// Created by Damian Netter on 28/06/2025.
//

#include "ZCApp/graphics/fonts/font_renderer.hpp"

#include <chrono>

#include "ZCApp/graphics/fonts/font_loader.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"
#include "ZCApp/graphics/utils/time_util.hpp"
#include "ZCGKit/external.hpp"

#define SPACE 5

void zc_app::font_renderer::build_shader()
{
    program = shaders::create_program("text_vert", "text_frag");

    u_atlas = glGetUniformLocation(program, "atlas");

    setup_ubo();
}

zc_app::font_renderer::geometry zc_app::font_renderer::build_geometry()
{
    GLuint vao, vbo, ebo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 5, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return { vao, vbo, ebo };
}

void zc_app::font_renderer::setup_ubo()
{
    glGenBuffers(1, &text_ubo);
    glGenBuffers(1, &transform_ubo);

    glBindBuffer(GL_UNIFORM_BUFFER, text_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(text_properties), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_UNIFORM_BUFFER, transform_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(transform_properties), nullptr, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, text_ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, transform_ubo);

    const GLuint textPropsIndex = glGetUniformBlockIndex(program, "text_properties");
    const GLuint transformPropsIndex = glGetUniformBlockIndex(program, "transform_properties");

    if (textPropsIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(program, textPropsIndex, 0);
    } else {
        printf("ERROR: text_properties uniform block not found in shader!\n");
    }

    if (transformPropsIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(program, transformPropsIndex, 1);
    } else {
        printf("ERROR: transform_properties uniform block not found in shader!\n");
    }

    if (const GLenum error = glGetError(); error != GL_NO_ERROR) {
        printf("OpenGL error in setup_ubo: %d\n", error);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void zc_app::font_renderer::update_text_properties(const text_properties& properties)
{
    glBindBuffer(GL_UNIFORM_BUFFER, text_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(text_properties), &properties);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void zc_app::font_renderer::update_transform_properties(const transform_properties& properties)
{
    glBindBuffer(GL_UNIFORM_BUFFER, transform_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(transform_properties), &properties);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void zc_app::font_renderer::set_parameters(const text_style &style, text_properties &text_props, transform_properties &transform_props, const properties &props)
{
    text_props.smoothing = 0.01F;

    text_props.text_color = glm::vec4(
        style.text_color.get_red_direct(),
        style.text_color.get_green_direct(),
        style.text_color.get_blue_direct(),
        style.text_color.get_alpha_direct()
    );

    text_props.text_shadow_enable = style.shadow_enable;
    text_props.text_shadow_color = glm::vec4(
        style.shadow_color.get_red_direct(),
        style.shadow_color.get_green_direct(),
        style.shadow_color.get_blue_direct(),
        style.shadow_color.get_alpha_direct()
    );

    text_props.text_shadow_offset = glm::vec2(style.shadow_offset.first, style.shadow_offset.second);

    text_props.text_outline_enable = style.outline_enable;
    text_props.text_outline_color = glm::vec4(
        style.outline_color.get_red_direct(),
        style.outline_color.get_green_direct(),
        style.outline_color.get_blue_direct(),
        style.outline_color.get_alpha_direct()
    );
    text_props.text_outline_width = style.outline_width;

    text_props.text_glow_enable = style.glow_enable;
    text_props.text_glow_radius = style.glow_radius;
    text_props.text_glow_intensity = style.glow_intensity;
    text_props.text_glow_color = glm::vec4(
        style.glow_color.get_red_direct(),
        style.glow_color.get_green_direct(),
        style.glow_color.get_blue_direct(),
        style.glow_color.get_alpha_direct()
    );

    text_props.text_rainbow_enable = style.rainbow_enable;
    text_props.text_rainbow_speed = style.rainbow_speed;
    text_props.text_rainbow_variation = style.rainbow_variation;
    text_props.text_rainbow_saturation = style.rainbow_saturation;

    update_text_properties(text_props);

    transform_props.total_characters_amount = props.total_characters;
    transform_props.text_animation_begin = style.text_animation_scale_begin;
    transform_props.text_animation_end = style.text_animation_scale_end;
    transform_props.text_animation_speed = style.text_animation_scale_speed;
    transform_props.text_magnification = style.text_size_magnification;
}

void zc_app::font_renderer::render(properties &props, text_properties &text_props, transform_properties &transform_props, const geometry &geom)
{
    glUniform1i(u_atlas, 0);

    const float currentTime = time_util::get_elapsed_time(props.startTime);

    text_props.time = currentTime;
    text_props.text_position = glm::vec2(props.x, props.y);

    glBindBuffer(GL_UNIFORM_BUFFER, text_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(text_properties), &text_props);

    transform_props.time = currentTime;
    transform_props.projection_matrix = perspective_util::get_projection_matrix_mat4();

    glBindBuffer(GL_UNIFORM_BUFFER, transform_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(transform_properties), &transform_props);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    vertices.reserve(props.total_characters * 4 * 5);
    indices.reserve(props.total_characters * 6);

    unsigned int vertex_offset = 0;
    float base_y = props.y;

    for (const auto &line : props.lines)
    {
        float cursor_x = props.x;
        float cursor_y = base_y;

        auto *buf = hb_buffer_create();

        hb_buffer_add_utf8(buf, line.c_str(), -1, 0, -1);
        hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
        hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
        hb_buffer_set_language(buf, hb_language_from_string("en", -1));

        hb_shape(props.font.hb_font, buf, nullptr, 0);

        unsigned int glyph_count;

        const auto *glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
        const auto *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

        int line_height = 0;

        for (int i = 0; i < glyph_count; i++)
        {
            const auto glyph_index = glyph_info[i].codepoint;
            const auto it = props.font.characters_map.find(glyph_index);

            if (it == props.font.characters_map.end())
            {
                continue;
            }

            const auto character = it->second;

            const auto glyph_xpos = static_cast<float>(glyph_pos[i].x_offset) / 64.0f;
            const auto glyph_ypos = static_cast<float>(glyph_pos[i].y_offset) / 64.0f;

            const auto bearing_xpos = static_cast<float>(character.bearing.x);
            const auto bearing_ypos = static_cast<float>(character.bearing.y);

            const auto size_xpos = static_cast<float>(character.size.x);
            const auto size_ypos = static_cast<float>(character.size.y);

            const auto atlas_offset_xpos = static_cast<float>(character.atlas_offset.x);
            const auto atlas_offset_ypos = static_cast<float>(character.atlas_offset.y);

            const float x_advance = static_cast<float>(glyph_pos[i].x_advance) / 64.0f;
            const float y_advance = static_cast<float>(glyph_pos[i].y_advance) / 64.0f;
            line_height = (size_ypos + bearing_ypos) * 2 + SPACE;

            const float xpos = cursor_x + glyph_xpos * props.text_scale + bearing_xpos * props.text_scale;
            const float ypos = cursor_y - (bearing_ypos - glyph_ypos) * props.text_scale;

            const float width = size_xpos * props.text_scale;
            const float height = size_ypos * props.text_scale;

            const float texture_xpos = atlas_offset_xpos / font_loader::ATLAS_WIDTH;
            const float texture_ypos = atlas_offset_ypos / font_loader::ATLAS_HEIGHT;

            const float texture_width = size_xpos / font_loader::ATLAS_WIDTH;
            const float texture_height = size_ypos / font_loader::ATLAS_HEIGHT;

            const auto index = static_cast<float>(i);

            vertices.insert(vertices.end(), {xpos, ypos, texture_xpos, texture_ypos, index});
            vertices.insert(vertices.end(), {xpos + width, ypos, texture_xpos + texture_width, texture_ypos, index});
            vertices.insert(vertices.end(), {
                                xpos + width, ypos + height, texture_xpos + texture_width,
                                texture_ypos + texture_height, index
                            });
            vertices.insert(vertices.end(), {xpos, ypos + height, texture_xpos, texture_ypos + texture_height, index});

            indices.insert(indices.end(), {vertex_offset, vertex_offset + 1, vertex_offset + 2});
            indices.insert(indices.end(), {vertex_offset, vertex_offset + 2, vertex_offset + 3});

            cursor_x += x_advance * props.text_scale;
            cursor_y += y_advance * props.text_scale;

            vertex_offset += 4;
        }

        base_y += line_height;

        hb_buffer_destroy(buf);
    }

    if (!vertices.empty())
    {
        glBindVertexArray(geom.vao);
        glBindBuffer(GL_ARRAY_BUFFER, geom.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geom.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

GLuint zc_app::font_renderer::get_program()
{
    return program;
}

zc_app::font_renderer::~font_renderer()
{
    if (zcg_kit::external::is_context_valid())
    {
        glDeleteBuffers(1, &text_ubo);
        glDeleteBuffers(1, &transform_ubo);
        glDeleteProgram(program);
    }
}