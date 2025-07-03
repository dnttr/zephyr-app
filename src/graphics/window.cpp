//
// Created by Damian Netter on 23/06/2025.
//

#include "ZCApp/graphics/window.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

#include "ZCApp/graphics/renderer.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

namespace zc_app
{
    static renderer *current_renderer = nullptr;

    void mouse_move_callback(zcg_mouse_pos_t mouse_pos)
    {
        if (current_renderer)
        {
            current_renderer->on_mouse_move(mouse_pos);
        }
    }

    void mouse_enter_callback(zcg_mouse_pos_t mouse_pos)
    {
        if (current_renderer)
        {
            current_renderer->on_mouse_enter(mouse_pos);
        }
    }

    void mouse_exit_callback(zcg_mouse_pos_t mouse_pos)
    {
        if (current_renderer)
        {
            current_renderer->on_mouse_exit(mouse_pos);
        }
    }

    void mouse_down_callback(zcg_mouse_pos_t mouse_pos, const int button)
    {
        if (current_renderer)
        {
            current_renderer->on_mouse_down(mouse_pos, button);
        }
    }

    void mouse_up_callback(zcg_mouse_pos_t mouse_pos, const int button)
    {
        if (current_renderer)
        {
            current_renderer->on_mouse_up(mouse_pos, button);
        }
    }

    void scroll_callback(zcg_scroll_event_t scroll_event)
    {
        if (current_renderer)
        {
            current_renderer->on_scroll(scroll_event);
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

            perspective_util::calculate_viewport();
        }
    }

    static void reshape_callback(const int width, const int height)
    {
        if (current_renderer)
        {
            auto &cfg = perspective_util::get_current_display_config();

            cfg.window_width = width;
            cfg.window_height = height;

            perspective_util::calculate_viewport();

            current_renderer->reshape(width, height);
        }
    }

    static void key_down_callback(zcg_key_event_t key_event)
    {
        if (current_renderer)
        {
            current_renderer->on_key_down(key_event);
        }
    }

    static void key_up_callback(zcg_key_event_t key_event)
    {
        if (current_renderer)
        {
            current_renderer->on_key_up(key_event);
        }
    }

    static void update_callback()
    {
        if (current_renderer)
        {
            current_renderer->update();
        }
    }

    void window::allocate(const std::string &title, const int x, const int y, const zcg_kit::display::config &cfg)
    {
        current_renderer = new renderer();

        m_callback_handle = {
            .on_exit_callback = destroy_callback,
            .on_render_callback = render_callback,
            .on_reshape_callback = reshape_callback,
            .on_init_callback = initialize_callback,
            .on_update_callback = update_callback,
            .on_mouse_move_callback = mouse_move_callback,
            .on_mouse_enter_callback = mouse_enter_callback,
            .on_mouse_exit_callback = mouse_exit_callback,
            .on_mouse_down_callback = mouse_down_callback,
            .on_mouse_up_callback = mouse_up_callback,
            .on_scroll_callback = scroll_callback,
            .on_key_down_callback = key_down_callback,
            .on_key_up_callback = key_up_callback,
        };

        if (cfg.window_width == 0 || cfg.window_height == 0)
        {
            throw std::invalid_argument("Window width and height must be greater than zero.");
        }

        perspective_util::calculate_viewport();

        const auto min_width_factor = static_cast<unsigned int>(2 * cfg.scale * cfg.dpi_scale);
        const auto min_height_factor = static_cast<unsigned int>(2 * cfg.scale * cfg.dpi_scale);

        zcg_window_args_t args = {
            .title = title.c_str(),
            .x = x,
            .y = y,
            .width = static_cast<int>(cfg.window_width),
            .height = static_cast<int>(cfg.window_height),
            .min_width = static_cast<int>(cfg.window_width / min_width_factor),
            .min_height = static_cast<int>(cfg.window_height / min_height_factor),
            .max_width = 0,
            .max_height = 0,
        };

        perspective_util::set_current_display_config(cfg);

        m_window = zcg_allocate(&args, &m_callback_handle);
    }

    void window::run() const
    {
        if (!current_renderer || !m_window)
        {
            throw std::runtime_error("Renderer is not initialized.");
        }

        if (m_callback_handle.on_init_callback == nullptr || m_callback_handle.on_render_callback == nullptr ||
            m_callback_handle.on_reshape_callback == nullptr || m_callback_handle.on_update_callback == nullptr ||
            m_callback_handle.on_exit_callback == nullptr)
        {
            throw std::runtime_error("Initialization callback is not set.");
        }

        zcg_run(m_window);
    }
}
