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
        std::map<std::string, std::string> shaders;

    public:

        void add_shader(const std::string& name, const std::string& source)
        {
            shaders[name] = source;
        }

        std::string get_source(const std::string& name) const
        {
            if (shaders.contains(name))
            {
                return shaders.find(name)->second;
            }
            return {};
        }

        bool shader_exists(const std::string& name) const
        {
            return shaders.contains(name);
        }
    };
}
