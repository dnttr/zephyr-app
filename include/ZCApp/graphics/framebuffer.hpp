//
// Created by Damian Netter on 29/06/2025.
//

#pragma once

#include <OpenGL/gl3.h>

namespace zc_app
{
    class framebuffer
    {
        static inline GLuint program{};

        static inline GLint u_projection{};
        static inline GLint u_texture{};
        static inline GLint u_position{};
        static inline GLint u_size{};
        static inline GLint u_alpha{};

        GLuint fbo{};
        GLuint texture{};
        GLuint rbo{};

        GLuint vao{};
        GLuint vbo{};
        GLuint ebo{};

        int width = 0;
        int height = 0;

        static void setup_program();
        static void fetch_uniforms();
        static void setup_uniforms(float x, float y, float width, float height);

        bool need_update = true;

    public:
        framebuffer() = default;
        framebuffer(const framebuffer &) = delete;
        framebuffer &operator=(const framebuffer &) = delete;

        ~framebuffer();

        void draw();

        void setup(int width, int height);

        void resize(int new_width, int new_height);

        void bind() const;

        void unbind();

        void destroy();

        [[nodiscard]] GLuint get_texture() const;

        [[nodiscard]] GLuint get_id() const;
    };
}
