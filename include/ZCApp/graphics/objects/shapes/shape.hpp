//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

#include <string>
#include <utility>
#include <OpenGL/gl3.h>

#include "ZCApp/graphics/shaders/shaders.hpp"

namespace zc_app
{
    class shape
    {
    protected:
        GLint u_color{};
        GLint u_opacity{};
        GLint u_projection{};
        GLint u_position{};

        GLuint program = -1;

        std::string vertex_shader, fragment_shader;

        virtual void setup_uniforms() = 0;

        virtual void setup_shape() = 0;

        virtual void render() = 0;
    public:
        virtual ~shape() = default;

        shape(std::string vertex, std::string fragment) : vertex_shader(std::move(vertex)),
                                                          fragment_shader(std::move(fragment))
        {
        }

        void draw()
        {
            if (program == -1)
            {
                program = shaders::create_program(vertex_shader, fragment_shader);

                u_projection = glGetUniformLocation(program, "projection_matrix");
                u_color = glGetUniformLocation(program, "shape_color");
                u_opacity = glGetUniformLocation(program, "shape_opacity");
                u_position = glGetUniformLocation(program, "shape_position");

                setup_uniforms();
                setup_shape();
            }

            render();
        }
    };
}
