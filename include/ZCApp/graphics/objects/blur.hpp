//
// Created by Damian Netter on 01/07/2025.
//

#pragma once

#include <OpenGL/gl3.h>

#include "ZCApp/graphics/framebuffer.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"

namespace zc_app
{
    class blur
    {
        GLuint blur_program{};

        GLuint blur_vao{};
        GLuint blur_vbo{};
        GLuint blur_ebo{};

        framebuffer fbo_1{}, fbo_2{};

        GLint u_blur_texture{};
        GLint u_blur_mask_texture{};
        GLint u_blur_radius{};
        GLint u_blur_size{};
        GLint u_blur_tint_color{};
        GLint u_blur_tint_strength{};
        GLint u_blur_quality{};

        void setup()
        {
            blur_program = shaders::create_program("fs_blur_vert", "partial_blur_frag");

            u_blur_texture = glGetUniformLocation(blur_program, "tex");
            u_blur_mask_texture = glGetUniformLocation(blur_program, "mask_tex");
            u_blur_radius = glGetUniformLocation(blur_program, "blur_radius");
            u_blur_size = glGetUniformLocation(blur_program, "tex_size");
            u_blur_tint_color = glGetUniformLocation(blur_program, "tint_color");
            u_blur_tint_strength = glGetUniformLocation(blur_program, "tint_strength");
            u_blur_quality = glGetUniformLocation(blur_program, "sample_count");

            constexpr float vertices_blur[] {
                0.0F, 0.0F, 0.0F, 1.0F,
                1.0F, 0.0F, 1.0F, 1.0F,
                1.0F, 1.0F, 1.0F, 0.0F,
                0.0F, 1.0F, 0.0F, 0.0F
            };

            const unsigned int indices[] = {
                0, 1, 2,
                2, 3, 0
            };

            glGenBuffers(1, &blur_vbo);
            glGenBuffers(1, &blur_ebo);
            glGenVertexArrays(1, &blur_vao);

            glBindVertexArray(blur_vao);

            glBindBuffer(GL_ARRAY_BUFFER, blur_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_blur), vertices_blur, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blur_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        }
    };


}
