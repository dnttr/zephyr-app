//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

#include <string>

#include "ZCGKit/display.hpp"
#include "ZCGKit/zcg_interface.h"

namespace zc_app
{
    class window
    {
        zcg_window_t *m_window = nullptr;

        zcg_callback_handle m_callback_handle = {};
    public:
        void allocate(const std::string &title, int x, int y, const zc_kit::display::config &cfg);

        void run() const;
    };
}
