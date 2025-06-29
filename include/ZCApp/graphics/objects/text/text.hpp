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

        static void initialize_shader();

    public:

        void initialize(const std::string &str, zc_app::container container, font_manager::font font, const text_style &style);

        void render();

        ~text();
    };
}
