//
// Created by Damian Netter on 25/06/2025.
//

#pragma once

#include <OpenGL/gl3.h>
#include <OpenGL/OpenGL.h>

#ifndef GL_EXT_texture_filter_anisotropic
#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

namespace zcg_kit
{
    class external
    {
    public:

        static GLint has_anisotropic_filtering()
        {
            GLint max_anisotropy = 0;
            glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy);

            return max_anisotropy;
        }

        static bool is_context_valid()
        {
            return CGLGetCurrentContext() != nullptr;
        }
    };
}