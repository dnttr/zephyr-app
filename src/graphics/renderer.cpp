//
// Created by Damian Netter on 23/06/2025.
//

//
//
// Created by Damian Netter on 14/05/2025.
//

#include <ft2build.h>
#include <iostream>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <OpenGL/gl3.h>

#include "ZCGKit/zcg_interface.h"

#include FT_FREETYPE_H

#include "ZCApp/graphics/renderer.hpp"

GLint projection, resolution, radius, pos, size;
GLuint program;
GLuint vao, vbo;

glm::mat4 matrix;

namespace zc_app
{
    int w_width;
    int w_height;

    glm::mat4 get_projection()
    {
        const glm::mat4 projection = glm::ortho(0, w_width, w_height, 0, -1, 1);

        return projection;
    }

    void renderer::initialize()
    {
        glClearColor(0.2, 0.2, 0.2, 1.0);
        glViewport(0, 0, w_width, w_height);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        matrix = get_projection();


        const unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        //shaders shader;
        //program = shader.compile();

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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
    }

    void renderer::render()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);

        glUniform2f(resolution, w_width, w_height);
        glUniform1f(radius, 20.0f);
        glUniform2f(pos, 20, 20);
        glUniform2f(size, 200, 200);

        glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(matrix));
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }


    void renderer::update()
    {
    }

    void renderer::reshape(const int width, const int height)
    {
        w_width = width;
        w_height = height;

        glViewport(0, 0, width, height);

        matrix = get_projection();
    }

    void renderer::destroy()
    {
    }

    void renderer::run()
    {
    }
}