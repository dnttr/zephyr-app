//
// Created by Damian Netter on 23/06/2025.
//

#include "ZCApp/graphics/window.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

#include "ZCApp/graphics/renderer.hpp"

namespace zc_app
{
    static renderer *current_renderer = nullptr;
    static display_config current_config;

    static void calculate_display()
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

    static void destroy_callback()
    {
        if (current_renderer)
        {
            current_renderer->destroy();

            delete current_renderer;
        }
    }

    static void render_callback()
    {
        if (current_renderer)
        {
            current_renderer->render();
        }
    }

    static void initialize_callback()
    {
        if (current_renderer)
        {
            current_renderer->initialize();

            calculate_display();
        }
    }

    static void reshape_callback(const int width, const int height)
    {
        if (current_renderer)
        {
            current_config.window_width = width;
            current_config.window_height = height;

            calculate_display();

            current_renderer->reshape(width, height);
        }
    }

    static void update_callback()
    {
        if (current_renderer)
        {
            current_renderer->update();
        }
    }
    display_config &window::get_current_display_config()
    {
        return current_config;
    }

    void window::allocate(const std::string &title, const int x, const int y, const display_config &config)
    {
        current_renderer = new renderer();

        m_callback_handle = {
            .on_exit_callback = destroy_callback,
            .on_render_callback = render_callback,
            .on_reshape_callback = reshape_callback,
            .on_init_callback = initialize_callback,
            .on_update_callback = update_callback
        };

        if (config.window_width == 0 || config.window_height == 0)
        {
            throw std::invalid_argument("Window width and height must be greater than zero.");
        }

        zcg_window_args_t args = {
            .title = title.c_str(),
            .x = x,
            .y = y,
            .width = static_cast<int>(config.window_width),
            .height = static_cast<int>(config.window_height),
        };

        current_config = config;

        m_window = zcg_allocate(&args, &m_callback_handle);

        calculate_display();
    }

    void window::run() const
    {
        if (!current_renderer || !m_window)
        {
            throw std::runtime_error("Renderer is not initialized.");
        }

        if (m_callback_handle.on_init_callback == nullptr || m_callback_handle.on_render_callback == nullptr ||
            m_callback_handle.on_reshape_callback == nullptr || m_callback_handle.on_update_callback == nullptr || m_callback_handle.on_exit_callback == nullptr)
        {
            throw std::runtime_error("Initialization callback is not set.");
        }

        zcg_run(m_window);
    }
}
