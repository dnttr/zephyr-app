//
// Created by Damian Netter on 25/06/2025.
//

#pragma once

#include <algorithm>
#include <string>

#include "ZCApp/graphics/textures/texture.hpp"

namespace zc_app
{
    class quad_texture final : public texture
    {
        GLuint ebo{};

    public:
        quad_texture(std::string name, const container &container) : texture(std::move(name), container)
        {
        }

        void setup() override;

        void render() override;

        ~quad_texture() override;
    };
}
