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

        bool update = false;

    protected:
        GLuint vao{}, vbo{};

        inline static int m_scale = 4;

        GLuint m_texture{};
        GLuint program{};

        container m_container{};

        std::string m_name;

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
        virtual ~texture() = default;

        texture(std::string name, const container &container) : m_container(container), m_name(std::move(name))
        {
        }

        explicit texture(std::string name) : m_name(std::move(name))
        {
        }

        void set_container(const container &new_container)
        {
            m_container = new_container;
            update = true;
        }

        [[nodiscard]] const container &get_container() const
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

                const float scaled_width = m_container.get_width() > 0
                                               ? m_container.get_width() / m_scale
                                               : tex_info.width / m_scale;
                const float scaled_height = m_container.get_height() > 0
                                                ? m_container.get_height() / m_scale
                                                : tex_info.height / m_scale;

                m_container.set_x(m_container.get_x() + (m_container.get_width() / 2));
                m_container.set_y(m_container.get_y() + (m_container.get_height() / 2));

                m_container.set_width(scaled_width);
                m_container.set_height(scaled_height);

                m_texture = tex_id;
                program = shaders::create_program("texture_vert", "texture_frag");
                fetch_uniforms();

                update = true;
            }

            glUseProgram(program);

            if (update)
            {
                setup_uniforms(m_container.get_x(), m_container.get_y(), m_container.get_width(),
                               m_container.get_height());

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
