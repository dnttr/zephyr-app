//
// Created by Damian Netter on 29/06/2025.
//

#include "ZCApp/graphics/framebuffer.hpp"

#include <stdexcept>

#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"
#include "ZCGKit/external.hpp"

namespace zc_app
{
    void framebuffer::setup(const int width, const int height)
    {
        if (fbo == 0)
        {
            this->width = width;
            this->height = height;

            glGenFramebuffers(1, &fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);

            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width, this->height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

            if (status != GL_FRAMEBUFFER_COMPLETE)
            {
                throw std::runtime_error("Framebuffer is not complete!");
            }

            if (program == 0)
            {
                setup_program();
                fetch_uniforms();
            }

            glGenBuffers(1, &vbo);
            glGenBuffers(1, &ebo);
            glGenVertexArrays(1, &vao);

            const unsigned int indices[] =
            {
                0, 1, 2,
                2, 3, 0
            };

            constexpr float vertices[] = {
                0.0F, 0.0F, 0.0f, 0.0F, 1.0F,
                1.0F, 0.0F, 0.0F, 1.0F, 1.0F,
                1.0F, 1.0F, 0.0F, 1.0F, 0.0F,
                0.0F, 1.0F, 0.0F, 0.0F, 0.0F
            };

            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                                  reinterpret_cast<void *>(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindVertexArray(0);
        }
    }

    void framebuffer::setup_program()
    {
        program = shaders::create_program("texture_vert", "texture_frag");

        if (program == 0)
        {
            throw std::runtime_error("Failed to create shader program for framebuffer.");
        }
    }

    void framebuffer::fetch_uniforms()
    {
        u_projection = glGetUniformLocation(program, "projection_matrix");
        u_texture = glGetUniformLocation(program, "tex");
        u_position = glGetUniformLocation(program, "tex_position");
        u_size = glGetUniformLocation(program, "tex_size");
        u_alpha = glGetUniformLocation(program, "tex_alpha");
    }

    void framebuffer::setup_uniforms(const float x, const float y, const float width, const float height)
    {
        glUniformMatrix4fv(u_projection, 1, GL_FALSE, perspective_util::get_projection_matrix());

        glUniform1i(u_texture, 0);
        glUniform2f(u_position, x, y);
        glUniform2f(u_size, width, height);
        glUniform1f(u_alpha, 1.0f);
    }

    framebuffer::~framebuffer()
    {
        destroy();
    }

    void framebuffer::draw()
    {
        if (fbo != 0)
        {
            glUseProgram(program);
            glActiveTexture(GL_TEXTURE0);

            if (need_update)
            {
                auto [width, height] = perspective_util::get_effective_virtual_dimensions();

                setup_uniforms(0.0f, 0.0f, width, height);

                need_update = false;
            }

            glBindTexture(GL_TEXTURE_2D, texture);
            glBindVertexArray(vao);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            glBindVertexArray(0);
            glUseProgram(0);
        }
    }

    void framebuffer::resize(const int width, const int height)
    {
        if (this->width == width && this->height == height)
        {
            return;
        }

        this->width = width;
        this->height = height;

        if (fbo != 0)
        {
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width, this->height);
        }
    }

    void framebuffer::bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    void framebuffer::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void framebuffer::destroy()
    {
        if (fbo != 0 && zcg_kit::external::is_context_valid())
        {
            glDeleteFramebuffers(1, &fbo);
            glDeleteRenderbuffers(1, &rbo);
            glDeleteTextures(1, &texture);

            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            glDeleteVertexArrays(1, &vao);

            rbo = 0;
            fbo = 0;
            ebo = 0;
            vbo = 0;
            vao = 0;
            texture = 0;
        }
    }

    GLuint framebuffer::get_texture() const
    {
        return texture;
    }

    GLuint framebuffer::get_id() const
    {
        return fbo;
    }

    void multi_attachment_framebuffer::setup(int width, int height, int attachment_count)
    {
        if (fbo == 0)
        {
            this->width = width;
            this->height = height;

            glGenFramebuffers(1, &fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            textures.resize(attachment_count);
            glGenTextures(attachment_count, textures.data());

            std::vector<GLenum> draw_buffers(attachment_count);

            for (int i = 0; i < attachment_count; i++)
            {
                glBindTexture(GL_TEXTURE_2D, textures[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
                draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
            }

            glDrawBuffers(attachment_count, draw_buffers.data());

            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);

            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                throw std::runtime_error("Multi-attachment framebuffer is not complete!");
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    void multi_attachment_framebuffer::bind(const int attachment_index) const
    {
        const GLenum buffers[] = {static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + attachment_index)};

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glDrawBuffers(1, buffers);
    }

    void multi_attachment_framebuffer::bind_attachments() const
    {
        std::vector<GLenum> buffers(textures.size());

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        for (size_t i = 0; i < textures.size(); i++)
        {
            buffers[i] = GL_COLOR_ATTACHMENT0 + i;
        }

        glDrawBuffers(GL_SIZEI_C(textures.size()), buffers.data());
    }

    void multi_attachment_framebuffer::resize(const int width, const int height)
    {
        if (this->width == width && this->height == height)
        {
            return;
        }

        this->width = width;
        this->height = height;

        if (fbo != 0)
        {
            for (const GLuint tex : textures)
            {
                glBindTexture(GL_TEXTURE_2D, tex);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            }

            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width, this->height);
        }
    }

    void multi_attachment_framebuffer::bind_textures(int start_unit) const
    {
        for (size_t i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + start_unit + i);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
        }
    }

    void multi_attachment_framebuffer::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint multi_attachment_framebuffer::get_texture(int attachment_index) const
    {
        return textures[attachment_index];
    }

    GLuint multi_attachment_framebuffer::get_id() const
    {
        return fbo;
    }

    multi_attachment_framebuffer::~multi_attachment_framebuffer()
    {
        destroy();
    }

    void multi_attachment_framebuffer::destroy()
    {
        if (fbo != 0)
        {
            glDeleteFramebuffers(1, &fbo);
            glDeleteRenderbuffers(1, &rbo);
            glDeleteTextures(GL_SIZEI_C(textures.size()), textures.data());

            fbo = 0;
            rbo = 0;

            textures.clear();
        }
    }
}
