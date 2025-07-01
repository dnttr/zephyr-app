//
// Created by Damian Netter on 01/07/2025.
//

#pragma once

#include <OpenGL/gl3.h>

#include "ZCApp/graphics/framebuffer.hpp"

namespace zc_app
{
    class fullscreen_blur
    {
    public:
        fullscreen_blur() = default;
        ~fullscreen_blur();

        void setup();

        void draw(GLuint input_texture, int width, int height);

        void reshape(int width, int height);

    private:
        void fetch_uniforms();
        void draw_quad() const;

        GLuint blur_program = 0;
        GLuint passthrough_program = 0;

        GLint u_blur_projection = 0;
        GLint u_blur_texture = 0;
        GLint u_blur_radius = 0;
        GLint u_blur_size = 0;
        GLint u_blur_tint_color = 0;
        GLint u_blur_tint_strength = 0;
        GLint u_blur_quality = 0;
        GLint u_passthrough_screen_texture = 0;

        GLuint blur_vao = 0;
        GLuint blur_vbo = 0;
        GLuint blur_ebo = 0;

        multi_attachment_framebuffer multi_fbo;

        bool need_update = true;
    };
}
