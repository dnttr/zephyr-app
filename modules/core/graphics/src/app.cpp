//
//
// Created by Damian Netter on 14/05/2025.
//

#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OpenGL/gl3.h"
#include "ZAKit/cg_interface.h"
#include "ZAKit/shaders.hpp"

#include "ft2build.h"

#include FT_FREETYPE_H

//TODO: rewrite it
//TODO: enable MSAA
const zcg_window_t *window;

GLint projection, resolution, radius, pos, size;
GLuint program;
GLuint vao, vbo;

float w_width = 800 * 2, w_height = 600 * 2;

glm::mat4 matrix;

void destroy()
{
}

glm::mat4 get_projection()
{
    glm::mat4 projection = glm::ortho(0.0f, (float)w_width, (float)w_height, 0.0f, -1.0f, 1.0f);

    return projection;
}

void init()
{
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glViewport(0, 0, w_width, w_height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    matrix = get_projection();


    unsigned int indices[] = {
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
    float vertices[] = {
        0.0f, 0.0f, 0.0f,
        w_width, 0.0f, 0.0f,
        w_width, w_height, 0.0f,
        0.0f, w_height, 0.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
}

void render()
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


void update()
{


}

void reshape(const int width, const int height)
{
    w_width = static_cast<float>(width);
    w_height = static_cast<float>(height);

    glViewport(0, 0, width, height);

    matrix = get_projection();
}

int run(std::map<std::string, std::string> &shaders)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "Could not init FreeType library" << std::endl;
        return -1;
    }
    FT_Face face;
    if (FT_New_Face(ft, "/System/Library/Fonts/Supplemental/Arial.ttf", 0, &face))
    {
        std::cerr << "Could not open font" << std::endl;
        return -1;
    }

    FT_Set_Pixel_Sizes(face , 0 /* should it even be 0? */, 48);

    FT_Load_Char(face, 'A', FT_LOAD_RENDER);

    /*
     * Get bitmap ptr
     * Create and bind texture
     * Upload data
     * Set params
     *
     * and something else, idk
     */

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

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
