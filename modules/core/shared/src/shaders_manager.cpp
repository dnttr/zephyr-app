//
// Created by Damian Netter on 20/06/2025.
//

#include "ZCApp/managers/shaders_manager.hpp"

std::map<std::string, std::string> zc_app::shaders_manager::shaders;

void zc_app::shaders_manager::add_shader(const std::string& name, const std::string& source)
{
    shaders[name] = source;
}

std::string zc_app::shaders_manager::get_source(const std::string& name)
{
    if (shaders.contains(name))
    {
        return shaders.find(name)->second;
    }
    return {};
}

bool zc_app::shaders_manager::shader_exists(const std::string& name)
{
    if (name.empty())
    {
        return false;
    }
    return shaders.contains(name);
}


