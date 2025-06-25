//
// Created by Damian Netter on 24/06/2025.
//

#pragma once

#include <string>
#include <unordered_map>
#include <OpenGL/gl3.h>

namespace zc_app
{
    class texture_loader
    {
    public:
        struct texture_info
        {
            void *data;
            int width;
            int height;
        };

        static void push(const std::string &name, const texture_info& info);

        static GLuint get(const std::string &name);

    private:
        static std::unordered_map<std::string, texture_info> textures_data_map;
    };
}
