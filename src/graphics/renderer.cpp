//
// Created by Damian Netter on 23/06/2025.
//

//
//
// Created by Damian Netter on 14/05/2025.
//

#include "ZCApp/graphics/renderer.hpp"

#include <ft2build.h>
#include <iostream>
#include <OpenGL/gl3.h>

#include "ZCApp/graphics/shapes/rectangle.hpp"

#include FT_FREETYPE_H

GLint projection, radius, pos, size, color, opacity;
GLuint program;
GLuint vao, vbo;

namespace zc_app
{
    rectangle *rect = nullptr;

    void renderer::initialize()
    {
        glClearColor(0.2F, 0.2F, 0.2F, 1.0F);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        rect = new rectangle();
    }

    void renderer::render() const
    {
        glClear(GL_COLOR_BUFFER_BIT);

        if (rect) {
            rect->draw(0.0F, 0.0F, 100.0F, 100.0F, 1.0F, 0.0F, 1.0F, 1.0F, 1.0f);
        }
    }

    void update_viewport()
    {
    }


    void renderer::reshape(const int width, const int height)
    {
    }

    void renderer::update()
    {
    }

    void renderer::destroy()
    {
        delete rect;
    }

    void renderer::run()
    {
    }
}
