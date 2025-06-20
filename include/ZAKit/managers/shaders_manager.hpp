//
// Created by Damian Netter on 20/06/2025.
//

#pragma once

#include <map>
#include <string>

namespace za_kit
{
    class shaders_manager
    {
        static std::map<std::string, std::string> shaders;

    public:

        static void add_shader(const std::string& name, const std::string& source);

        static std::string get_source(const std::string& name);

        static bool shader_exists(const std::string& name);
    };
}
