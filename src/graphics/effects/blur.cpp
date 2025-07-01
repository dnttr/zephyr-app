//
// Created by Damian Netter on 01/07/2025.
//

#include "ZCApp/graphics/effects/blur.hpp"

#include <functional>

namespace zc_app
{
    void blur::setup()
    {
        blur_program = shaders::create_program("fs_blur_vert", "partial_blur_frag");

        u_blur_texture = glGetUniformLocation(blur_program, "tex");
        u_blur_mask_texture = glGetUniformLocation(blur_program, "mask_tex");
        u_blur_radius = glGetUniformLocation(blur_program, "radius");
        u_blur_size = glGetUniformLocation(blur_program, "size");
        u_blur_tint_color = glGetUniformLocation(blur_program, "tint_color");
        u_blur_tint_strength = glGetUniformLocation(blur_program, "tint_strength");
        u_blur_quality = glGetUniformLocation(blur_program, "quality");
        u_pass_screen_texture = glGetUniformLocation(pass_program, "screenTexture");

        constexpr float vertices_blur[]{
            0.0F, 0.0F, 0.0F, 0.0F,
            1.0F, 0.0F, 1.0F, 0.0F,
            1.0F, 1.0F, 1.0F, 1.0F,
            0.0F, 1.0F, 0.0F, 1.0F
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

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                              reinterpret_cast<void *>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        this->need_update = true;
    }

    void blur::draw_quad() const
    {
        glBindVertexArray(blur_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void blur::render_with_sources(const GLuint background_texture, const GLuint mask_texture,
                                   const int width, const int height, const colour &tint_color)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(blur_program);

        if (need_update)
        {
            glUniform1i(u_blur_texture, 0);
            glUniform1i(u_blur_mask_texture, 1);
            glUniform1f(u_blur_radius, 90.0f);
            glUniform2f(u_blur_size, static_cast<float>(width), static_cast<float>(height));
            glUniform1f(u_blur_tint_strength, 0.08f);
            glUniform3f(u_blur_tint_color, tint_color.get_red_direct(), tint_color.get_green_direct(),
                        tint_color.get_blue_direct());
            need_update = false;
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, background_texture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mask_texture);

        draw_quad();
    }

    void blur::capture_and_blur(const int width, const int height, const colour &tint_color,
                                std::function<void()> const &draw_background,
                                std::function<void()> const &draw_mask)
    {
        glDisable(GL_BLEND);

        multi_fbo.setup(width, height, 2);

        multi_fbo.bind(0);
        glViewport(0, 0, width, height);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        draw_background();

        multi_fbo.bind(1);
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        draw_mask();

        multi_fbo.unbind();

        render_with_sources(multi_fbo.get_texture(0), multi_fbo.get_texture(1),
                            width, height, tint_color);
    }

    void blur::reshape(const int width, const int height)
    {
        multi_fbo.resize(width, height);

        this->need_update = true;
    }
}
