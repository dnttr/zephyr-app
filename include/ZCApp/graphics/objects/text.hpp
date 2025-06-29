//
// Created by Damian Netter on 28/06/2025.
//

#pragma once

#include <sstream>
#include <utility>
#include <vector>
#include <OpenGL/gl3.h>

#include "text_style.hpp"
#include "ZCApp/graphics/fonts/font_renderer.hpp"
#include "ZCApp/graphics/utils/string_util.hpp"

namespace zc_app
{
    class text
    {
        inline static bool is_shader_initialized = false;

        font_renderer::properties properties{};
        font_renderer::geometry geom{};

        font_renderer::transform_properties transform_props{};
        font_renderer::text_properties text_props{};

        text_style style{};

        bool is_initialized = false;

        static void initialize_shader()
        {
            font_renderer::build_shader();

            is_shader_initialized = true;
        }
    public:

        void initialize(const std::string &str, font_manager::font font, text_style &style)
        {
            const auto [lines, size] = get_lines(str);

            if (!is_shader_initialized)
            {
                initialize_shader();
            }

            geom = font_renderer::build_geometry();

            properties.font = std::move(font);
            properties.x = 20;
            properties.y = 20;
            properties.text_scale = 1.0f;
            properties.lines = lines;
            properties.total_characters = size;
            properties.startTime = std::chrono::steady_clock::now();

            font_renderer::set_parameters(this->style, text_props, transform_props, properties);
        }

        void render()
        {
            glUseProgram(font_renderer::get_program());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, properties.font.atlas_texture_id);

            font_renderer::render(properties, text_props, transform_props, geom);

            glBindTexture(GL_TEXTURE_2D, 0);
            glUseProgram(0);
        }
    };
}
