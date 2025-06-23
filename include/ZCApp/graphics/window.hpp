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
    public:

        void allocate(const std::string &title, int x, int y, int width, int height);

        void build(renderer *g_renderer);
        void run() const;
    };
}
