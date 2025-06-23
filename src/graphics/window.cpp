//
// Created by Damian Netter on 23/06/2025.
//

#include "ZCApp/graphics/window.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <OpenGL/gl3.h>

#include "ZCApp/graphics/renderer.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

namespace zc_app
{
    static renderer *current_renderer = nullptr;
    static display_config current_config;

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
            glViewport(static_cast<GLint>(current_config.viewport_x),
                static_cast<GLint>(current_config.viewport_y),
                static_cast<GLsizei>(current_config.viewport_width),
                static_cast<GLsizei>(current_config.viewport_height));

            current_renderer->render();
        }
    }

    static void initialize_callback()
    {
        if (current_renderer)
        {
            current_renderer->initialize();

            perspective_util::calculate_viewport(current_config);
        }
    }

    static void reshape_callback(const int width, const int height)
    {
        if (current_renderer)
        {
            current_config.window_width = width;
            current_config.window_height = height;

            perspective_util::calculate_viewport(current_config);

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

        perspective_util::calculate_viewport(current_config);
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
