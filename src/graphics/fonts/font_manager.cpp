//
// Created by Damian Netter on 26/06/2025.
//

#include "ZCApp/graphics/fonts/font_manager.hpp"

namespace zc_app
{
    std::unordered_map<std::string, font_manager::font> font_manager::fonts_map;
    FT_Library font_manager::ft;

    void font_manager::initialize()
    {
        if (FT_Init_FreeType(&ft))
        {
            debug_print_cerr("unable to initialize FreeType library");
        }
    }
}