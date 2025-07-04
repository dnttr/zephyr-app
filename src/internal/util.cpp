//
// Created by Damian Netter on 20/06/2025.
//

#include "ZCKit/internal/util.hpp"

#include <filesystem>
#include <sstream>
#include <vector>

std::vector<std::string> zc_kit::util::split(const std::string &str, const char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

bool zc_kit::util::is_path_valid(const std::string &path)
{
    return std::filesystem::exists(path);
}

std::vector<std::string> zc_kit::util::to_vector(const int argc, char *argv[])
{
    std::vector<std::string> args;
    args.reserve(argc - 1);

    for (int i = 1; i < argc; ++i)
    {
        args.emplace_back(argv[i]);
    }

    return args;
}
