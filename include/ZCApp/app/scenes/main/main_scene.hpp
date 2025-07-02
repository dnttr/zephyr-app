//
// Created by Damian Netter on 01/07/2025.
//

#pragma once

#include "ZCApp/graphics/effects/partial_blur.hpp"
#include "ZCApp/graphics/objects/background.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/graphics/scene/scene.hpp"
#include "ZCApp/graphics/textures/fan_texture.hpp"
#include "ZCApp/graphics/textures/texture.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

#define SPACE 10

namespace zc_app
{
    class main_scene final : public scene
    {
        colour tint_color{100, 100, 100};

        fan_texture avatar{"avatar.png", 24.0F, 32};

        partial_blur blur_effect{};
        background bg{};

        rectangle user_list_rect{colour (255, 0, 0), colour(0, 0, 0, 200), 1, 12.5F};

        rectangle user_name_rect{colour (255, 0, 0), colour(0, 0, 0, 200), 1, 12.5F};
        rectangle user_chat_rect{colour (130, 255, 255), colour(0, 0, 0, 200), 1, 12.5F};
        rectangle user_input_rect{colour (130, 130, 130, 50), colour(0, 0, 0, 200), 1, 12.5F};
        rectangle user_description_rect{colour (130, 130, 130, 50), colour(0, 0, 0, 200), 1, 12.5F};
        colour bg_color{37, 37, 41};

        text user_name, description;

        int scene_width = 0;
        int scene_height = 0;
    public:
        void initialize(int scene_width, int scene_height) override
        {
            float list_end_x = scene_width / 6;
            float name_x = SPACE * 2 + list_end_x;
            float name_end_y = scene_height / 10;

            user_list_rect.set_container(container(SPACE, SPACE, list_end_x, scene_height - SPACE * 2));
            user_chat_rect.set_container(container(SPACE * 2 + list_end_x, SPACE * 2 + name_end_y, scene_width - SPACE * 3 - list_end_x, scene_height - SPACE * 3 - name_end_y));

            user_name_rect.set_container(container(name_x, SPACE, scene_width - name_x - SPACE , name_end_y));
            user_input_rect.set_container(container(name_x, scene_height - SPACE - name_end_y, scene_width - name_x - SPACE, name_end_y));

            const float description_end_y = name_end_y - SPACE;
            user_description_rect.set_container(container(SPACE * 2, SPACE * 2, list_end_x - SPACE * 2, description_end_y));

            avatar.set_container(container(SPACE * 4.5f, description_end_y, 2.5f, 2.5f));
            avatar.setup();

            text_style normal;
            normal.text_size_magnification = 0.09F;
            normal.text_color = colour(245, 245, 245, 255);
            normal.outline_width = 1.0F;
            normal.outline_color = colour(0, 0, 0, 200);
            normal.outline_enable = true;
            normal.shadow_enable = true;
            normal.shadow_offset = {4.0F, 4.0F};
            user_name.initialize("User name", container(avatar.get_container().get_x() + avatar.get_container().get_width() + SPACE * 1.5, description_end_y - SPACE * 1.25), font_manager::get_font("Roboto-Regular"), normal);
            normal.reset();
            normal.text_size_magnification = 0.06F;
            normal.rainbow_enable = true;
            description.initialize("Description\nHello there", container(avatar.get_container().get_x() + avatar.get_container().get_width() + SPACE * 1.5, description_end_y - SPACE * 1.5 * 0.09), font_manager::get_font("Roboto-Regular"), normal);

            bg.setup();
        }

        void render() override
        {
            blur_effect.setup();

            glClear(GL_COLOR_BUFFER_BIT);

            blur_effect.capture_and_blur(
                scene_width, scene_height,
                tint_color,
                [this]
                {
                    bg.draw(scene_width, scene_height);
                },
                [this]() {
                    user_list_rect.draw();
                    user_name_rect.draw();
                    user_chat_rect.draw();
                }, bg_color
            );

            glEnable(GL_BLEND);
            user_description_rect.draw();
            user_input_rect.draw();
            glDisable(GL_BLEND);
            avatar.draw();

            user_name.render();
            description.render();
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
