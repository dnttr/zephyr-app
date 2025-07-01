//
// Created by Damian Netter on 01/07/2025.
//

#pragma once

#include "ZCApp/graphics/effects/partial_blur.hpp"
#include "ZCApp/graphics/objects/background.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/scene/scene.hpp"

namespace zc_app
{
    class main_scene final : public scene
    {
        colour tint_color{150, 150, 150};

        partial_blur blur_effect{};
        background bg{};

        rectangle rect{container(0, 0, 200, 540), colour(50, 50, 50, 10), 20.0F};

        int scene_width = 0;
        int scene_height = 0;
    public:
        void initialize() override
        {
            blur_effect.setup();

            bg.setup();
        }

        void render() override
        {
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            blur_effect.capture_and_blur(
                scene_width, scene_height,
                tint_color,
                [this]
                {
                    bg.draw(scene_width, scene_height);
                },
                [this]() {
                    rect.draw();
                }
            );
        }

        void update() override
        {
        }

        void resize(int width, int height) override
        {
            scene_width = width;
            scene_height = height;

            bg.reshape(width, height);
            blur_effect.reshape(width, height);
        }

        void destroy() override
        {
        }
    };
}
