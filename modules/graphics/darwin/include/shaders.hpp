//
// Created by Damian Netter on 18/05/2025.
//

#pragma once

#include <iostream>
#include <ostream>

#include "OpenGL/gl3.h"

class shaders
{

    const char *vertex = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;

            uniform mat4 projection_matrix;

            void main()
            {
                gl_Position = projection_matrix * vec4(aPos, 1.0);
            }
        )";

    const char *fragment = R"(
    #version 330 core
    out vec4 FragColor;

    uniform vec2 rectPos;
    uniform vec2 rectSize;
    uniform float radius;
    uniform vec2 resolution;

    void main() {
    vec2 fragPos = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);
    vec2 pos = fragPos - rectPos;

    vec2 dist = abs(pos - rectSize * 0.5) - (rectSize * 0.5) + vec2(radius);
    float d = length(max(dist, 0.0)) - radius;

float edgeWidth = fwidth(d) * 1.5;

float alpha = 1.0 - smoothstep(0.0, edgeWidth, d);

if (alpha < 0.001)
    discard;

FragColor = vec4(1.0, 0.2, 0.2, alpha);
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