//
// Created by Damian Netter on 29/06/2025.
//

#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace zc_app
{
    class string_util
    {
    public:
        static std::pair<std::vector<std::string>, int> get_lines(const std::string &text)
        {
            std::vector<std::string> lines;
            std::stringstream ss(text);
            std::string line;

            while (std::getline(ss, line, '\n'))
            {
                lines.push_back(line);
            }

            size_t total = 0;

            for (const auto &str : lines)
            {
                total += str.length();
            }

            return {lines, static_cast<int>(total)};
        }
    };
}
