//
// Created by Damian Netter on 26/06/2025.
//

#include "ZCApp/graphics/fonts/font_loader.hpp"

#include <string>
#include <vector>

#include "ZCApp/graphics/fonts/font_manager.hpp"
#include "ZCGKit/external.hpp"

namespace zc_app
{
    std::map<std::string, std::vector<int8_t>> buffers{};

    void font_loader::push_font(const std::string& name, std::vector<int8_t> provided_data)
    {
        if (provided_data.empty())
        {
            debug_print_cerr("No font data provided for: " + name);
            return;
        }

        if (buffers.contains(name))
        {
            debug_print_cerr("Font with name '" + name + "' already exists.");
            return;
        }

        buffers.emplace(name, std::move(provided_data));

        debug_print("Pushed font: " + name + " with size: " + std::to_string(buffers[name].size()) + " bytes");
    }

    void font_loader::load_font(const std::string& name)
    {
        const auto font_data = buffers.find(name)->second;
        const size_t size = font_data.size();

        const std::unique_ptr<unsigned char[]> buffer(new unsigned char[size]);
        std::memcpy(buffer.get(), font_data.data(), size);

        if (buffer && size > 0)
        {
            if (font_manager::fonts_map.contains(name))
            {
                throw std::runtime_error("Font with name '" + name + "' already exists.");
            }

            FT_Face face;

            if (FT_New_Memory_Face(font_manager::ft, buffer.get(), static_cast<FT_Long>(size), 0, &face))
            {
                if (font_manager::ft == nullptr)
                {
                    debug_print_cerr("FreeType library is not initialized.");
                    return;
                }

                debug_print_cerr("unable to create FreeType face for font: " + name);
                return;
            }

            if (FT_Set_Pixel_Sizes(face, 0, 64))
            {
                debug_print_cerr("unable to set pixel size for font: " + name);
                FT_Done_Face(face);
                return;
            }

            font_manager::font new_font;

            glGenTextures(1, &new_font.atlas_texture_id);
            glBindTexture(GL_TEXTURE_2D, new_font.atlas_texture_id);

            std::vector<unsigned char> atlas_data(ATLAS_WIDTH * ATLAS_HEIGHT, 0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ATLAS_WIDTH, ATLAS_HEIGHT, 0, GL_RED,
                         GL_UNSIGNED_BYTE, atlas_data.data());

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

            FT_Int major, minor, patch;

            FT_Library_Version(font_manager::ft, &major, &minor, &patch);
            debug_print("FreeType version: " + std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch));

            FT_GlyphSlot slot = face->glyph;

            for (char value : ascii)
            {
                if (FT_Load_Char(face, value, FT_LOAD_RENDER))
                {
                    debug_print_cerr("unable to load character '" + std::string(1, value) + "' for font: " + name);
                }

                FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

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

                new_font.characters_map[value] = font_manager::character(
                    {current_x, current_y},
                    {width, height},
                    {face->glyph->bitmap_left, face->glyph->bitmap_top},
                    face->glyph->advance.x
                );

                current_x += width;
            }

            glBindTexture(GL_TEXTURE_2D, 0);
            FT_Done_Face(face);

            font_manager::fonts_map[name] = std::move(new_font);
        }
        else
        {
            debug_print_cerr("invalid font data provided for: " + name);
        }
    }
};
