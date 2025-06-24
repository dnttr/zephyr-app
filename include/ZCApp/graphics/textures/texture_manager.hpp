//
// Created by Damian Netter on 24/06/2025.
//

#pragma once

#include <unordered_map>
#include <OpenGL/gl3.h>

#include <string>

namespace zc_app
{
    class texture_manager
    {
        std::unordered_map<std::string, GLuint> textures;

    public:
        void add_texture(const std::string &name, const int width, const int height)
        {
            if (textures.contains(name))
            {
                throw std::runtime_error("Texture with name '" + name + "' already exists.");
            }
        }
    };
}
