//
// Created by Damian Netter on 20/06/2025.
//

#pragma once

#include <string>
#include <vector>

namespace zc_kit
{
    class util
    {
    public:
        static std::vector<std::string> split(const std::string &str, char delimiter);

        static bool is_path_valid(const std::string &path);

        static std::vector<std::string> to_vector(int argc, char *argv[]);
    };
}
