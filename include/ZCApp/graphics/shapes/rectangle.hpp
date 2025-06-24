//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

#include "shape.hpp"

namespace zc_app
{
    class rectangle final : public shape
    {
        GLint size{};
        GLint radius{};

        GLuint vao{}, vbo{}, ebo{};

    public:

        rectangle() : shape("rectangle_vert", "rectangle_frag")
        {
            setup_uniforms();
            setup_shape();
        }

        ~rectangle() override;

        void draw(float x, float y, float width, float height, float alpha, float red, float green, float blue, float radius) const;

        void setup_uniforms() override;

        void setup_shape() override;
    };
}
