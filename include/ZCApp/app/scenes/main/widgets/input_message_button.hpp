//
// Created by Damian Netter on 03/07/2025.
//

#pragma once
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"

namespace zc_app
{
    class input_message_button
    {
        rectangle input_area_glass{glass_tint, glass_border, 1, BORDER_RADIUS};
        rectangle message_input{colour(255, 255, 255, 30), accent_color, 2, 25.0F};

        text input_placeholder{};

        float max_animation_width = 4.0f;
        float current_animation_width = 0.0f;
        float animation_speed = 600.0f;

        float default_position_x = 0.0f;
        float default_position_y = 0.0f;

        bool is_hovered = false;

    public:
        void initialize(const float chat_x, float input_height, float scene_height, float scene_width)
        {
            input_area_glass.set_container(container(
                chat_x, scene_height - input_height - PADDING,
                scene_width - chat_x - PADDING,
                input_height
            ));

            message_input.set_container(container(
                chat_x + PADDING * 2, scene_height - input_height - PADDING + input_height / 2 - 25,
                scene_width - chat_x - PADDING * 6 - 120,
                50.0f
            ));

            text_style placeholder_style{};
            placeholder_style.text_color = colour(0, 0, 0, 180);
            placeholder_style.text_size_magnification = 0.1F;

            constexpr float font_size = 128 * 0.1 / 2;

            default_position_x = message_input.get_container().get_x() + 20;
            default_position_y = message_input.get_container().get_y() + message_input.get_container().get_height() / 2 - font_size;
            input_placeholder.initialize(
                "Type a message...",
                container(default_position_x,
                          default_position_y),
                font_manager::get_font("Roboto-Regular"),
                placeholder_style
            );
        }

        void draw_glassy()
        {
            input_area_glass.draw();
        }

        void draw()
        {
            message_input.draw();
            input_placeholder.render();

            const float deltaTime = time_util::get_delta_time();

            if (is_hovered)
            {
                float increment_value = animation_speed * deltaTime;


                if (current_animation_width < max_animation_width)
                {
                    current_animation_width += increment_value;
                }
                else
                {
                    current_animation_width = max_animation_width;
                }
            }
            else
            {
                current_animation_width = std::max(current_animation_width - animation_speed * deltaTime, 0.0F);
            }

            input_placeholder.set_properties_position(default_position_x + current_animation_width, default_position_y);
        }

        void on_mouse_move(zcg_mouse_pos_t pos)
        {
            const float scale = perspective_util::get_current_display_config().scale / 2;

            const float mouse_x = round(pos.x / scale);
            const float mouse_y = round(pos.y / scale);
            const auto container = message_input.get_container();

            if (mouse_x >= container.get_x() && mouse_x <= container.get_x() + container.get_width())
            {
                if (mouse_y >= container.get_y() && mouse_y <= container.get_y() + container.get_height())
                {
                    is_hovered = true;
                }
                else
                {
                    is_hovered = false;
                }
            }
        }
    };
}
