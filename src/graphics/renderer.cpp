//
// Created by Damian Netter on 23/06/2025.
//

//
//
// Created by Damian Netter on 14/05/2025.
//

#include "ZCApp/graphics/renderer.hpp"

#include <iostream>
#include <OpenGL/gl3.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <random>
#include <glm/gtx/string_cast.hpp>

#include "ZCApp/app/scenes/main/main_scene.hpp"
#include "ZCApp/graphics/fonts/font_loader.hpp"
#include "ZCApp/graphics/fonts/font_manager.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

namespace zc_app
{
    text tx;

    main_scene ms;

    void renderer::initialize()
    {
        font_manager::initialize();
        font_loader::load_font("Roboto-Regular", 128);
        font_loader::load_font("Roboto-Medium", 128);
        font_manager::font font = font_manager::get_font("Roboto-Regular");


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_MULTISAMPLE);

        auto [width, height] = perspective_util::get_effective_virtual_dimensions();

        ms.initialize(width / 2, height / 2);
    }

    void renderer::render() const
    {
        const auto &cfg = perspective_util::get_current_display_config();

        glViewport(static_cast<GLint>(cfg.viewport_x), static_cast<GLint>(cfg.viewport_y),
                   static_cast<GLsizei>(cfg.viewport_width),
                   static_cast<GLsizei>(cfg.viewport_height));

        ms.render();
    }


    void renderer::reshape(const int width, const int height)
    {
        ms.resize(width, height);
    }

    void renderer::update()
    {
        ms.update();
    }

    void renderer::destroy()
    {
        ms.destroy();
    }

    void renderer::run()
    {
    }

    void renderer::on_mouse_up(const zcg_mouse_pos_t &mouse_pos, const int button)
    {
    }

    void renderer::on_mouse_down(const zcg_mouse_pos_t &mouse_pos, int button)
    {
    }

    void renderer::on_mouse_move(const zcg_mouse_pos_t &mouse_pos)
    {
    }

    void renderer::on_mouse_enter(const zcg_mouse_pos_t &mouse_pos)
    {
    }

    void renderer::on_mouse_exit(const zcg_mouse_pos_t &mouse_pos)
    {
    }

    void renderer::on_scroll(const zcg_scroll_event_t &scroll_event)
    {
        ms.scroll(scroll_event);
    }
}
