//
// Created by Damian Netter on 18/05/2025.
//

#pragma once

#include <iostream>
#include <ostream>

#include "OpenGL/gl3.h"

class shaders
{

    //TODO: add projection matrix
    const char *vertex = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;

            uniform mat4 projection_matrix;
            uniform vec2 scale;

            void main()
            {
                vec3 screenPos = vec3(aPos.x * scale.x, aPos.y * scale.y, aPos.z);
                gl_Position = projection_matrix * vec4(screenPos, 1.0);
            }
        )";

    const char *fragment = R"(
            #version 330 core
            out vec4 FragColor;

            uniform float radius;
            uniform vec2 resolution;

            void main()
            {
                vec2 center = resolution * 0.5;
    vec2 pos = gl_FragCoord.xy - center;

    // Random fixed rectangle size (e.g., 300x200 pixels)
    vec2 rectSize = vec2(300.0, 200.0);
    vec2 halfSize = rectSize * 0.5;

    vec2 dist = abs(pos) - halfSize + vec2(radius);
    float d = length(max(dist, 0.0)) - radius;

    if (d > 0.0)
        discard;

    FragColor = vec4(1.0, 0.2, 0.2, 1.0);
            }
        )";

    GLuint create(const char *source, GLuint type)
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

    GLuint compile()
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
    }
};