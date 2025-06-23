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
        GLuint resolution;
        GLuint projection;

        GLuint program;
    public:
        virtual ~shape() = default;

        shape(const std::string &vertex, const std::string &fragment)
        {
            program = shaders::create_program(vertex, fragment);

            resolution = glGetUniformLocation(program, "resolution");
            projection = glGetUniformLocation(program, "projection_matrix");
        }

        virtual void draw() const = 0;
    };
}
