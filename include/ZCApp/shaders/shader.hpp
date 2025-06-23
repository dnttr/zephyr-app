//
// Created by Damian Netter on 18/05/2025.
//

#pragma once

#include <iostream>
#include <OpenGL/gl3.h>
#include <ZNBKit/internal/wrapper.hpp>

namespace zc_app
{
    class shader
    {
    public:

        static GLuint create_program(const std::string &vertex_name, const std::string &fragment_name);

        static GLuint create(const char *source, GLuint type);

        //WARNING: this function does not delete the shaders; ensure to delete them when no longer needed
        static GLuint compile(GLuint vertex_shader, GLuint fragment_shader);
    };

    static void delete_shader(const GLuint shader)
    {
        if (shader != 0)
        {
            glDeleteShader(shader);
        }
    }
}