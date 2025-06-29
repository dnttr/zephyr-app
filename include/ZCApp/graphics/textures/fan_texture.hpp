//
// Created by Damian Netter on 25/06/2025.
//

#pragma once

#include "ZCApp/graphics/textures/texture.hpp"
#include "ZCApp/graphics/utils/circle_util.hpp"

namespace zc_app
{
    class fan_texture final : public texture
    {
        const int segments;
        const float radius;

    public:
        fan_texture(std::string name, const container &container, const float radius, const int segments) : texture(std::move(name), container), segments(segments), radius(radius)
        {
        }

        void setup() override;

        void render() override;
    };
}
