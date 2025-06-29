//
// Created by Damian Netter on 29/06/2025.
//

#pragma once

#include <OpenGL/gl3.h>

namespace zc_app
{
    class framebuffer
    {
        GLuint fbo = 0;
        GLuint texture = 0;
        GLuint rbo = 0;

        int width = 0;
        int height = 0;

    public:
        framebuffer(const framebuffer &) = delete;
        framebuffer &operator=(const framebuffer &) = delete;

        void setup(int width, int height);

        void resize(int new_width, int new_height);

        void bind();

        void unbind();
    };
}
