//
// Created by Damian Netter on 20/06/2025.
//

#include <iostream>

#include "ZAKit/app_runner.hpp"
#include "ZAKit/util.hpp"
#include "ZNBKit/vm_management.hpp"

int main(const int argc, char *argv[])
{
    try {
        const auto args = za_kit::util::to_vector(argc, argv);

        if (args.empty())
        {
            std::cerr << "No VM path provided. Usage: " << argv[0] << " vm_path=<path_to_vm>" << std::endl;

            throw std::invalid_argument("No VM path provided.");
        }

        const auto vm_path = za_kit::util::parse_argument(args.at(0));

        if (!za_kit::util::is_path_valid(vm_path))
        {
            std::cerr << "VM path does not exist: " << vm_path << std::endl;

            throw std::invalid_argument("Invalid VM path provided.");
        }

        return za_kit::app_runner::run(vm_path);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}