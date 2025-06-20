//
// Created by Damian Netter on 20/06/2025.
//

#include "ZAKit/managers/shaders_manager.hpp"

void za_kit::shaders_manager::add_shader(const std::string& name, const std::string& source)
{
    shaders[name] = source;
}

std::string za_kit::shaders_manager::get_source(const std::string& name)
{
    if (shaders.contains(name))
    {
        return shaders.find(name)->second;
    }
    return {};
}

bool za_kit::shaders_manager::shader_exists(const std::string& name)
{
    return shaders.contains(name);
}


