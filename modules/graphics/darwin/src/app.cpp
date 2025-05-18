//
//
// Created by Damian Netter on 14/05/2025.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "cg_interface.h"
#include "OpenGL/gl3.h"
#include "shaders.hpp"

const zcg_window_t *window;

GLuint program;
GLint resolution_location, radius_location, matrix_location;
GLuint vao;

glm::mat4 projection_matrix;

void destroy()
{
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);

    glUniform1f(radius_location, 0.1f);
    glUniform2f(resolution_location, 800.0f, 600.0f);
    glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void init()
{
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glViewport(0, 0, 800, 600);


    projection_matrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

    float x = 100.0f;
    float y = 100.0f;
    float width = 200.0f;
    float height = 150.0f;

    const float vertices[] = {
        x,         y,          0.0f,
        x + width, y,          0.0f,
        x + width, y + height, 0.0f,
        x,         y + height, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    shaders shader;
    program = shader.compile();

    radius_location = glGetUniformLocation(program, "radius");
    resolution_location = glGetUniformLocation(program, "resolution");
    matrix_location = glGetUniformLocation(program, "projection_matrix");

    vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
}

void update()
{
}

void reshape(int width, int height)
{
}

int main(int argc, char *argv[])
{
    zcg_callback_handle handle = {
        .on_exit_callback = destroy,
        .on_render_callback = render,
        .on_init_callback = init,
        .on_reshape_callback = reshape,
        .on_update_callback = update
    };

    zcg_window_args_t args = {
        .title = "Core Graphics",
        .x = 0,
        .y = 0,
        .width = 800,
        .height = 600
    };

    window = zcg_allocate(&args, &handle);

    zcg_run(window);

    return 0;
}
