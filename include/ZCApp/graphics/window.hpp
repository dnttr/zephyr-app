//
// Created by Damian Netter on 23/06/2025.
//

#pragma once
#include <string>

#include "renderer.hpp"
#include "ZCGKit/zcg_interface.h"

namespace zc_app
{
    class window
    {
        zcg_window_t *m_window = nullptr;
        renderer *m_renderer = nullptr;
        zcg_callback_handle m_callback_handle = {};
    public:
        struct display_config
        {
            int window_width = 0;
            int window_height = 0;

            float virtual_width = 1920.0F;
            float virtual_height = 1080.0F;

            float viewport_x = 0.0F;
            float viewport_y = 0.0F;

            float viewport_width = 0.0F;
            float viewport_height = 0.0F;

            bool maintain_aspect_ratio = true;

            float scale = 1.0F;
            float dpi_scale = 1.0F;
        };
        void allocate(const std::string &title, int x, int y, int width, int height);

        void build(renderer *g_renderer);
        void run() const;
    };
}
