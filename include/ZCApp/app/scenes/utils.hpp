//
// Created by Damian Netter on 06/07/2025.
//

#pragma once

#include "ZCApp/graphics/objects/shapes/container.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"
#include "ZCGKit/zcg_interface.h"

namespace zc_app
{
    [[nodiscard]] inline bool is_in_area(const container &container, const float x, const float y)
    {
        const float scale = perspective_util::get_current_display_config().scale / 2;

        const float mouse_x = x / scale;
        const float mouse_y = y / scale;

        if (mouse_x < container.get_x() ||
            mouse_x > container.get_x() + container.get_width() ||
            mouse_y < container.get_y() ||
            mouse_y > container.get_y() + container.get_height())
        {
            return false;
        }

        return true;
    }

    inline bool is_in_area(const container &container, const zcg_mouse_pos_t &pos)
    {
        return is_in_area(container, pos.x, pos.y);
    }

    inline bool is_in_area(const container &container, const zcg_scroll_event_t &scroll_event)
    {
        return is_in_area(container, scroll_event.mouse_x, scroll_event.mouse_y);
    }
}
