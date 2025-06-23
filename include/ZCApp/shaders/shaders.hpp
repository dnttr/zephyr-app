//
// Created by Damian Netter on 18/05/2025.
//

#pragma once

#include <map>
#include <OpenGL/gl3.h>
#include <ZNBKit/internal/wrapper.hpp>

namespace zc_app
{
    class shaders
    {
    public:
        /**
         * It fetches the vertex and fragment shader source code from the shader manager.
         */
        static GLuint create_program(const std::string &vertex_name, const std::string &fragment_name);

        static GLuint create(const char *source, GLuint type);

        /**
         * It is crucial to ensure that shaders are deleted when no longer needed, as this method does not delete them.
         */
        static GLuint compile(GLuint vertex_shader, GLuint fragment_shader);

        static void delete_shader(GLuint shader);

        class manager
        {
            static std::map<std::string, std::string> shaders;
        public:

            static void add_shader(const std::string &name, const std::string &source);

            static std::string get_source(const std::string &name);

            static bool shader_exists(const std::string &name);
        };
    };
}
