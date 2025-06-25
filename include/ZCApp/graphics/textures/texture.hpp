//
// Created by Damian Netter on 25/06/2025.
//

#pragma once

#include <string>
#include <OpenGL/gl3.h>

#include "texture_loader.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"
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
            const unsigned int indices[] = {
                0, 1, 2,
                2, 3, 0
            };

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &ebo);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);

            const auto &current_config = perspective_util::get_current_display_config();
            const float vertices[] = {
                0.0F, 0.0F, 0.0F,  0.0f, 0.0f,
                1.0F, 0.0F, 0.0F,  1.0f, 0.0f,
                1.0F, 1.0F, 0.0F,  1.0f, 1.0f,
                0.0F, 1.0F, 0.0F,  0.0f, 1.0f
            };


            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

            auto [scale_x, scale_y] = perspective_util::get_effective_virtual_dimensions();

            glUniformMatrix4fv(glGetUniformLocation(program, "projection_matrix"), 1, GL_FALSE, perspective_util::get_projection_matrix());
            glUniform1i(glGetUniformLocation(program, "ourTexture"), 0);
            glUniform2f(glGetUniformLocation(program, "position"), 200, 200);
            glUniform2f(glGetUniformLocation(program, "size"), 256 / 4, 256 / 4);
            const auto &current_config = perspective_util::get_current_display_config();

            glBindVertexArray(vao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_texture);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    };
}
