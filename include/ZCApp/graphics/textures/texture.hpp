//
// Created by Damian Netter on 25/06/2025.
//

#pragma once

#include <string>
#include <utility>
#include <OpenGL/gl3.h>

#include "texture_loader.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

namespace zc_app
{
    class texture
    {
        void fetch_uniforms()
        {
            m_projection_uniform = glGetUniformLocation(m_program, "m_projection_matrix");
            m_texture_uniform = glGetUniformLocation(m_program, "m_texture");
            m_position_uniform = glGetUniformLocation(m_program, "m_texture_position");
            m_size_uniform = glGetUniformLocation(m_program, "m_texture_size");
        }

        bool update = false;

    protected:
        GLuint vao{}, vbo{}, ebo{};

        inline static int m_scale = 4;

        GLuint m_texture{};
        GLuint m_program{};

        container m_container;

        std::string m_name;

        GLint m_projection_uniform{};
        GLint m_texture_uniform{};
        GLint m_position_uniform{};
        GLint m_size_uniform{};

        virtual void setup_uniforms(const float x, const float y, const float width, const float height)
        {
            glUniformMatrix4fv(m_projection_uniform, 1, GL_FALSE, perspective_util::get_projection_matrix());

            glUniform1i(m_texture_uniform, 0);
            glUniform2f(m_position_uniform, x, y);
            glUniform2f(m_size_uniform, width, height);
        }

        virtual void render() = 0;

        virtual void setup() = 0;

    public:
        virtual ~texture() = default;

        texture(std::string name, const container &container) : m_container(container), m_name(std::move(name))
        {
        }

        void draw()
        {
            if (m_texture == 0 || m_program == 0)
            {
                setup();

                auto [tex_id, tex_info] = texture_loader::get(m_name, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TRUE, MAX_ANISOTROPIC);

                const int scaled_width = tex_info.width / m_scale;
                const int scaled_height = tex_info.height / m_scale;

                m_container.set_width(static_cast<float>(scaled_width));
                m_container.set_height(static_cast<float>(scaled_height));

                m_texture = tex_id;
                m_program = shaders::create_program("texture_vert", "texture_frag");
                fetch_uniforms();

                update = true;
            }

            glUseProgram(m_program);

            if (update)
            {
                setup_uniforms(m_container.get_x(), m_container.get_y(), m_container.get_width(), m_container.get_height());

                update = false;
            }

            render();

            glUseProgram(0);
        }

        void reshape()
        {
            update = true;
        }
    };
}
