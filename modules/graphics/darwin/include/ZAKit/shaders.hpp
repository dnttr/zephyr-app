//
// Created by Damian Netter on 18/05/2025.
//

#pragma once

#include <iostream>
#include <ostream>

#include "OpenGL/gl3.h"

namespace za_kit
{
    class shaders
    {
        GLuint create(const char *source, const GLuint type)
        {
            const GLuint shader = glCreateShader(type);
            glShaderSource(shader, 1, &source, nullptr);
            glCompileShader(shader);

            int success = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (success == GL_FALSE)
            {
                int length = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
                std::vector<char> infoLog(length);

                glGetShaderInfoLog(shader, length, nullptr, infoLog.data());

                std::cout << infoLog.data() << std::endl;

                glDeleteShader(shader);

                return 0;
            }

            return shader;
        }
    public:

        /*GLuint compile()
        {
            const GLuint vertex_shader = create(vertex, GL_VERTEX_SHADER);
            const GLuint fragment_shader = create(fragment, GL_FRAGMENT_SHADER);

            if (vertex_shader == 0 || fragment_shader == 0)
            {
                return 0;
            }

            const GLuint program = glCreateProgram();

            glAttachShader(program, vertex_shader);
            glAttachShader(program, fragment_shader);
            glLinkProgram(program);

            int success = 0;
            glGetProgramiv(program, GL_LINK_STATUS, &success);

            if (success == GL_FALSE)
            {
                int length = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
                std::vector<char> infoLog(length);

                glGetProgramInfoLog(program, length, nullptr, infoLog.data());

                std::cout << infoLog.data() << std::endl;

                glDeleteProgram(program);

                return 0;
            }

            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);

            return program;
        }*/
    };
}