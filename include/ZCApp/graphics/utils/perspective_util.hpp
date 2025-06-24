//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace zc_app
{
    class perspective_util
    {
        static glm::mat4 projection_matrix; //    glm::mat4 window::projection_matrix = glm::mat4(1.0f);
        static display_config current_config;
    public:

        static display_config &get_current_display_config()
        {
            return current_config;
        }

        static void set_current_display_config(const display_config &config)
        {
            current_config = config;
        }

        static glm::mat4 get_projection(const float width, const float height)
        {
            return glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
        }

        static void calculate_viewport()
        {
            if (current_config.maintain_aspect_ratio)
            {
                const auto f_w_width = static_cast<float>(current_config.window_width);
                const auto f_w_height = static_cast<float>(current_config.window_height);

                const float scale_x = f_w_width / current_config.virtual_width;
                const float scale_y = f_w_height / current_config.virtual_height;

                current_config.scale = std::min(scale_x, scale_y) * current_config.dpi_scale;

                current_config.viewport_width = current_config.virtual_width * current_config.scale;
                current_config.viewport_height = current_config.virtual_height * current_config.scale;

                current_config.viewport_x = (f_w_width - current_config.viewport_width) * 0.5F;
                current_config.viewport_y = (f_w_height - current_config.viewport_height) * 0.5F;
            }
            else
            {
                throw std::runtime_error("Aspect ratio maintenance is not implemented yet.");
            }
        }

        static const float *get_projection_matrix()
        {
            const auto &config = get_current_display_config();
            projection_matrix = perspective_util::get_projection(config.virtual_width, config.virtual_height);
            return glm::value_ptr(projection_matrix);
        }
    };
}
