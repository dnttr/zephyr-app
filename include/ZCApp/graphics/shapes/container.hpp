//
// Created by Damian Netter on 24/06/2025.
//

#pragma once

namespace zc_app
{
    class container
    {
        float x, y, width, height;

    public:
        container(const float x, const float y, const float width, const float height) : x(x), y(y), width(width), height(height)
        {
        }

        container (const float x, const float y) : x(x), y(y), width(0.0f), height(0.0f)
        {
        }

        container() : x(0.0f), y(0.0f), width(0.0f), height(0.0f)
        {
        }

        void set_x(const float new_x)
        {
            x = new_x;
        }

        void set_y(const float new_y)
        {
            y = new_y;
        }

        void set_width(const float new_width)
        {
            width = new_width;
        }

        void set_height(const float new_height)
        {
            height = new_height;
        }

        [[nodiscard]] float get_x() const
        {
            return x;
        }

        [[nodiscard]] float get_y() const
        {
            return y;
        }

        [[nodiscard]] float get_width() const
        {
            return width;
        }

        [[nodiscard]] float get_height() const
        {
            return height;
        }
    };
}
