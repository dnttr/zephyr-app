//
// Created by Damian Netter on 03/07/2025.
//

#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <OpenGL/gl3.h>

namespace zc_app
{
    class scissor
    {
    public:
        struct info
        {
            int x, y;
            int width, height;

            bool enabled;
            bool disabled;
        };

        struct options
        {
            bool enable = true;
            bool disable = true;
            bool flip = true;
            bool clamp = true;
            bool normalize = false;
            bool scale = true;
            bool additional_scaling = false;

            int viewport_x = -1, viewport_y = -1, viewport_width = -1, viewport_height = -1;
        };

        static info glScissorOp(std::function<void()> func, const float x, const float y, const float width, const float height,
                                const options &options)
        {
            int viewport_x = options.viewport_x;
            int viewport_y = options.viewport_y;
            int viewport_width = options.viewport_width;
            int viewport_height = options.viewport_height;

            if (viewport_x == -1 || viewport_y == -1 || viewport_width == -1 || viewport_height == -1)
            {
                GLint viewport[4];
                glGetIntegerv(GL_VIEWPORT, viewport);

                if (viewport_x == -1)
                {
                    viewport_x = viewport[0];
                }

                if (viewport_y == -1)
                {
                    viewport_y = viewport[1];
                }

                if (viewport_width == -1)
                {
                    viewport_width = viewport[2];
                }

                if (viewport_height == -1)
                {
                    viewport_height = viewport[3];
                }
            }

            float scissor_x = x;
            float scissor_y = y;
            float scissor_width = width;
            float scissor_height = height;

            if (options.scale)
            {
                const float scale = (perspective_util::get_current_display_config().dpi_scale *
                    perspective_util::get_current_display_config().scale) / 2;

                scissor_x *= scale;
                scissor_y *= scale;
                scissor_width *= scale;
                scissor_height *= scale;
            }

            if (options.additional_scaling)
            {
                scissor_x /= 2;
                scissor_y /= 2;
                scissor_width /= 2;
                scissor_height /= 2;
            }

            if (options.normalize)
            {
                scissor_x = x * viewport_width;
                scissor_y = y * viewport_height;
                scissor_width = width * viewport_width;
                scissor_height = height * viewport_height;
            }

            if (options.flip)
            {
                scissor_y = viewport_height - scissor_y - scissor_height;
            }

            scissor_x += viewport_x;
            scissor_y += viewport_y;

            if (options.clamp)
            {
                const auto right = std::min(scissor_x + scissor_width, static_cast<float>(viewport_x + viewport_width));
                const auto top = std::min(scissor_y + scissor_height, static_cast<float>(viewport_y + viewport_height));

                scissor_x = std::max(scissor_x, static_cast<float>(viewport_x));
                scissor_y = std::max(scissor_y, static_cast<float>(viewport_y));
                scissor_width = std::max(right - scissor_x, 0.0f);
                scissor_height = std::max(top - scissor_y, 0.0f);
            }

            const auto final_x = static_cast<int>(std::round(scissor_x));
            const auto final_y = static_cast<int>(std::round(scissor_y));
            const auto final_width = static_cast<int>(std::round(scissor_width));
            const auto final_height = static_cast<int>(std::round(scissor_height));

            if (options.enable)
            {
                glEnable(GL_SCISSOR_TEST);
            }

            glScissor(final_x, final_y, final_width, final_height);
            func();

            if (options.disable)
            {
                glDisable(GL_SCISSOR_TEST);
            }

            return {final_x, final_y, final_width, final_height, options.enable, options.disable};
        }

        static info glScissorOp(std::function<void()> const &callback, const container container, const options &op)
        {
            return glScissorOp(callback, container.get_x(), container.get_y(), container.get_width(), container.get_height(), op);
        }
    };
}
