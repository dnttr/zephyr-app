//
// Created by Damian Netter on 20/06/2025.
//

#include "ZCKit/internal/util.hpp"

#include <filesystem>
#include <vector>

std::string zc_kit::util::parse_argument(const std::string &argument)
{
    const size_t position = argument.find_last_of('=');

    if (position == std::string::npos)
    {
        throw std::invalid_argument("Invalid argument. Expected format: name=value");
    }

    return argument.substr(position + 1);
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
