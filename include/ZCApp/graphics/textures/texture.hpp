//
// Created by Damian Netter on 25/06/2025.
//

#pragma once

#include <iostream>
#include <string>
#include <utility>
#include <OpenGL/gl3.h>

#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCApp/graphics/objects/shapes/container.hpp"
#include "ZCApp/graphics/textures/texture_loader.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

namespace zc_app
{
    class texture
    {
        void fetch_uniforms()
        {
            u_projection = glGetUniformLocation(program, "projection_matrix");
            u_texture = glGetUniformLocation(program, "tex");
            u_position = glGetUniformLocation(program, "tex_position");
            u_size = glGetUniformLocation(program, "tex_size");
            u_alpha = glGetUniformLocation(program, "tex_alpha");
        }

        bool uniforms_need_update = false;
    protected:
        GLuint vao{}, vbo{};

        inline static int m_scale = 4;

        GLuint m_texture{};
        GLuint program{};

        container m_container{};

        std::string m_name;
        texture_loader::texture_info m_tex_info_cache{};


        GLint u_projection{};
        GLint u_texture{};
        GLint u_position{};
        GLint u_size{};
        GLint u_alpha{};

        virtual void setup_uniforms(const float x, const float y, const float width, const float height)
        {
            glUniformMatrix4fv(u_projection, 1, GL_FALSE, perspective_util::get_projection_matrix());

            glUniform1i(u_texture, 0);
            glUniform2f(u_position, x, y);
            glUniform2f(u_size, width, height);
            glUniform1f(u_alpha, 1.0f);
        }

        virtual void render() = 0;

        virtual void setup() = 0;

    public:
        virtual ~texture()
        {
            if (program != 0) glDeleteProgram(program);
            if (m_texture != 0) glDeleteTextures(1, &m_texture);
        }

        texture(std::string name, const container &container) : m_container(container), m_name(std::move(name))
        {
        }

        explicit texture(std::string name) : m_name(std::move(name))
        {
        }

        void set_container(const container &new_container)
        {
            m_container = new_container;
            uniforms_need_update = true;
        }

        [[nodiscard]] container &get_container()
        {
            return m_container;
        }

        void draw()
        {
            if (m_texture == 0 || program == 0)
            {
                setup();

                auto [tex_id, tex_info] = texture_loader::get(m_name, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TRUE,
                                                              MAX_ANISOTROPIC);
                m_texture = tex_id;
                m_tex_info_cache = tex_info;

                program = shaders::create_program("texture_vert", "texture_frag");
                fetch_uniforms();

                uniforms_need_update = true;
            }

            if (m_tex_info_cache.data == nullptr)
            {
                throw std::runtime_error("Texture data is null for texture: " + m_name);
            }

            glUseProgram(program);

            float final_draw_x = m_container.get_x();
            float final_draw_y = m_container.get_y();
            float final_draw_width = m_container.get_width();
            float final_draw_height = m_container.get_height();

            if (final_draw_width > 0)
            {
                final_draw_width /= static_cast<float>(m_scale);
            }
            else if (m_tex_info_cache.width > 0)
            {
                final_draw_width = static_cast<float>(m_tex_info_cache.width) / static_cast<float>(m_scale);
            }

            if (final_draw_height > 0)
            {
                final_draw_height /= static_cast<float>(m_scale);
            }
            else if (m_tex_info_cache.height > 0)
            {
                final_draw_height = static_cast<float>(m_tex_info_cache.height) / static_cast<float>(m_scale);
            }

            final_draw_x += final_draw_width / 2.0f;
            final_draw_y += final_draw_height / 2.0f;

            if (uniforms_need_update)
            {
                setup_uniforms(final_draw_x, final_draw_y, final_draw_width, final_draw_height);
                uniforms_need_update = false;
            }

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_texture);

            render();

            glBindTexture(GL_TEXTURE_2D, 0);
            glUseProgram(0);
        }

        void reshape()
        {
            uniforms_need_update = true;
        }
    };
}
