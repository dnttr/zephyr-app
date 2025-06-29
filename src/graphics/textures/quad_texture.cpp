//
// Created by Damian Netter on 26/06/2025.
//

#include "ZCApp/graphics/textures/quad_texture.hpp"

#include "ZCGKit/external.hpp"

namespace zc_app
{
    void quad_texture::setup()
    {
        const unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

        constexpr float vertices[] = {
            0.0F, 0.0F, 0.0F, 0.0f, 0.0f,
            1.0F, 0.0F, 0.0F, 1.0f, 0.0f,
            1.0F, 1.0F, 0.0F, 1.0f, 1.0f,
            0.0F, 1.0F, 0.0F, 0.0f, 1.0f
        };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void quad_texture::render()
    {
        glBindVertexArray(vao);

        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, m_texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
    }

    quad_texture::~quad_texture()
    {
        if (zcg_kit::external::is_context_valid())
        {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
        }
    }
}