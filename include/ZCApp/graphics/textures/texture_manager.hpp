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
        static std::unordered_map<std::string, GLuint> textures;

    public:
        static void add_texture(const std::string &name, GLuint texture);

        static void delete_texture(const std::string &name);

        static void cleanup();
    };
}
