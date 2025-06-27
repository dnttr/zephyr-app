//
// Created by Damian Netter on 26/06/2025.
//

#pragma once

#include <ft2build.h>
#include <map>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <OpenGL/gl3.h>
#include <ZNBKit/debug.hpp>

#include FT_FREETYPE_H

namespace zc_app
{
    class font_manager
    {
        friend class font_loader;

        static FT_Library ft;

    public:
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

        static void initialize();

        static font get_font(const std::string &name)
        {
            const auto pair = fonts_map.find(name);

            if (pair == fonts_map.end())
            {
                throw std::runtime_error("Font not found");
            }

            return pair->second;
        }
    };
}
