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

    void text::initialize(const std::string &str, const container container, font_manager::font font,
                          const text_style &style)
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
        this->str = str;

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

    void text::set_properties_position(float x, float y)
    {
        text_props.text_position.x = x;
        text_props.text_position.y = y;

        properties.x = x;
        properties.y = y;

        font_renderer::set_parameters(this->style, text_props, transform_props, properties);
    }

    void text::increment_text_position(const float x, const float y)
    {
        text_props.text_position.x += x;
        text_props.text_position.y += y;

        properties.x += x;
        properties.y += y;

        font_renderer::set_parameters(this->style, text_props, transform_props, properties);
    }

    void text::set_color(const colour color)
    {
        style.text_color = color;
        text_props.text_color = color.get_vec4();

        font_renderer::set_parameters(style, text_props, transform_props, properties);
    }

    colour text::get_color() const
    {
        return style.text_color;
    }

    void text::set_glow_intensity(float value)
    {
        style.glow_intensity = value;
        text_props.text_glow_intensity = value;

        font_renderer::set_parameters(style, text_props, transform_props, properties);
    }

    void text::set_glow_color(const colour &color)
    {
        style.glow_color = color;
        text_props.text_glow_color = color.get_vec4();

        font_renderer::set_parameters(style, text_props, transform_props, properties);
    }

    void text::set_glow_enable(const bool enable)
    {
        style.glow_enable = enable;
        text_props.text_glow_enable = enable;

        font_renderer::set_parameters(style, text_props, transform_props, properties);
    }

    void text::set_glow_radius(const float radius)
    {
        style.glow_radius = radius;
        text_props.text_glow_radius = radius;

        font_renderer::set_parameters(style, text_props, transform_props, properties);
    }

    bool text::is_glow_enabled() const
    {
        return style.glow_enable;
    }

    colour text::get_glow_color() const
    {
        return style.glow_color;
    }

    float text::get_glow_intensity() const
    {
        return style.glow_intensity;
    }

    float text::get_glow_radius() const
    {
        return style.glow_radius;
    }

    void text::set_x(const float x)
    {
        text_props.text_position.x = x;
        properties.x = x;

        font_renderer::set_parameters(this->style, text_props, transform_props, properties);
    }

    void text::set_y(const float y)
    {
        text_props.text_position.y = y;
        properties.y = y;

        font_renderer::set_parameters(this->style, text_props, transform_props, properties);
    }

    std::pair<float, float> text::get_text_position() const
    {
        return {properties.x, properties.y};
    }

    std::string text::get_text()
    {
        return str;
    }

    text_style &text::get_style()
    {
        return style;
    }

    void text::set_text(const std::string &str)
    {
        const auto [lines, size] = string_util::get_lines(str);

        properties.lines = lines;
        properties.total_characters = size;

        font_renderer::set_parameters(this->style, text_props, transform_props, properties);
    }
}
