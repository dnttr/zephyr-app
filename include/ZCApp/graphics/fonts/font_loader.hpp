//
// Created by Damian Netter on 26/06/2025.
//

#pragma once

#include <array>
#include <vector>

namespace zc_app
{
    class font_loader
    {

        static constexpr std::array<char, 95> ascii = {
            ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.',
            '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=',
            '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
            'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[',
            '\\', ']', '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
            'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
            'z', '{', '|', '}', '~'
        };

    public:
        static constexpr int ATLAS_WIDTH = 1024;
        static constexpr int ATLAS_HEIGHT = 1024;

        static void push_font(const std::string &name, std::vector<int8_t> provided_data);

        static void load_font(const std::string &name);
    };
}
