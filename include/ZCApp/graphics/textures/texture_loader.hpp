//
// Created by Damian Netter on 24/06/2025.
//

#pragma once

#include <string>
#include <unordered_map>
#include <OpenGL/gl3.h>

#define MAX_ANISOTROPIC (-1)

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

        static std::pair<GLuint, texture_info> get(const std::string &name, int min, int mag, bool mipmap, int anisotropic);

    private:
        static std::unordered_map<std::string, texture_info> textures_data_map;
    };
}
