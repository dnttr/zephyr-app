//
// Created by Damian Netter on 23/06/2025.
//

#pragma once
#include "shape.hpp"

namespace zc_app
{
    class rectangle final : public shape
    {
        float x, y, width, height, radius;
    public:

        //TODO: specify them later
        rectangle(const float x, const float y, const float width, const float height, const float radius = 0.0f)
            : shape("", ""), x(x), y(y), width(width), height(height), radius(radius)
        {
        }

        void draw() const override;
    };
}
