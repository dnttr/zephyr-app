//
// Created by Damian Netter on 27/06/2025.
//

#pragma once
#include "font_manager.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCApp/graphics/utils/colour.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

namespace zc_app
{
    class colour;

    class font_renderer
    {
        inline static GLuint ebo{};
        inline static GLuint vbo{};
        inline static GLuint vao{};

        inline static GLuint program = -1;

        inline static GLint projection;
        inline static GLint fontAtlas;
        inline static GLint textColor;
        inline static GLint outlineColor;
        inline static GLint shadowColor;
        inline static GLint smoothing;
        inline static GLint outlineWidth;
        inline static GLint shadowOffset;

        inline static bool setup_x;
    public:
        static void setup()
        {
            program = shaders::create_program("text_vert", "text_frag");

            projection = glGetUniformLocation(program, "projection");
            fontAtlas = glGetUniformLocation(program, "fontAtlas");
            textColor = glGetUniformLocation(program, "textColor");
            outlineColor = glGetUniformLocation(program, "outlineColor");
            shadowColor = glGetUniformLocation(program, "shadowColor");
            smoothing = glGetUniformLocation(program, "smoothing");
            outlineWidth = glGetUniformLocation(program, "outlineWidth");
            shadowOffset = glGetUniformLocation(program, "shadowOffset");

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &ebo);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            setup_x = true;
        }

        static void render(font_manager::font &font, const std::string &text, float x, float y, float f_scale, const colour &color)
        {
            if (!setup_x)
            {
                setup();
            }

            if (program == -1 || program == 0) {
                debug_print_cerr("Invalid shader program: " + std::to_string(program));
                return;
            }

            glUseProgram(program);
            glUniform1i(fontAtlas, 0);

            auto proj = perspective_util::get_projection_matrix();

            glUniformMatrix4fv(projection, 1, GL_FALSE, proj);

            glUniform1f(smoothing, 0.01f);
            glUniform1f(outlineWidth, 0.0f);
            glUniform2f(shadowOffset, 0.0f, 0.0f);
            glUniform4f(outlineColor, 0.0f, 0.0f, 0.0f, 0.5f);
            glUniform4f(shadowColor, 0.0f, 0.0f, 0.0f, 0.5f);

            glUniform4f(textColor, color.get_red_direct(), color.get_green_direct(), color.get_blue_direct(),
                       color.get_alpha_direct());


            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, font.atlas_texture_id);

            std::vector<float> vertices;
            vertices.reserve(text.size() * 6 * 4);

            float cursor_x = x;
            float cursor_y = y;

            for (char c : text)
            {
                auto it = font.characters_map.find(c);

                if (it == font.characters_map.end())
                {
                    continue;
                }

                auto character = it->second;

                float xpos = cursor_x + character.bearing.x * f_scale;
                float ypos = cursor_y - (character.size.y - character.bearing.y) * f_scale;
                float w = character.size.x * f_scale;
                float h = character.size.y * f_scale;

                float tex_x = static_cast<float>(character.atlas_offset.x) / font_loader::ATLAS_WIDTH;
                float tex_y = static_cast<float>(character.atlas_offset.y) / font_loader::ATLAS_HEIGHT;
                float tex_w = static_cast<float>(character.atlas_offset.x + character.size.x) / font_loader::ATLAS_WIDTH;
                float tex_h = static_cast<float>(character.atlas_offset.y + character.size.y) / font_loader::ATLAS_HEIGHT;

                // 1   1
                //
                // 1

                //FIRST TRIANGLE
                // bottom left
                vertices.push_back(xpos);           vertices.push_back(ypos);           vertices.push_back(tex_x);      vertices.push_back(tex_y);
                //top left
                vertices.push_back(xpos);           vertices.push_back(ypos + h);     vertices.push_back(tex_x);      vertices.push_back(tex_h);
                //top right
                vertices.push_back(xpos + w);     vertices.push_back(ypos + h);     vertices.push_back(tex_w);      vertices.push_back(tex_h);

                //SECOND TRIANGLE

                //      2
                //
                // 2    2

                //bottom left
                vertices.push_back(xpos);          vertices.push_back(ypos);            vertices.push_back(tex_x);      vertices.push_back(tex_y);
                //bottom right
                vertices.push_back(xpos + w);    vertices.push_back(ypos);            vertices.push_back(tex_w);      vertices.push_back(tex_y);
                //top right
                vertices.push_back(xpos + w);    vertices.push_back(ypos + h);      vertices.push_back(tex_w);      vertices.push_back(tex_h);

                cursor_x += (character.advance >> 6) * f_scale;
            }

            if (!vertices.empty())
            {

                glBindVertexArray(vao);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);

                GLint bufferSize;
                glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

                if (vertices.size() * sizeof(float) > static_cast<size_t>(bufferSize))
                {
                    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
                }
                else
                {
                    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
                }

                glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 4);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }

            glBindTexture(GL_TEXTURE_2D, 0);
            glUseProgram(0);
        }
    };
}
