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

#include "ZCApp/graphics/framebuffer.hpp"
#include "ZCApp/graphics/fonts/font_loader.hpp"
#include "ZCApp/graphics/fonts/font_manager.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"
#include <glm/gtx/string_cast.hpp>
#include <random>

#include "ZCApp/graphics/objects/background.hpp"
#include "ZCApp/graphics/objects/blur.hpp"

namespace zc_app
{
    blur b;

    text tx;

    void renderer::initialize()
    {
        font_manager::initialize();
        font_loader::load_font("Roboto-Regular", 128);
        font_manager::font font = font_manager::get_font("Roboto-Regular");
        text_style style;
        style.text_size_magnification = 0.1;
        style.rainbow_enable = true;
        style.rainbow_saturation = 0.2F;
        style.rainbow_variation = 0.1F;
        style.rainbow_speed = 2.0F;
        style.shadow_enable = true;
        style.shadow_offset = {4.0F, 4.0F};

        tx.initialize(
            "Hello, World! This is a test of the text rendering system.\nIt should be able to handle long lines and multiple lines of text without any issues.",
            container{100.0, 100.0}, font, style);

        b.setup();

        glClearColor(0.2F, 0.2F, 0.2F, 1.0F);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_MULTISAMPLE);

        //  glGenBuffers(1, &framebuffer);
    }

    void renderer::render() const
    {
        const auto &cfg = perspective_util::get_current_display_config();

        glViewport(static_cast<GLint>(cfg.viewport_x), static_cast<GLint>(cfg.viewport_y),
                   static_cast<GLsizei>(cfg.viewport_width),
                   static_cast<GLsizei>(cfg.viewport_height));

        b.render(cfg.window_width , cfg.window_height);
    }


    void renderer::reshape(const int width, const int height)
    {
        const auto &cfg = perspective_util::get_current_display_config();

        b.reshape(cfg.window_width , cfg.window_height);
    }

    void renderer::update()
    {
    }

    void renderer::destroy()
    {
    }

    void renderer::run()
    {
    }
}
