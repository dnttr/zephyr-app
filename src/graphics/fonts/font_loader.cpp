//
// Created by Damian Netter on 26/06/2025.
//

#include <string>

#include "ZCApp/graphics/fonts/font_manager.hpp"
#include "ZCApp/graphics/fonts/font_loader.hpp"

namespace zc_app
{
    void font_loader::load_font(const std::string &name, const unsigned char *buffer, const size_t size)
    {
        if (buffer != nullptr && size > 0)
        {
            if (font_manager::fonts_map.contains(name))
            {
                throw std::runtime_error("Font with name '" + name + "' already exists.");
            }

            FT_Face face;

            if (FT_New_Memory_Face(font_manager::ft, buffer, static_cast<FT_Long>(size), 0, &face))
            {
                debug_print_cerr("unable to create FreeType face for font: " + name);
                return;
            }

            if (FT_Set_Pixel_Sizes(face, 0, 48))
            {
                debug_print_cerr("unable to set pixel size for font: " + name);
                FT_Done_Face(face);
                return;
            }


            font_manager::font new_font;

            glGenTextures(1, &new_font.atlas_texture_id);
            glBindTexture(GL_TEXTURE_2D, new_font.atlas_texture_id);

            std::vector<unsigned char> atlas_data(ATLAS_WIDTH * ATLAS_HEIGHT, 0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED,
                         GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            if (const GLint level = zcg_kit::external::has_anisotropic_filtering(); level > 1)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
            }

            int current_x = 0;
            int current_y = 0;
            int row_height = 0;

            for (char value : ascii)
            {
                if (FT_Load_Char(face, value, FT_LOAD_RENDER))
                {
                    debug_print_cerr("unable to load character '" + std::string(1, value) + "' for font: " + name);
                }

                int width = face->glyph->bitmap.width;
                int height = face->glyph->bitmap.rows;

                if (current_x + width > ATLAS_WIDTH)
                {
                    current_x = 0;
                    current_y += row_height;
                    row_height = 0;
                }

                if (height > row_height)
                {
                    row_height = height;
                }

                if (current_y + height > ATLAS_HEIGHT)
                {
                    throw std::runtime_error("Font atlas is too small for font: " + name);
                }

                if (width > 0 && height > 0)
                {
                    glTexSubImage2D(GL_TEXTURE_2D, 0, current_x, current_y, width, height, GL_RED, GL_UNSIGNED_BYTE,
                                    face->glyph->bitmap.buffer);
                }

                new_font.characters_map[value] = font_manager::character({current_x, current_y}, {width, height},
                                                           {face->glyph->bitmap_left, face->glyph->bitmap_top},
                                                           face->glyph->advance.x);
            }

            glBindTexture(GL_TEXTURE_2D, 0);
            FT_Done_Face(face);
        }
        else
        {
            debug_print_cerr("invalid font data provided for: " + name);
        }
    }
};
