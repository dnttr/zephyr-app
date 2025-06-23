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
#include "ZCApp/graphics/utils/glm_util.hpp"

#include FT_FREETYPE_H

GLint projection, radius, pos, size, color, opacity;
GLuint program;
GLuint vao, vbo;

namespace zc_app
{
    int window_width = 0;
    int window_height = 0;

    window::display_config display_config = {};

    void calculate()
    {
        if (display_config.maintain_aspect_ratio)
        {
            const auto f_w_width = static_cast<float>(display_config.window_width);
            const auto f_w_height = static_cast<float>(display_config.window_height);

            const float scale_x = f_w_width / display_config.virtual_width;
            const float scale_y = f_w_height / display_config.virtual_height;

            display_config.scale = std::min(scale_x, scale_y) * display_config.dpi_scale;

            display_config.viewport_width = display_config.virtual_width * display_config.scale;
            display_config.viewport_height = display_config.virtual_height * display_config.scale;

            display_config.viewport_x = (f_w_width - display_config.viewport_width) * 0.5F;
            display_config.viewport_y = (f_w_height - display_config.viewport_height) * 0.5F;
        }
        else
        {
            throw std::runtime_error("Aspect ratio maintenance is not implemented yet.");
        }
    }

    void renderer::initialize()
    {
        display_config.window_width = 1920;
        display_config.window_height = 1080;

        display_config.virtual_width = 1920.0F;
        display_config.virtual_height = 1080.0F;

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
            display_config.virtual_width, 0.0F, 0.0F,
            display_config.virtual_width, display_config.virtual_height, 0.0F,
            0.0F, display_config.virtual_height, 0.0F
        };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        calculate();
    }

    void renderer::render() const
    {
        glViewport(
     (GLint)display_config.viewport_x,
     (GLint)display_config.viewport_y,
     (GLsizei)display_config.viewport_width,
     (GLsizei)display_config.viewport_height);

        glm::mat4 projection_matrix = glm::ortho(
            0.0f, display_config.virtual_width,
            0.0f, display_config.virtual_height,
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
        window_width = width;
        window_height = height;

        display_config.window_width = width;
        display_config.window_height = height;
        calculate();
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
