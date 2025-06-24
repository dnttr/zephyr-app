//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ZCGKit/display.hpp"

namespace zc_app
{
    class perspective_util
    {
        static glm::mat4 projection_matrix;
        static zcg_kit::display::config current_config;

    public:
        static zcg_kit::display::config &get_current_display_config();

        static void set_current_display_config(const zcg_kit::display::config &config);

        static glm::mat4 get_projection();

        static void calculate_viewport();

        static const float *get_projection_matrix();
    };
}
