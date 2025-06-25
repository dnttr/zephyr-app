//
// Created by Damian Netter on 24/06/2025.
//

#include "ZCApp/graphics/textures/texture_loader.hpp"

#include <OpenGL/gl3.h>

#include "ZCGKit/external.hpp"

namespace zc_app
{
    std::unordered_map<std::string, texture_loader::texture_info> texture_loader::textures_data_map;

    void texture_loader::push(const std::string &name, const texture_info& info)
    {
        if (textures_data_map.contains(name))
        {
            throw std::runtime_error("Texture with name '" + name + "' already exists.");
        }

        textures_data_map.emplace(name, info);
    }

    GLuint texture_loader::get(const std::string &name)
    {
        const auto tex_data = textures_data_map.find(name);

        if (tex_data == textures_data_map.end())
        {
            return 0;
        }

        const auto &[data, width, height] = tex_data->second;

        GLuint texture_id = 0;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (const GLint level = zcg_kit::external::hasAnisotropicFiltering(); level > 1)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        return texture_id;
    }
}
