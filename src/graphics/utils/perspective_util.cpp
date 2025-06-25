//
// Created by Damian Netter on 24/06/2025.
//

#include "ZCApp/graphics/utils/perspective_util.hpp"

#include <algorithm>
#include <stdexcept>

namespace zc_app
{
    zcg_kit::display::config perspective_util::current_config;
    glm::mat4 perspective_util::projection_matrix = glm::mat4(1.0f);

    zcg_kit::display::config &perspective_util::get_current_display_config()
    {
        return current_config;
    }

    void perspective_util::set_current_display_config(const zcg_kit::display::config &config)
    {
        current_config = config;
    }

    std::pair<float, float> perspective_util::get_effective_virtual_dimensions()
    {
        const float effective_virtual_width = current_config.virtual_width / current_config.dpi_scale;
        const float effective_virtual_height = current_config.virtual_height / current_config.dpi_scale;

        return {effective_virtual_width, effective_virtual_height};
    }

    glm::mat4 perspective_util::get_projection()
    {
        const auto [effective_virtual_width, effective_virtual_height] = get_effective_virtual_dimensions();
        return glm::ortho(0.0f, effective_virtual_width, effective_virtual_height, 0.0f, -1.0f, 1.0f);
    }

    void perspective_util::calculate_viewport()
    {
        if (current_config.maintain_aspect_ratio)
        {
            current_config.dpi_scale = 2.0F;

            const auto f_w_width = static_cast<float>(current_config.window_width);
            const auto f_w_height = static_cast<float>(current_config.window_height);

            const float effective_virtual_width = current_config.virtual_width / current_config.dpi_scale;
            const float effective_virtual_height = current_config.virtual_height / current_config.dpi_scale;

            const float scale_x = f_w_width / effective_virtual_width;
            const float scale_y = f_w_height / effective_virtual_height;

            current_config.scale = std::min(scale_x, scale_y);

            current_config.viewport_width = effective_virtual_width * current_config.scale;
            current_config.viewport_height = effective_virtual_height * current_config.scale;

            current_config.viewport_x = (f_w_width - current_config.viewport_width) * 0.5F;
            current_config.viewport_y = (f_w_height - current_config.viewport_height) * 0.5F;

            projection_matrix = get_projection();
        }
        else
        {
            throw std::runtime_error("disabled aspect ratio is not implemented yet.");
        }
    }

    const float *perspective_util::get_projection_matrix()
    {
        return glm::value_ptr(projection_matrix);
    }
}
