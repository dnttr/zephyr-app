//
// Created by Damian Netter on 26/06/2025.
//

#include "ZCApp/graphics/textures/fan_texture.hpp"

#include "ZCGKit/external.hpp"

namespace zc_app
{
    void fan_texture::setup()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

        const auto circle_vertices = circle_util::generate_circle_vertices(radius, segments);
        const auto size = circle_vertices.size() * sizeof(float);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), circle_vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void fan_texture::render()
    {
        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        const GLsizei vertexCount = 1 + (segments + 1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(0);
    }

    fan_texture::~fan_texture()
    {
        if (zcg_kit::external::is_context_valid())
        {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
        }
    }
}