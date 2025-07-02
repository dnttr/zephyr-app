//
// Created by Damian Netter on 01/07/2025.
//

#include "ZCApp/graphics/effects/fullscreen_blur.hpp"

#include <fstream>
#include <functional>

#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCGKit/external.hpp"

namespace zc_app
{
    framebuffer fbo_1, fbo_2;

    fullscreen_blur::~fullscreen_blur()
    {
        if (zcg_kit::external::is_context_valid())
        {
            glDeleteProgram(blur_program);
            glDeleteProgram(passthrough_program);
            glDeleteBuffers(1, &blur_vbo);
            glDeleteBuffers(1, &blur_ebo);
            glDeleteVertexArrays(1, &blur_vao);

            fbo_1.destroy();
            fbo_2.destroy();
        }
    }

    void fullscreen_blur::setup(int width, int height)
    {
        if (fbo_1.get_texture() == 0 || fbo_2.get_texture() == 0) {
            blur_program = shaders::create_program("fs_blur_vert", "fs_blur_frag");
            passthrough_program = shaders::create_program("fs_blur_vert", "passthrough_frag");

            fetch_uniforms();

            glGenBuffers(1, &blur_vbo);
            glGenBuffers(1, &blur_ebo);
            glGenVertexArrays(1, &blur_vao);

            constexpr float vertices_blur[]{
                0.0F, 0.0F, 0.0F, 1.0F,
                1.0F, 0.0F, 1.0F, 1.0F,
                1.0F, 1.0F, 1.0F, 0.0F,
                0.0F, 1.0F, 0.0F, 0.0F
            };

            const unsigned int indices[] = {
                0, 1, 2,
                2, 3, 0
            };

            glBindVertexArray(blur_vao);

            glBindBuffer(GL_ARRAY_BUFFER, blur_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_blur), vertices_blur, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blur_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
    }

    void fullscreen_blur::fetch_uniforms()
    {
        u_blur_projection = glGetUniformLocation(blur_program, "projection_matrix");

        u_blur_texture = glGetUniformLocation(blur_program, "tex");
        u_blur_radius = glGetUniformLocation(blur_program, "blur_radius");
        u_blur_size = glGetUniformLocation(blur_program, "tex_size");
        u_blur_tint_color = glGetUniformLocation(blur_program, "tint_color");
        u_blur_tint_strength = glGetUniformLocation(blur_program, "tint_strength");
        u_blur_quality = glGetUniformLocation(blur_program, "sample_count");

        u_passthrough_screen_texture = glGetUniformLocation(passthrough_program, "screenTexture");
    }

    void fullscreen_blur::draw_quad() const
    {
        glBindVertexArray(blur_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void fullscreen_blur::draw(int width, int height, std::function<void()> const &render_scene)
    {
        GLint original_fbo;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &original_fbo);

        fbo_1.setup(width, height);
        fbo_2.setup(width, height);

        fbo_1.bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_scene();

        fbo_1.unbind();

        glUseProgram(blur_program);

        if (need_update)
        {
            glUniform3f(u_blur_tint_color, 0.2f, 0.2f, 0.2f);
            glUniform2f(u_blur_size, static_cast<float>(width), static_cast<float>(height));
            glUniform1f(u_blur_radius, 10.0f);
            glUniform1f(u_blur_tint_strength, 0.5f);
            glUniform1i(u_blur_texture, 0);
            glUniform1i(u_blur_quality, 20);
        }

        fbo_2.bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo_1.get_texture());

        draw_quad();
        fbo_2.unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, original_fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(passthrough_program);
        glUniform1i(u_passthrough_screen_texture, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo_2.get_texture());

        draw_quad();

        glUseProgram(0);

        need_update = false;
    }

    void fullscreen_blur::reshape(int width, int height)
    {
        fbo_1.resize(width, height);
        fbo_2.resize(width, height);

        glUseProgram(blur_program);
        glUniform2f(u_blur_size, static_cast<float>(width), static_cast<float>(height));
        glUseProgram(0);

        need_update = true;
    }
}