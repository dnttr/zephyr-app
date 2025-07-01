//
// Created by Damian Netter on 27/06/2025.
//

#pragma once

#include <glm/glm.hpp>

#include "ZCApp/graphics/fonts/font_manager.hpp"
#include "ZCApp/graphics/objects/text/text_style.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"

namespace zc_app
{
    class colour;

    class font_renderer
    {
    public:
        struct text_properties
        {
            float time;
            float smoothing;
            glm::vec2 text_position;

            glm::vec4 text_color;

            int text_shadow_enable;
            float _pad0[3]; // Padding
            glm::vec4 text_shadow_color;
            glm::vec2 text_shadow_offset;

            int text_outline_enable;
            float _pad1[1]; // Padding
            glm::vec4 text_outline_color;
            float text_outline_width;

            int text_glow_enable;
            float text_glow_radius;
            float text_glow_intensity;
            glm::vec4 text_glow_color;

            int text_rainbow_enable;
            float text_rainbow_speed;
            float text_rainbow_variation;
            float text_rainbow_saturation;
        };

        struct transform_properties
        {
            glm::mat4 projection_matrix;
            float time;
            int total_characters_amount;
            float text_animation_begin;
            float text_animation_end;
            float text_animation_speed;
            float text_magnification;
            float _pad0[3]; //Padding
        };

    private:
        inline static GLuint program;

        static inline GLuint text_ubo;
        static inline GLuint transform_ubo;

        static void update_text_properties(const text_properties &properties);
        static void update_transform_properties(const transform_properties &properties);

        static void setup_ubo();

    public:
        static inline GLint u_atlas;

        struct geometry
        {
            GLuint vao;
            GLuint vbo;
            GLuint ebo;
        };

        struct properties
        {
            font_manager::font font;

            float x;
            float y;

            float text_scale;

            int total_characters;

            std::vector<std::string> lines;
            std::chrono::steady_clock::time_point startTime;
        };

        ~font_renderer();

        static GLuint get_program();

        static void build_shader();

        static geometry build_geometry();

        static void set_parameters(const text_style &style,
                                   text_properties &text_props,
                                   transform_properties &transform_props,
                                   const properties &props);

        static void render(properties &props,
                           text_properties &text_props,
                           transform_properties &transform_props,
                           const geometry &geom);
    };
}
