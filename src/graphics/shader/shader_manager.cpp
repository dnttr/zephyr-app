//
// Created by Damian Netter on 20/06/2025.
//

#include "ZCApp/shaders/shader_manager.hpp"

std::map<std::string, std::string> zc_app::shader_manager::shaders;

void zc_app::shader_manager::add_shader(const std::string& name, const std::string& source)
{
    shaders[name] = source;
}

std::string zc_app::shader_manager::get_source(const std::string& name)
{
    if (shaders.contains(name))
    {
        return shaders.find(name)->second;
    }
    return {};
}

bool zc_app::shader_manager::shader_exists(const std::string& name)
{
    if (name.empty())
    {
        return false;
    }
    return shaders.contains(name);
}