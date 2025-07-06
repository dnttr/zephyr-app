#pragma once

#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/graphics/textures/fan_texture.hpp"
#include "ZCApp/app/scenes/utils.hpp"

#include <string>
#include <functional>

namespace zc_app
{
    class friend_button
    {
    public:
        rectangle button_shape{colour(130, 130, 130, 40), colour(0, 0, 0, 200), 1, 7.5F};
        fan_texture avatar_texture{"avatar.png", 2.0F, 32};
        text name_text;
        text status_text;

        std::string conversation_id;
        bool is_hovered = false;

        std::function<void(const std::string&)> on_click_callback;

        friend_button() = default;

        friend_button(const std::string& id, const std::string& name, const std::string& status, const std::string& avatar_path)
            : conversation_id(id), avatar_texture(avatar_path, 2.0F, 32)
        {
            name_text.set_text(name);
            status_text.set_text(status);
        }

        void setup(const container& c)
        {
            button_shape.set_container(c);

            float avatar_size = 40.0f;
            avatar_texture.set_container(container(
                button_shape.get_container().get_x() + PADDING,
                button_shape.get_container().get_y() + (button_shape.get_container().get_height() / 2) - (avatar_size / 2),
                avatar_size, avatar_size
            ));
            avatar_texture.setup();

            text_style name_style;
            name_style.text_size_magnification = 0.07F;
            name_style.text_color = colour(255, 255, 255, 240);
            name_style.shadow_enable = true;
            name_style.shadow_offset = {2.0F, 2.0F};
            name_style.shadow_color = colour(0, 0, 0, 100);

            name_text.initialize(
                name_text.get_text(),
                container(button_shape.get_container().get_x() + PADDING + avatar_size + 10.0f, button_shape.get_container().get_y() + 15),
                font_manager::get_font("Roboto-Medium"),
                name_style
            );

            text_style status_style = name_style;
            status_style.text_size_magnification = 0.05F;
            status_style.text_color = colour(180, 180, 180, 200);
            status_style.shadow_enable = false;

            status_text.initialize(
                status_text.get_text(),
                container(button_shape.get_container().get_x() + PADDING + avatar_size + 10.0f, button_shape.get_container().get_y() + 35),
                font_manager::get_font("Roboto-Regular"),
                status_style
            );
        }

        void set_on_click_callback(std::function<void(const std::string&)> callback)
        {
            on_click_callback = callback;
        }

        void render()
        {
            if (is_hovered) {
                // button_shape.set_fill_color(colour(130, 130, 130, 60)); // Original commented out lines
            } else {
                // button_shape.set_fill_color(colour(130, 130, 130, 40)); // Original commented out lines
            }

            button_shape.draw();
            avatar_texture.draw();
            name_text.render();
            status_text.render();
        }

        container &get_container()
        {
            return button_shape.get_container();
        }

        void on_mouse_down(const zcg_mouse_pos_t& mouse_pos, int button)
        {
            if (button == ZCG_MOUSE_BUTTON_LEFT && zc_app::is_in_area(button_shape.get_container(), mouse_pos))
            {
                if (on_click_callback)
                {
                    on_click_callback(conversation_id);
                }
            }
        }

        void on_mouse_up(const zcg_mouse_pos_t& mouse_pos, int button)
        {
        }

        void on_mouse_move(const zcg_mouse_pos_t& mouse_pos)
        {
            is_hovered = is_in_area(button_shape.get_container(), mouse_pos);
        }
    };
}