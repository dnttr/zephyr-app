#pragma once

#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "modal_input_field.hpp"

#include <string>
#include <functional>
#include <regex>

#include "ZCGKit/zcg_interface.h"

namespace zc_app
{
    class connection_modal
    {
        static constexpr float MODAL_WIDTH = 400.0f;
        static constexpr float MODAL_HEIGHT = 320.0f;
        static constexpr float INPUT_HEIGHT = 40.0f;
        static constexpr float BUTTON_WIDTH = 150.0f;
        static constexpr float BUTTON_HEIGHT = 45.0f;
        static constexpr float SDF_BASE_SIZE = 128.0f;
        static constexpr float FONT_ASPECT_RATIO = 0.5f;

        rectangle overlay_background{colour(0, 0, 0, 210), colour(0, 0, 0, 0), 0, 0.0F};
        rectangle modal_panel{colour(30, 30, 30, 230), colour(20, 20, 20), 1, BORDER_RADIUS};
        rectangle connect_button{accent_color, colour(255, 255, 255, 100), 1, 15.0F};

        text modal_title_text;
        text connect_button_text;
        text error_text;

        modal_input_field username_input{};
        modal_input_field ip_address_input{};
        modal_input_field port_input{};

        bool is_visible = false;
        bool is_button_hovered = false;
        bool is_connecting = false;

        colour button_original_color = accent_color;
        colour button_hover_color = colour(255, 255, 255);

        std::function<void(const std::string &, const std::string &, const std::string &)> on_connect_callback;

        void set_error_message(const std::string &message);

        void execute_connect();

    public:
        void initialize(int scene_w, int scene_h);

        void update(float delta_time);

        void draw();

        void on_char_typed(unsigned int char_code);

        void on_mouse_down(const zcg_mouse_pos_t &mouse_pos, int button);

        void on_key_down(const zcg_key_event_t &key_event);

        void on_mouse_move(const zcg_mouse_pos_t &mouse_pos);

        void on_connection_result(bool success, const std::string &message);

        [[nodiscard]] std::string get_username() const;

        [[nodiscard]] bool is_modal_visible() const;

        void set_visible(bool visible);

        void set_on_connect_callback(
            std::function<void(const std::string &, const std::string &, const std::string &)> callback);
    };
}
