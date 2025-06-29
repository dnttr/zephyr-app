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

        GLuint vao{}, vbo{}, ebo{};

        container dimension;
        colour color;
        float radius;

    public:
        rectangle(const container container, const colour shape_colour, const float radius = 0.0F) :
            shape("rectangle_vert", "rectangle_frag"), dimension(container), color(shape_colour), radius(radius)
        {
        }

        ~rectangle() override;

        [[nodiscard]] container &get_container();

        void set_container(const container &new_container);

        [[nodiscard]] colour &get_colour();

        void set_colour(const colour &new_colour);

        [[nodiscard]] float get_radius() const;

        void set_radius(float new_radius);

    protected:
        void setup_uniforms() override;

        void setup_shape() override;

        void render() override;
    };
}
