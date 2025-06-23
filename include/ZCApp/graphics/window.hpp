//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

#include <string>

#include "renderer.hpp"
#include "types.hpp"
#include "ZCGKit/zcg_interface.h"

namespace zc_app
{
    class window
    {
        zcg_window_t *m_window = nullptr;

        zcg_callback_handle m_callback_handle = {};
    public:
        static display_config& get_current_display_config();

        void allocate(const std::string &title, int x, int y, const display_config &config);

        void run() const;
    };
}
