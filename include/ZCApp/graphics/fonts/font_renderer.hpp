//
// Created by Damian Netter on 27/06/2025.
//

#pragma once

#include "font_manager.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCApp/graphics/utils/colour.hpp"

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
        inline static GLint useEffects;
        inline static GLint totalChars;
        inline static GLint charIndex;
        inline static GLint variation;
        inline static GLint position;
        inline static GLint saturation;
        inline static GLint time_u;
        inline static GLint speed;
        inline static GLint glowRadius;
        inline static GLint glowIntensity;
        inline static GLint glowColor;
        inline static GLint useGlow;

        inline static bool setup_x;

        static void setup(const std::string &name);
    public:

        static void render(font_manager::font &font, const std::string &text, float x, float y, float f_scale, const colour &color);
    };
}