//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

#include <string>
#include <glm/fwd.hpp>

#include "renderer.hpp"
#include "types.hpp"
#include "utils/perspective_util.hpp"
#include "ZCGKit/zcg_interface.h"

namespace zc_app
{
    class window
    {
        zcg_window_t *m_window = nullptr;

        zcg_callback_handle m_callback_handle = {};

        static glm::mat4 projection_matrix;
    public:
        static display_config &get_current_display_config();

        static const float *get_projection_matrix()
        {
            const auto &config = get_current_display_config();
            projection_matrix = perspective_util::get_projection(config.virtual_width, config.virtual_height);
            return glm::value_ptr(projection_matrix);
        }

        void allocate(const std::string &title, int x, int y, const display_config &config);

        void run() const;
    };
}
