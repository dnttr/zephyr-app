//
// Created by Damian Netter on 24/06/2025.
//

#include "ZCApp/graphics/textures/texture_manager.hpp"

namespace zc_app
{
    std::unordered_map<std::string, GLuint> texture_manager::textures;

    void texture_manager::add_texture(const std::string &name, GLuint texture)
    {
        if (textures.contains(name))
        {
            throw std::runtime_error("Texture with name '" + name + "' already exists.");
        }

        textures.emplace(name, texture);
    }

    void texture_manager::delete_texture(const std::string &name)
    {
        if (const auto it = textures.find(name); it != textures.end())
        {
            const unsigned texture_id = it->second;

            if (texture_id != 0)
            {
                glDeleteTextures(1, &texture_id);
                textures.erase(it);
            }
        }
    }

    void texture_manager::cleanup()
    {
        for (const auto& [name, texture] : textures)
        {
            if (texture != 0)
            {
                glDeleteTextures(1, &texture);
                textures.erase(name);
            }
        }
    }
}
