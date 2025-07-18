//
// Created by Damian Netter on 26/06/2025.
//

#pragma once


#include <hb-ft.h>
#include <hb.h>
#include <iostream>
#include <map>
#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <freetype/freetype.h>
#include <glm/glm.hpp>
#include <OpenGL/gl3.h>

#include FT_FREETYPE_H

namespace zc_app
{
    class font_manager
    {
        friend class font_loader;

        static FT_Library ft;
        static std::map<std::string, FT_Face> faces;

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
            std::map<unsigned int, character> characters_map;

            hb_font_t *hb_font = nullptr;
        };

        static std::unordered_map<std::string, font> fonts_map;

        static void initialize();

        static font& get_font(const std::string &name)
        {
            const auto pair = fonts_map.find(name);

            if (pair == fonts_map.end())
            {
                throw std::runtime_error("Font not found ");
            }

            return pair->second;
        }

        ~font_manager()
        {
            cleanup();
        }

        static void cleanup()
        {
            for (const auto &val: fonts_map | std::views::values)
            {
                hb_font_destroy(val.hb_font);
            }
            fonts_map.clear();

            for (const auto &val: faces | std::views::values)
            {
                FT_Done_Face(val);
            }

            faces.clear();
            FT_Done_FreeType(ft);
        }
    };
}
