//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

#include <string>
#include <OpenGL/gl3.h>

#include "ZCApp/graphics/shaders/shaders.hpp"

namespace zc_app
{
    class shape
    {
    protected:
        GLint color, opacity;
        GLint projection;
        GLint position;

        GLuint program;

        virtual void setup_uniforms() = 0;

        virtual void setup_shape() = 0;

    public:
        virtual ~shape() = default;

        shape(const std::string &vertex, const std::string &fragment)
        {
            program = shaders::create_program(vertex, fragment);

            projection = glGetUniformLocation(program, "projection_matrix");

            color = glGetUniformLocation(program, "shape_color");
            opacity = glGetUniformLocation(program, "shape_opacity");
            position = glGetUniformLocation(program, "shape_position");
        }
    };
}
