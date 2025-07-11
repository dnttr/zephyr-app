//
// Created by Damian Netter on 24/06/2025.
//

#include "ZCApp/graphics/objects/shapes/rectangle.hpp"

#include <iostream>

#include "ZCApp/graphics/utils/perspective_util.hpp"
#include "ZCGKit/external.hpp"

namespace zc_app
{
    rectangle::~rectangle()
    {
        if (zcg_kit::external::is_context_valid())
        {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
        }
    }

    void rectangle::setup_uniforms()
    {
        u_size = glGetUniformLocation(program, "shape_size");
        u_radius = glGetUniformLocation(program, "shape_radius");
        u_outline_color = glGetUniformLocation(program, "outline_color");
        u_outline_opacity = glGetUniformLocation(program, "outline_opacity");
        u_outline_width = glGetUniformLocation(program, "outline_width");
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

        constexpr float vertices[] = {
            0.0F, 0.0F, 0.0F,
            1.0F, 0.0F, 0.0F,
            1.0F, 1.0F, 0.0F,
            0.0F, 1.0F, 0.0F
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

        glUniformMatrix4fv(u_projection, 1, GL_FALSE, perspective_util::get_projection_matrix());
        glUniform2f(u_position, dimension.get_x(), dimension.get_y());
        glUniform2f(u_size, dimension.get_width(), dimension.get_height());
        glUniform1f(this->u_radius, radius);
        glUniform3f(u_color, color.get_red_direct(), color.get_green_direct(), color.get_blue_direct());
        glUniform1f(u_opacity, color.get_alpha_direct());
        glUniform3f(u_outline_color, outline_color.get_red_direct(), outline_color.get_green_direct(),
                    outline_color.get_blue_direct());
        glUniform1f(u_outline_opacity, outline_color.get_alpha_direct());
        glUniform1f(u_outline_width, outline_width);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    container& rectangle::get_container()
    {
        return dimension;
    }

    void rectangle::set_container(const container& new_container)
    {
        dimension = new_container;
    }

    colour& rectangle::get_colour()
    {
        return color;
    }

    void rectangle::set_colour(const colour& new_colour)
    {
        color = new_colour;
    }

    float rectangle::get_radius() const
    {
        return radius;
    }

    void rectangle::set_radius(const float new_radius)
    {
        radius = new_radius;
    }

    void rectangle::set_outline(const float width, const colour &color, const float opacity)
    {
        outline_width = width;
        outline_color = color;
        outline_color.set_alpha_u8(static_cast<uint8_t>(opacity));
    }

    void rectangle::set_outline_opacity(const float opacity)
    {
        outline_color.set_alpha_u8(static_cast<uint8_t>(opacity));
    }

    float rectangle::get_outline_width() const
    {
        return outline_width;
    }

    colour rectangle::get_outline_color() const
    {
        return outline_color;
    }

    float rectangle::get_outline_opacity() const
    {
        return outline_color.get_alpha_u8();
    }

    void rectangle::set_outline_color(colour colour)
    {
        outline_color = colour;
        outline_color.set_alpha_u8(static_cast<uint8_t>(outline_color.get_alpha_direct()));
    }
}
