//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

#include "shape.hpp"

namespace zc_app
{
    class rectangle final : public shape
    {
        GLuint size;
        GLuint radius;

        GLuint vao, vbo, ebo;

    public:

        rectangle() : shape("rectangle_vert", "rectangle_frag")
        {
            size = glGetUniformLocation(program, "rectangle_size");
            radius = glGetUniformLocation(program, "rectangle_radius");

            const unsigned int indices[] = {
                0, 1, 2,
                2, 3, 0
            };

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &ebo);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);

            const auto& current_config = window::get_current_display_config();

            const float vertices[] = {
                0.0F, 0.0F, 0.0F,
                current_config.virtual_width, 0.0F, 0.0F,
                current_config.virtual_width, current_config.virtual_height, 0.0F,
                0.0F, current_config.virtual_height, 0.0F
            };


            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);
        }

        ~rectangle() override
        {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
        }

        void draw(float x, float y, float width, float height, float alpha, float red, float green, float blue, const float radius) const
        {
            glUseProgram(program);

            const float* proj_matrix = window::get_projection_matrix();
            if (!proj_matrix) {
                std::cerr << "ERROR: Projection matrix is null!" << std::endl;
                return;
            }

            if (projection == -1) std::cerr << "WARNING: projection uniform not found!" << std::endl;
            if (position == -1) std::cerr << "WARNING: position uniform not found!" << std::endl;
            if (size == -1) std::cerr << "WARNING: size uniform not found!" << std::endl;
            if (color == -1) std::cerr << "WARNING: color uniform not found!" << std::endl;
            if (opacity == -1) std::cerr << "WARNING: opacity uniform not found!" << std::endl;



            glUniformMatrix4fv(projection, 1, GL_FALSE, window::get_projection_matrix());
            glUniform2f(position, x, y);
            glUniform2f(size, width, height);
            glUniform1f(this->radius, radius);
            glUniform3f(color, red, green, blue);
            glUniform1f(opacity, alpha);

            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            glBindVertexArray(0);
            glUseProgram(0);
        }
    };
}
