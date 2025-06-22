//
// Created by Damian Netter on 20/06/2025.
//

#pragma once

#include <string>

namespace zc_kit
{
    class util {
    public:
        static std::string parse_argument(const std::string &argument);

        static bool is_path_valid(const std::string &path);

        static std::vector<std::string> to_vector(int argc, char *argv[]);
    };
}