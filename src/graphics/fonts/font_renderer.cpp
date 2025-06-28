//
// Created by Damian Netter on 28/06/2025.
//

#include "ZCApp/graphics/fonts/font_renderer.hpp"

#include <chrono>
#include <sstream>

#include "ZCApp/graphics/fonts/font_loader.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

void zc_app::font_renderer::setup(const std::string &name) {
    program = shaders::create_program("text_vert", "text_frag");

    projection = glGetUniformLocation(program, "projection");
    fontAtlas = glGetUniformLocation(program, "fontAtlas");
    textColor = glGetUniformLocation(program, "textColor");
    outlineColor = glGetUniformLocation(program, "outlineColor");
    shadowColor = glGetUniformLocation(program, "shadowColor");
    smoothing = glGetUniformLocation(program, "smoothing");
    outlineWidth = glGetUniformLocation(program, "outlineWidth");
    shadowOffset = glGetUniformLocation(program, "shadowOffset");
    useEffects = glGetUniformLocation(program, "useEffects");
    totalChars = glGetUniformLocation(program, "totalChars");
    variation = glGetUniformLocation(program, "variation");
    position = glGetUniformLocation(program, "position");
    saturation = glGetUniformLocation(program, "saturation");
    time_u = glGetUniformLocation(program, "time");
    speed = glGetUniformLocation(program, "speed");
    glowColor = glGetUniformLocation(program, "glowColor");
    glowRadius = glGetUniformLocation(program, "glowRadius");
    glowIntensity = glGetUniformLocation(program, "glowIntensity");
    useGlow = glGetUniformLocation(program, "useGlow");

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

    setup_x = true;
}

auto startTime = std::chrono::high_resolution_clock::now();

void zc_app::font_renderer::render(font_manager::font &font, const std::string &text, float x, float y, float f_scale,
    const colour &color) {
    if (!setup_x)
    {
        setup(text);
    }

    if (program == -1 || program == 0) {
        debug_print_cerr("Invalid shader program: " + std::to_string(program));
        return;
    }

    std::vector<std::string> lines;
    std::stringstream ss(text);
    std::string line;

    while (std::getline(ss, line, '\n')) {
        lines.push_back(line);
    }

    size_t total = 0;
    for (auto str : lines) {
        total += str.length();
    }

    glUseProgram(program);
    glUniform1i(fontAtlas, 0);

    auto proj = perspective_util::get_projection_matrix();
    glUniformMatrix4fv(projection, 1, GL_FALSE, proj);

    glUniform1f(smoothing, 0.01f);
    glUniform1f(outlineWidth, 0.0f);
    glUniform2f(shadowOffset, 0.0f, 0.0f);
    glUniform4f(outlineColor, 0.0f, 0.0f, 0.0f, 1.0f);
    glUniform4f(shadowColor, 0.0f, 0.0f, 0.0f, 0.5f);
    glUniform1i(useEffects, 1);
    glUniform1i(totalChars, static_cast<int>(total));
    glUniform1f(variation, 0.3f);
    glUniform1f(saturation, 0.5f);
    glUniform1f(speed, 0.1f);
    glUniform4f(glowColor, 1.0f, 1.0f, 0.0f, 0.8f);
    glUniform1f(glowRadius, 0.25f);
    glUniform1f(glowIntensity, 1.0f);
    glUniform1i(useGlow, 1);

    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = currentTime - startTime;
    float timeSeconds = elapsed.count();
    glUniform1f(time_u, timeSeconds);

    glUniform4f(textColor, color.get_red_direct(), color.get_green_direct(), color.get_blue_direct(),
                color.get_alpha_direct());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font.atlas_texture_id);

    std::vector<float> vertices;

    vertices.reserve(total * 4 * 5);

    std::vector<unsigned int> indices;
    indices.reserve(total * 6);

    unsigned int vertex_offset = 0;

    float another_y = y;

    for (auto line : lines) {
        float c_x = x;
        float c_y = another_y;

        hb_buffer_t *buf = hb_buffer_create();
        hb_buffer_add_utf8(buf, line.c_str(), -1, 0, -1);
        hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
        hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
        hb_buffer_set_language(buf, hb_language_from_string("en", -1));

        hb_shape(font.hb_font, buf, nullptr, 0);

        unsigned int glyph_count;
        hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
        hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

        for (int i = 0; i < glyph_count; i++)
        {
            glUniform1i(charIndex, i);

            const hb_codepoint_t glyph_index = glyph_info[i].codepoint;
            const auto it = font.characters_map.find(glyph_index);

            if (it == font.characters_map.end()) continue;

            auto character = it->second;

            const float xpos = c_x + (glyph_pos[i].x_offset / 64.0f) * f_scale + character.bearing.x * f_scale;
            const float ypos = c_y - (character.bearing.y - (glyph_pos[i].y_offset / 64.0f)) * f_scale;

            glUniform2f(position, xpos, ypos);

            const float w = character.size.x * f_scale;
            const float h = character.size.y * f_scale;

            const float tex_x = static_cast<float>(character.atlas_offset.x) / font_loader::ATLAS_WIDTH;
            const float tex_y = static_cast<float>(character.atlas_offset.y) / font_loader::ATLAS_HEIGHT;
            const float tex_w = static_cast<float>(character.size.x) / font_loader::ATLAS_WIDTH;
            const float tex_h = static_cast<float>(character.size.y) / font_loader::ATLAS_HEIGHT;

            vertices.insert(vertices.end(), {xpos,     ypos,     tex_x,         tex_y        ,  static_cast<float>(i)});
            vertices.insert(vertices.end(), {xpos + w, ypos,     tex_x + tex_w, tex_y        ,  static_cast<float>(i)});
            vertices.insert(vertices.end(), {xpos + w, ypos + h, tex_x + tex_w, tex_y + tex_h,  static_cast<float>(i)});
            vertices.insert(vertices.end(), {xpos,     ypos + h, tex_x,         tex_y + tex_h,  static_cast<float>(i)});


            indices.insert(indices.end(), {vertex_offset, vertex_offset + 1, vertex_offset + 2});
            indices.insert(indices.end(), {vertex_offset, vertex_offset + 2, vertex_offset + 3});

            c_x += (glyph_pos[i].x_advance / 64.0f) * f_scale;
            c_y += (glyph_pos[i].y_advance / 64.0f) * f_scale;

            vertex_offset += 4;
        }

        another_y += f_scale * 64.0f;
        hb_buffer_destroy(buf);
    }

    if (!vertices.empty())
    {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
