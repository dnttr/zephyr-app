//
// Created by Damian Netter on 24/06/2025.
//

#include "ZCApp/graphics/shapes/rectangle.hpp"

#include "ZCApp/graphics/utils/perspective_util.hpp"

namespace zc_app
{
    rectangle::~rectangle()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    void rectangle::setup_uniforms()
    {
        size = glGetUniformLocation(program, "rectangle_size");
        radius = glGetUniformLocation(program, "rectangle_radius");
    }

    void rectangle::setup_shape()
    {
        const unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &ebo);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

        const auto& current_config = perspective_util::get_current_display_config();

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

    void rectangle::render()
    {
        glUseProgram(program);

        glUniformMatrix4fv(projection, 1, GL_FALSE, perspective_util::get_projection_matrix());
        glUniform2f(position, m_container.get_x(), m_container.get_y());
        glUniform2f(size, m_container.get_width(), m_container.get_height());
        glUniform1f(this->radius, m_radius);
        glUniform3f(color, m_colour.get_red_direct(), m_colour.get_green_direct(), m_colour.get_blue_direct());
        glUniform1f(opacity, m_colour.get_alpha_direct());

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    container& rectangle::get_container()
    {
        return m_container;
    }

    void rectangle::set_container(const container& new_container)
    {
        m_container = new_container;
    }

    colour& rectangle::get_colour()
    {
        return m_colour;
    }

    void rectangle::set_colour(const colour& new_colour)
    {
        m_colour = new_colour;
    }

    float rectangle::get_radius() const
    {
        return m_radius;
    }

    void rectangle::set_radius(const float new_radius)
    {
        m_radius = new_radius;
    }
}