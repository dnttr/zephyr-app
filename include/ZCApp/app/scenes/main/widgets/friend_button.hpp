//
// Created by Damian Netter on 02/07/2025.
//

#pragma once
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"

namespace zc_app
{
    class friend_button
    {
    public:
        rectangle button_shape{colour(130, 130, 130, 40), colour(0, 0, 0, 200), 1, 7.5F};

        void setup(const container container)
        {
            button_shape.set_container(container);
        }

        void render()
        {
            button_shape.draw();
        }

        container &get_container()
        {
            return button_shape.get_container();
        }

        void on_click()
        {
        }
    };
}
