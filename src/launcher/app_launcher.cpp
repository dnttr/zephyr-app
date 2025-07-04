//
// Created by Damian Netter on 20/06/2025.
//

#include <iostream>
#include <map>

#include <ZNBKit/vm/vm_management.hpp>
#include "ZCApp/app/app_runner.hpp"
#include "ZCKit/internal/util.hpp"

int main(const int argc, char *argv[])
{
    try
    {
        const auto args = zc_kit::util::to_vector(argc, argv);
        const auto separated_args = zc_kit::util::split(args[0], ';');

        std::map<std::string, std::string> arguments;

        for (const std::string& arg : separated_args)
        {
            auto single_arg = zc_kit::util::split(arg, '=');

            if (single_arg.size() != 2)
            {
                throw std::runtime_error("Invalid argument format: " + arg);
            }

            std::string key = single_arg[0];
            std::string value = single_arg[1];

            arguments.emplace(key, value);
        }

        std::vector<std::string> classpath;

        if (!(arguments.contains("management") || arguments.contains("network")))
        {
            throw std::runtime_error("No management or network argument provided.");
        }

        const auto management_path = arguments["management"];
        const auto network_path = arguments["network"];

        if (!(zc_kit::util::is_path_valid(management_path) && zc_kit::util::is_path_valid(network_path)))
        {
            throw std::runtime_error("Invalid management or network path provided.");
        }


        classpath.push_back(arguments["management"]);
        classpath.push_back(arguments["network"]);

        zc_kit::app_runner::run(classpath);

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
