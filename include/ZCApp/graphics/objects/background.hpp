//
// Created by Damian Netter on 30/06/2025.
//

#pragma once

#include <OpenGL/gl3.h>

namespace zc_app
{
    class background
    {
        GLuint point_vbo{}, point_instanced_vbo{}, line_vbo{};
        GLuint point_vao{}, line_vao{};
        GLuint point_ebo{}, line_ebo{};

        GLuint point_program{}, line_program{};

        GLint u_point_projection{};
        GLint u_point_ball_radius{};
        GLint u_point_color{};

        bool need_update = true;

        void fetch_uniforms();
    public:
        background() = default;

        void setup();

        void render();
        void reshape() const;

        ~background();
    };
}