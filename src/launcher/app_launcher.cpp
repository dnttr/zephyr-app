//
// Created by Damian Netter on 20/06/2025.
//

#include <iostream>
#include <filesystem>
#include <map>

#include "ZCApp/app/app_runner.hpp"
#include "ZCApp/app/test/test_runner.hpp"

#define GL_SILENCE_DEPRECATION

struct command_line_args
{
    std::string main_jar;
    std::string role = "app";
};

command_line_args parse_arguments(const int argc, char *argv[])
{
    command_line_args args;

    for (int i = 1; i < argc; ++i)
    {
        if (const std::string_view arg = argv[i]; arg == "--main" && i + 1 < argc)
        {
            args.main_jar = argv[++i];
        }
        else if (arg == "--role" && i + 1 < argc)
        {
            args.role = argv[++i];
        }
    }

    return args;
}

int main(const int argc, char *argv[])
{
    try
    {
        const auto [jar, role] = parse_arguments(argc, argv);

        if (jar.empty())
        {
            throw std::runtime_error("Missing --main <path_to_jar>");
        }

        if (role == "test_harness")
        {
            zc_kit::test_runner::run_test_harness(argv[0], jar);
            return 0;
        }

        zc_kit::bridge::internal_initialize(jar);

        if (role == "client_a" || role == "client_b")
        {
            zc_kit::app_runner::run_test(role);
        }
        else
        {
            zc_kit::app_runner::run_application();
        }

        zc_kit::bridge::internal_terminate();
    }
    catch (const std::exception &e)
    {
        std::cerr << "App Runner Error: " << e.what() << std::endl;
        zc_kit::bridge::internal_terminate();
    }

    return 0;
}
