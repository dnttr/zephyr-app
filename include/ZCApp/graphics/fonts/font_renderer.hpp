//
// Created by Damian Netter on 27/06/2025.
//

#pragma once

#include "font_manager.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCApp/graphics/utils/colour.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"
#include <hb.h>

namespace zc_app
{
    class colour;

    class font_renderer
    {
        inline static GLuint ebo{};
        inline static GLuint vbo{};
        inline static GLuint vao{};

        inline static GLuint program = -1;

        inline static GLint projection;
        inline static GLint fontAtlas;
        inline static GLint textColor;
        inline static GLint outlineColor;
        inline static GLint shadowColor;
        inline static GLint smoothing;
        inline static GLint outlineWidth;
        inline static GLint shadowOffset;

        inline static bool setup_x;

        static void setup();
    public:

        static void render(font_manager::font &font, const std::string &text, float x, float y, float f_scale, const colour &color);
    };
}