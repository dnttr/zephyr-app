//
// Created by Damian Netter on 23/06/2025.
//

#include "ZCApp/graphics/window.hpp"

#include <stdexcept>
#include <string>

#include "ZCApp/graphics/renderer.hpp"

namespace zc_app
{
    static renderer *current_renderer = nullptr;

    static void destroy_callback()
    {
        if (current_renderer)
        {
            current_renderer->destroy();
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
        }
    }

    static void reshape_callback(const int width, const int height)
    {
        if (current_renderer)
        {
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

    void window::build(renderer *g_renderer)
    {
        m_renderer = g_renderer;
    }

    void window::allocate(const std::string &title, const int x, const int y, const int width, const int height)
    {
        if (!m_renderer)
        {
            throw std::runtime_error("Renderer is not set. Please build the renderer first.");
        }

        zcg_callback_handle handle = {
            .on_exit_callback = destroy_callback,
            .on_render_callback = render_callback,
            .on_reshape_callback = reshape_callback,
            .on_init_callback = initialize_callback,
            .on_update_callback = update_callback
        };

        zcg_window_args_t args = {
            .title = title.c_str(),
            .x = x,
            .y = y,
            .width = width,
            .height = height
        };

        current_renderer = m_renderer;

        m_window = zcg_allocate(&args, &handle);
    }

    void window::run() const
    {
        if (!(m_renderer || m_window))
        {
            throw std::runtime_error("Renderer is not initialized.");
        }



        zcg_run(m_window);
    }
}
