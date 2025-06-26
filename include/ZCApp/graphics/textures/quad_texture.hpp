//
// Created by Damian Netter on 25/06/2025.
//

#pragma once

#include <algorithm>
#include <string>

#include "texture.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"

namespace zc_app
{
    class quad_texture final : public texture
    {
    public:
        quad_texture(std::string name, const container &container) : texture(std::move(name), container)
        {
        }

        void setup() override
        {
            const unsigned int indices[] = {
                0, 1, 2,
                2, 3, 0
            };

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &ebo);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);

            constexpr float vertices[] = {
                0.0F, 0.0F, 0.0F,  0.0f, 0.0f,
                1.0F, 0.0F, 0.0F,  1.0f, 0.0f,
                1.0F, 1.0F, 0.0F,  1.0f, 1.0f,
                0.0F, 1.0F, 0.0F,  0.0f, 1.0f
            };


            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            glBindVertexArray(0);
        }
    };
}
