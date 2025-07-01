//
// Created by Damian Netter on 29/06/2025.
//

#pragma once

#include <vector>
#include <OpenGL/gl3.h>

namespace zc_app
{

#define GL_SIZEI_PTR static_cast<GLsizeiptr>
#define GL_SIZEI static_cast<GLsizei>

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

    class multi_attachment_framebuffer
    {
        //  static inline GLuint program{};, for now not used, iwll be added later

        GLuint fbo{};
        GLuint rbo{};

        std::vector<GLuint> textures;

        int width = 0;
        int height = 0;

        bool need_update = true;

    public:
        multi_attachment_framebuffer() = default;
        multi_attachment_framebuffer(const multi_attachment_framebuffer &) = delete;
        multi_attachment_framebuffer &operator=(const multi_attachment_framebuffer &) = delete;

        ~multi_attachment_framebuffer();

        void setup(int width, int height, int attachment_count);

        void bind(int attachment_index) const;

        void bind_attachments() const;

        void bind_textures(int start_unit = 0) const;

        void unbind();

        void resize(int new_width, int new_height);

        [[nodiscard]] GLuint get_texture(int attachment_index) const;

        [[nodiscard]] GLuint get_id() const;

        void destroy();
    };
}
