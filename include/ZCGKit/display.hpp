//
// Created by Damian Netter on 23/06/2025.
//
#pragma once

namespace zc_kit
{
    class display
    {
    public:
        struct config
        {
            unsigned int window_width = 0;
            unsigned int window_height = 0;

            float virtual_width = 1920.0F; // probably needs adjusting. have to look into it, especially on different devices
            float virtual_height = 1080.0F;

            float viewport_x = 0.0F;
            float viewport_y = 0.0F;

            float viewport_width = 0.0F;
            float viewport_height = 0.0F;

            bool maintain_aspect_ratio = true;

            float scale = 1.0F;
            float dpi_scale = 1.0F; //retina specific, needs adjustment with ZGC
        };
    };
}
