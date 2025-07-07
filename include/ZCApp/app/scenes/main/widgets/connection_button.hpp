//
// Created by Damian Netter on 03/07/2025.
//

#pragma once

#include <functional>
#include <string>

#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/app/scenes/utils.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/graphics/textures/fan_texture.hpp"

namespace zc_app
{
    static colour default_color{glass_tint, 60};
    static colour hovered_colour{glass_tint, 100};

    class connection_button
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

        connection_button(std::string id, std::string name, std::string status, const std::string &avatar_path);

        connection_button();

        void setup(const container &c);

        void render();

        void set_on_click_callback(std::function<void(const std::string &)> callback);

        void on_mouse_down(const zcg_mouse_pos_t &mouse_pos, int button);

        void on_mouse_move(const zcg_mouse_pos_t &mouse_pos);

        container &get_container();
    };
}
