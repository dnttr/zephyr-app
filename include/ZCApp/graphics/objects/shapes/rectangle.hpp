//
// Created by Damian Netter on 23/06/2025.
//

#pragma once

#include "container.hpp"
#include "shape.hpp"
#include "ZCApp/graphics/utils/colour.hpp"

namespace zc_app
{
    class rectangle final : public shape
    {
        GLint u_size{};
        GLint u_radius{};
        GLint u_outline_color{};
        GLint u_outline_width{};
        GLint u_outline_opacity{};

        float outline_width = 0;
        colour outline_color{0, 0, 0, 0};

        GLuint vao{}, vbo{}, ebo{};

        container dimension;
        colour color;
        float radius;

    public:
        rectangle(const container container, const colour shape_colour, float outline_width, const colour outline_color,
                  const float radius = 0.0F) :
            shape("rectangle_vert", "rectangle_frag"), outline_width(outline_width), outline_color(outline_color),
            dimension(container), color(shape_colour), radius(radius)
        {
        }

        explicit rectangle(const colour shape_colour, const float radius = 0.0F) :
            shape("rectangle_vert", "rectangle_frag"), outline_width(0.0F), outline_color(colour(0, 0, 0, 0)),
            dimension(0.0F, 0.0F, 100.0F, 100.0F), color(shape_colour), radius(radius)
        {
        }

        explicit rectangle(const colour shape_colour, const colour outline_color, const float outline_width, const float radius = 0.0F) :
            shape("rectangle_vert", "rectangle_frag"), outline_width(outline_width), outline_color(outline_color),
            dimension(0.0F, 0.0F, 100.0F, 100.0F), color(shape_colour), radius(radius)
        {
        }

        ~rectangle() override;

        [[nodiscard]] container &get_container();

        void set_container(const container &new_container);

        [[nodiscard]] colour &get_colour();

        void set_colour(const colour &new_colour);

        [[nodiscard]] float get_radius() const;

        void set_radius(float new_radius);

        void set_outline(float width, const colour &color, float opacity = 0.0F);

        [[nodiscard]] float get_outline_width() const;

        [[nodiscard]] colour get_outline_color() const;

        [[nodiscard]] float get_outline_opacity() const;

    protected:
        void setup_uniforms() override;

        void setup_shape() override;

        void render() override;
    };
}
