//
// Created by Damian Netter on 25/06/2025.
//

#pragma once

#include <string>
#include <OpenGL/gl3.h>

#include "texture_loader.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCApp/graphics/utils/circle_util.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

namespace zc_app
{
    class texture
    {
        GLuint vao{}, vbo{}, ebo{};

        GLuint m_texture = 0;
        GLuint program = 0;

        std::string m_name;

    public:
        explicit texture(const std::string &name) : m_name(name)
        {
        }

        void setup()
        {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);

            const auto circle_vertices = generate_circle_vertices(0.5F,128);

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, circle_vertices.size() * sizeof(float), circle_vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);
        }

        void draw()
        {
            if (m_texture == 0)
            {
                setup();

                m_texture = texture_loader::get(m_name);
                program = shaders::create_program("texture_vert", "texture_frag");
            }

            glUseProgram(program);
            const auto &current_config = perspective_util::get_current_display_config();
            float size_x = 256.0f;
            float size_y = 256.0f;

            glUniform2f(glGetUniformLocation(program, "size"), size_x, size_y);

            glUniformMatrix4fv(glGetUniformLocation(program, "projection_matrix"), 1, GL_FALSE, perspective_util::get_projection_matrix());
            glUniform1i(glGetUniformLocation(program, "ourTexture"), 0);
            glUniform2f(glGetUniformLocation(program, "position"), 200, 200);

            glBindVertexArray(vao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_texture);

            const GLsizei vertexCount = 1 + (128 + 1);
            glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
            glBindVertexArray(0);
        }
    };
}
