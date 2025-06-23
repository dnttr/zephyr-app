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
#include <map>
#include <glm/glm.hpp>
#include <OpenGL/gl3.h>

#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCApp/graphics/utils/glm_util.hpp"

#include FT_FREETYPE_H

GLint projection, resolution, radius, pos, size;
GLuint program;
GLuint vao, vbo;

glm::mat4 matrix;

namespace zc_app
{
    void renderer::initialize()
    {
        glClearColor(0.2F, 0.2F, 0.2F, 1.0F);
        glViewport(0.0F, 0.0F, w_width, w_height);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        matrix = glm_util::get_projection(static_cast<float>(w_width), static_cast<float>(w_height));

        const unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        program = shaders::create_program("rounded_rectangle_vert", "rounded_rect_frag");

        resolution = glGetUniformLocation(program, "resolution");
        projection = glGetUniformLocation(program, "projection_matrix");
        radius = glGetUniformLocation(program, "radius");
        pos = glGetUniformLocation(program, "rectPos");
        size = glGetUniformLocation(program, "rectSize");

        vao = 0;
        vbo = 0;
        GLuint ebo = 0;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &ebo);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

        const int vertices[] = {
            0, 0, 0,
            w_width, 0, 0,
            w_width, w_height, 0,
            0, w_height, 0
        };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 3 * sizeof(int), nullptr);
        glEnableVertexAttribArray(0);
    }

    void renderer::render()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);

        glUniform2f(resolution, static_cast<float>(w_width), static_cast<float>(w_height));
        glUniform1f(radius, 20);
        glUniform2f(pos, 100, 100);
        glUniform2f(size, 200, 200);
        glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(matrix));

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }


    void renderer::reshape(const int width, const int height)
    {
        w_width = width;
        w_height = height;

        glViewport(0, 0, width, height);

        matrix = glm_util::get_projection(static_cast<float>(width), static_cast<float>(height));
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