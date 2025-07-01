//
// Created by Damian Netter on 01/07/2025.
//

#pragma once

#include <OpenGL/gl3.h>
#include <__functional/function.h>

#include "ZCApp/graphics/framebuffer.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"

namespace zc_app
{
    class partial_blur final
    {
    public:
        partial_blur() = default;

        GLuint blur_program{};
        GLuint pass_program{};

        GLuint blur_vao{};
        GLuint blur_vbo{};
        GLuint blur_ebo{};

        multi_attachment_framebuffer multi_fbo;

        GLint u_blur_texture{};
        GLint u_blur_mask_texture{};
        GLint u_blur_radius{};
        GLint u_blur_size{};
        GLint u_blur_tint_color{};
        GLint u_blur_tint_strength{};
        GLint u_blur_quality{};
        GLint u_pass_screen_texture{};
        GLint u_blur_alpha{};
        GLint u_blur_time{};

        bool need_update = true;

        void setup();

        void draw_quad() const;

        void render_with_sources(GLuint background_texture, GLuint mask_texture, int width, int height,
                                 const colour &tint_color);

        void capture_and_blur(int width, int height, const colour &tint_color, const std::function<void()> &draw_background,
                              const std::function<void()> &draw_mask);

        void reshape(int width, int height);
    };
}
