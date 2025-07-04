//
// Created by Damian Netter on 20/06/2025.
//

#include <iostream>
#include <filesystem>
#include <map>
#include <thread>

#include <ZNBKit/vm/vm_management.hpp>
#include "ZCApp/app/app_runner.hpp"
#include "ZCGKit/ipc/client.hpp"
#include "ZCKit/internal/util.hpp"

#define LIBRARIES_SEPARATOR '|'

int main(const int argc, char *argv[])
{
    try
    {
        const auto args = zc_kit::util::to_vector(argc, argv);
        const auto separated_args = zc_kit::util::split(args[0], '+');

        std::map<std::string, std::string> arguments;

        for (const std::string &arg : separated_args)
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

        if (!arguments.contains("libraries") || arguments["libraries"].empty() || !arguments.
            contains("native_libraries") || arguments["native_libraries"].empty())
        {
            throw std::runtime_error("No libraries");
        }

        std::vector<std::string> classpath;
        std::vector<std::string> native_libraries;
        std::string main;

        for (const auto &[name, value] : arguments)
        {
            if (name == "main")
            {
                if (zc_kit::util::is_path_valid(value) && std::filesystem::is_regular_file(value) && value.
                    ends_with(".jar"))
                {
                    main = std::filesystem::absolute(value);
                }
            }
            if (name == "libraries")
            {
                const auto files = zc_kit::util::split(value, LIBRARIES_SEPARATOR);
                constexpr std::vector<std::string> libraries;

                for (const auto &file : files)
                {
                    if (zc_kit::util::is_path_valid(file))
                    {
                        if (std::filesystem::is_directory(file))
                        {
                            for (const auto &entry : std::filesystem::directory_iterator(file))
                            {
                                if (std::filesystem::is_regular_file(entry) && entry.path().extension() == ".jar")
                                {
                                    classpath.push_back(entry.path().string());
                                }
                            }
                        }
                        else
                        {
                            if (std::filesystem::is_regular_file(file) && file.ends_with(".jar"))
                            {
                                classpath.push_back(file);
                            }
                            else
                            {
                                throw std::runtime_error("Invalid library file: " + file);
                            }
                        }
                    }
                }
            }
            else if (name == "native_libraries")
            {
                for (const auto files = zc_kit::util::split(value, LIBRARIES_SEPARATOR); const auto &file : files)
                {
                    if (zc_kit::util::is_path_valid(file))
                    {
                        if (std::filesystem::is_directory(file))
                        {
                            for (const auto &entry : std::filesystem::directory_iterator(file))
                            {
                                if (std::filesystem::is_regular_file(entry) && entry.path().extension() == ".dylib")
                                {
                                    native_libraries.push_back(std::filesystem::absolute(entry.path().string()));
                                }
                            }
                        }
                        else
                        {
                            if (std::filesystem::is_regular_file(file) && file.ends_with(".dylib"))
                            {
                                native_libraries.push_back(std::filesystem::absolute(file));
                            }
                            else
                            {
                                throw std::runtime_error("Invalid native library file: " + file);
                            }
                        }
                    }
                }
            }
        }

        zcg_kit::client client;
        client.start(main);

        std::cout << main << std::endl;

        std::thread listenerThread([&client]()
        {
            while (client.is_connected())
            {
                if (std::string msg = client.read(); !msg.empty())
                {
                    std::cout << "Received: " << msg << std::endl;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });

        while (true)
        {

        }

        // zc_kit::app_runner::run(classpath, native_libraries);

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
