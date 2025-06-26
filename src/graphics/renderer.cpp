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

#include "ZCApp/graphics/shapes/rectangle.hpp"
#include "ZCApp/graphics/textures/fan_texture.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

namespace zc_app
{
    rectangle rect(container(0, 0, 100.0F, 100.0F), colour(255, 0, 0, 255), 20.0F);
    fan_texture fan("test.png", container(200.0f, 200.0f), 0.5F, 128);

    void renderer::initialize()
    {
        glClearColor(0.2F, 0.2F, 0.2F, 1.0F);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_MULTISAMPLE);

      //  glGenBuffers(1, &framebuffer);
    }

    void renderer::render() const
    {
        glClear(GL_COLOR_BUFFER_BIT);

        const auto &cfg = perspective_util::get_current_display_config();
        glViewport(static_cast<GLint>(cfg.viewport_x),
            static_cast<GLint>(cfg.viewport_y),
            static_cast<GLsizei>(cfg.viewport_width),
            static_cast<GLsizei>(cfg.viewport_height));

        rect.draw();
        fan.draw();
    }


    void renderer::reshape(const int width, const int height)
    {
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
