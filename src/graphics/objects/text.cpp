//
// Created by Damian Netter on 28/06/2025.
//

#include "ZCApp/graphics/objects/text/text.hpp"

#include "ZCApp/graphics/utils/string_util.hpp"
#include "ZCGKit/external.hpp"

namespace zc_app
{
    void text::initialize_shader()
    {
        font_renderer::build_shader();

        is_shader_initialized = true;
    }

    void text::initialize(const std::string &str, const container container, font_manager::font font, const text_style &style)
    {
        const auto [lines, size] = string_util::get_lines(str);

        if (!is_shader_initialized)
        {
            initialize_shader();
        }

        dimension = container;

        geom = font_renderer::build_geometry();

        properties.font = std::move(font);
        properties.x = dimension.get_x();
        properties.y = dimension.get_y();
        properties.text_scale = 1.0f;
        properties.lines = lines;
        properties.total_characters = size;
        properties.startTime = std::chrono::steady_clock::now();

        this->style = style;

        font_renderer::set_parameters(this->style, text_props, transform_props, properties);

        is_initialized = true;
    }

    void text::render()
    {
        glUseProgram(font_renderer::get_program());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, properties.font.atlas_texture_id);

        font_renderer::render(properties, text_props, transform_props, geom);

        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

    text::~text()
    {
        if (is_initialized && zcg_kit::external::is_context_valid())
        {
            glDeleteBuffers(1, &geom.vbo);
            glDeleteBuffers(1, &geom.ebo);
            glDeleteVertexArrays(1, &geom.vao);

            is_initialized = false;
        }
    }
}
