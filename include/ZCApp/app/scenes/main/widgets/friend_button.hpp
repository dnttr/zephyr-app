#pragma once

#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/graphics/textures/fan_texture.hpp"
#include "ZCApp/app/scenes/utils.hpp"

#include <string>
#include <functional>
#include <utility>

namespace zc_app
{
    static colour default_color{40, 40, 40, 70};
    static colour hovered_colour{40, 40, 40, 100};

    class friend_button
    {
        fan_texture avatar_texture{"avatar.png", 2.5F, 128};

        const float avatar_size = 32.0f;
        bool is_hovered = false;

        text name_text;
        text status_text;

        std::string name;
        std::string status;
        std::string conversation_id;

        std::function<void(const std::string &)> on_click_callback;

    public:
        rectangle button_shape{default_color, colour(0, 0, 0, 240), 1, 30.0F};

        friend_button(std::string id, std::string name, std::string status, const std::string &avatar_path)
            : avatar_texture(avatar_path, 2.0F, 32), name(std::move(name)), status(std::move(status)),
              conversation_id(std::move(id))
        {
        }

        friend_button() = default;

        void setup(const container &c)
        {
            text_style name_style;
            text_style status_style = name_style;

            button_shape.set_container(c);

            name_style.text_size_magnification = 0.099F;
            name_style.text_color = colour(255, 255, 255, 255);
            name_style.shadow_enable = true;

            status_style.text_size_magnification = 0.06F;
            status_style.text_color = colour(235, 235, 235, 255);
            status_style.shadow_enable = false;

            avatar_texture.set_container(container(
                button_shape.get_container().get_x() + (avatar_size + PADDING / 2.0F) / 2.0f,
                button_shape.get_container().get_y() + 2 + (button_shape.get_container().get_height() - PADDING) / 2.0f,
                avatar_size,
                avatar_size
            ));

            avatar_texture.setup();

            name_text.initialize(
                name,
                container(button_shape.get_container().get_x() + PADDING + avatar_size + 10.0f,
                          button_shape.get_container().get_y() + (button_shape.get_container().get_height() - PADDING * 2.0F) / 2.0F),
                font_manager::get_font("Roboto-Medium"),
                name_style
            );

            status_text.initialize(
                status,
                container(button_shape.get_container().get_x() + PADDING + avatar_size + 10.0f,
                          button_shape.get_container().get_y() + (button_shape.get_container().get_height() + PADDING /
                              2.0F) / 2.0F),
                font_manager::get_font("Roboto-Regular"),
                status_style
            );
        }

        void set_on_click_callback(std::function<void(const std::string &)> callback)
        {
            on_click_callback = std::move(callback);
        }

        void render()
        {
            if (is_hovered)
            {
                button_shape.set_colour(hovered_colour);
            }
            else
            {
                button_shape.set_colour(default_color);
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

        void on_mouse_down(const zcg_mouse_pos_t &mouse_pos, const int button)
        {
            if (button == ZCG_MOUSE_BUTTON_LEFT && is_in_area(button_shape.get_container(), mouse_pos))
            {
                if (on_click_callback)
                {
                    on_click_callback(conversation_id);
                }
            }
        }


        void on_mouse_move(const zcg_mouse_pos_t &mouse_pos)
        {
            is_hovered = is_in_area(button_shape.get_container(), mouse_pos);
        }
    };
}
