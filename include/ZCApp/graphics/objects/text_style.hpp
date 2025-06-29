//
// Created by Damian Netter on 29/06/2025.
//

#pragma once
#include <utility>

#include "ZCApp/graphics/utils/colour.hpp"

namespace zc_app
{
    struct text_style
    {
        bool outline_enable = false;
        bool shadow_enable = false;
        bool rainbow_enable = false;
        bool glow_enable = false;

        std::pair<float, float> shadow_offset = { 0.0F, 0.0F };

        colour text_color = colour(255, 255, 255, 255);
        colour shadow_color = colour(0, 0, 0, 128);
        colour outline_color = colour(0, 0, 0, 255);
        colour glow_color = colour(255, 255, 0, 204);

        float outline_width = 1.0F;
        float rainbow_variation = 0.3F;
        float rainbow_saturation = 0.5F;
        float rainbow_speed = 0.1F;

        float glow_radius = 0.25F;
        float glow_intensity = 1.0F;

        float end_text_scale = 1.0F;
        float begin_text_scale = 1.0F;
        float speed_text_scale = 1.0F;
    };
}
