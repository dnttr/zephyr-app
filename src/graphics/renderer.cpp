//
// Created by Damian Netter on 23/06/2025.
//

//
//
// Created by Damian Netter on 14/05/2025.
//

#include "ZCApp/graphics/renderer.hpp"

#include <ft2build.h>
#include <iostream>
#include <OpenGL/gl3.h>

#include "ZCApp/graphics/window.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

#include FT_FREETYPE_H

GLint projection, radius, pos, size, color, opacity;
GLuint program;
GLuint vao, vbo;

namespace zc_app
{
    void renderer::initialize()
    {
        const auto& current_config = window::get_current_display_config();

        glClearColor(0.2F, 0.2F, 0.2F, 1.0F);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        const unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        program = shaders::create_program("rectangle_vert", "rectangle_frag");

        projection = glGetUniformLocation(program, "projection_matrix");
        radius = glGetUniformLocation(program, "rectangle_radius");
        pos = glGetUniformLocation(program, "rectangle_position");
        size = glGetUniformLocation(program, "rectangle_size");
        color = glGetUniformLocation(program, "rectangle_color");
        opacity = glGetUniformLocation(program, "rectangle_opacity");

        vao = 0;
        vbo = 0;
        GLuint ebo = 0;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &ebo);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

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
    }

    void renderer::render() const
    {
        const auto& current_config = window::get_current_display_config();

        glm::mat4 projection_matrix = glm::ortho(
            0.0f, current_config.virtual_width,
            0.0f, current_config.virtual_height,
            -1.0f, 1.0f
        );

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);

        glUniform3f(color, 1.0F, 1.0F, 1.0F);
        glUniform1f(opacity, 0.5F);
        glUniform1f(radius, 20.0F);
        glUniform2f(pos, 100.0F, 100.0F);
        glUniform2f(size, 500.0F, 500.0F);
        glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    void update_viewport()
    {

    }


    void renderer::reshape(const int width, const int height)
    {
    }

    void renderer::update()
    {
    }

    void renderer::destroy()
    {
    }

    void renderer::run()
    {
    }
}
