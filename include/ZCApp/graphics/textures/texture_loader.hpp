//
// Created by Damian Netter on 24/06/2025.
//

#pragma once

#include <OpenGL/gl3.h>

namespace zc_app
{
    class texture_loader
    {
        static GLuint load(const void *pixels, int width, int height);
    };
}
