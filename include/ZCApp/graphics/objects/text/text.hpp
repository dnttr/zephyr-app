//
// Created by Damian Netter on 28/06/2025.
//

#pragma once

#include <sstream>

#include "ZCApp/graphics/fonts/font_renderer.hpp"
#include "ZCApp/graphics/objects/shapes/container.hpp"
#include "ZCApp/graphics/objects/text/text_style.hpp"

namespace zc_app
{
    class text
    {
        inline static bool is_shader_initialized = false;

        font_renderer::properties properties{};
        font_renderer::geometry geom{};

        font_renderer::transform_properties transform_props{};
        font_renderer::text_properties text_props{};

        container dimension{};
        text_style style{};

        bool is_initialized = false;

        std::string str;

        static void initialize_shader();

    public:
        ~text();

        void initialize(const std::string &str, container container, font_manager::font font, const text_style &style);

        void render();

        void set_properties_position(float x, float y);
        void increment_text_position(float x, float y);

        void set_color(colour color);

        [[nodiscard]] colour get_color() const;

        void set_glow_intensity(float value);

        void set_glow_color(const colour &color);

        void set_glow_enable(bool enable);

        void set_glow_radius(float radius);

        [[nodiscard]] bool is_glow_enabled() const;

        [[nodiscard]] colour get_glow_color() const;

        [[nodiscard]] float get_glow_intensity() const;

        [[nodiscard]] float get_glow_radius() const;

        void set_x(float x);

        void set_y(float y);

        [[nodiscard]] std::pair<float, float> get_text_position() const;

        std::string get_text();

        text_style &get_style();

        void set_text(const std::string &str);
    };
}
