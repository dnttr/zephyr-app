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

namespace zc_app
{
    rectangle *rect;

    void renderer::initialize()
    {
        glClearColor(0.2F, 0.2F, 0.2F, 1.0F);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        rect = new rectangle(container(0, 0, 100.0F, 100.0F), colour(255, 0, 0, 255), 20.0F);
    }

    void renderer::render() const
    {
        glClear(GL_COLOR_BUFFER_BIT);

        if (rect)
        {
            rect->draw();
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
