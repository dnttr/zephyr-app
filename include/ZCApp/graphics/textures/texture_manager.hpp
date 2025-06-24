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
        void add_texture(const std::string &name, GLuint texture)
        {
            if (textures.contains(name))
            {
                throw std::runtime_error("Texture with name '" + name + "' already exists.");
            }

            textures.emplace(name, texture);
        }

        void delete_texture(const std::string &name)
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

        void cleanup()
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
    };
}
