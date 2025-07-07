//
// Created by Damian Netter on 06/07/2025.
//

#pragma once

#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/app/scenes/utils.hpp"
#include "ZCGKit/zcg_interface.h"

#include <string>
#include <functional>

namespace zc_app
{
    class request_modal
    {
        rectangle overlay_background{colour(0, 0, 0, 210), colour(0, 0, 0, 0), 0, 0.0F};
        rectangle modal_panel{colour(30, 30, 30, 230), colour(20, 20, 20), 1, BORDER_RADIUS};
        rectangle accept_button{accent_color, colour(255, 255, 255, 100), 1, 15.0F};
        rectangle decline_button{colour(80, 80, 80, 200), colour(255, 255, 255, 100), 1, 15.0F};

        text title_text;
        text accept_text;
        text decline_text;

        bool is_visible = false;

        std::function<void(bool)> on_answered_callback;

    public:
        void initialize(int scene_width, int scene_height);

        void show(const std::string& sender_name);

        void set_on_answered_callback(std::function<void(bool)> callback);

        void draw();

        void on_mouse_down(const zcg_mouse_pos_t& mouse_pos, int button);

        [[nodiscard]] bool is_modal_visible() const;
    };
}