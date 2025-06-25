//
// Created by Damian Netter on 25/06/2025.
//

#pragma once

#include "texture.hpp"
#include "ZCApp/graphics/shapes/container.hpp"
#include "ZCApp/graphics/utils/circle_util.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

namespace zc_app
{
    class fan_texture final : public texture
    {
        const int segments;
        const float radius;

    public:
        fan_texture(std::string name, const container &container, const float radius, const int segments) : texture(std::move(name), container), segments(segments), radius(radius)
        {
        }

        void setup() override
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

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);
        }

        void render() override
        {
            glBindVertexArray(vao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_texture);

            const GLsizei vertexCount = 1 + (segments + 1);
            glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
            glBindVertexArray(0);
        }
    };
}
