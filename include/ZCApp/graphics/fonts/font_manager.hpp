//
// Created by Damian Netter on 26/06/2025.
//

#pragma once

#include <ft2build.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <glm/vec2.hpp>
#include <OpenGL/gl3.h>
#include <ZNBKit/debug.hpp>

#include "ZCGKit/external.hpp"

#include FT_FREETYPE_H

namespace zc_app
{
    class font_manager
    {
        friend class font_loader;

        struct character
        {
            glm::ivec2 atlas_offset;
            glm::ivec2 size;
            glm::ivec2 bearing;
            GLuint advance;
        };

        struct font
        {
            GLuint atlas_texture_id = 0;
            std::map<char, character> characters_map;
        };

        static std::unordered_map<std::string, font> fonts_map;
        static FT_Library ft;

    public:
        static void initialize();
    };
}
