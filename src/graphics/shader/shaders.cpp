//
// Created by Damian Netter on 23/06/2025.
//

#include "ZCApp/shaders/shaders.hpp"

namespace zc_app
{
    std::map<std::string, std::string> shaders::manager::sources;

    GLuint shaders::create_program(const std::string &vertex_name, const std::string &fragment_name)
    {
        VAR_CONTENT_CHECK(vertex_name);
        VAR_CONTENT_CHECK(fragment_name);

        const auto vertex_source = manager::get_source(vertex_name);
        const auto fragment_source = manager::get_source(fragment_name);

        VAR_CONTENT_CHECK(vertex_source);
        VAR_CONTENT_CHECK(fragment_source);

        const GLuint vertex_shader = create(vertex_source.c_str(), GL_VERTEX_SHADER);
        const GLuint fragment_shader = create(fragment_source.c_str(), GL_FRAGMENT_SHADER);

        return compile(vertex_shader, fragment_shader);
    }

    GLuint shaders::create(const char *source, const GLuint type)
    {
        const GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        int success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE)
        {
            int length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> infoLog(length);

            glGetShaderInfoLog(shader, length, nullptr, infoLog.data());

            glDeleteShader(shader);

            return 0;
        }

        return shader;
    }

    GLuint shaders::compile(const GLuint vertex_shader, const GLuint fragment_shader)
    {
        if (vertex_shader == 0 || fragment_shader == 0)
        {
            return 0;
        }

        const GLuint program = glCreateProgram();

        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);

        int success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (success == GL_FALSE)
        {
            int length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> infoLog(length);

            glGetProgramInfoLog(program, length, nullptr, infoLog.data());

            glDeleteProgram(program);

            return 0;
        }

        return program;
    }

    void shaders::delete_shader(const GLuint shader)
    {
        if (shader != 0)
        {
            glDeleteShader(shader);
        }
    }

    void shaders::manager::add_shader(const std::string &name, const std::string &source)
    {
        sources[name] = source;
    }

    std::string shaders::manager::get_source(const std::string &name)
    {
        if (sources.contains(name))
        {
            return sources.find(name)->second;
        }

        return {};
    }

    bool shaders::manager::shader_exists(const std::string &name)
    {
        return sources.contains(name);
    }
}
