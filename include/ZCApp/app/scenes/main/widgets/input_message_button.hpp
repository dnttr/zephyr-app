//
// Created by Damian Netter on 03/07/2025.
//

#pragma once

#include <string>

#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/fonts/font_manager.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCGKit/zcg_interface.h"

namespace zc_app
{
    class input_message_button
    {
        rectangle input_area_glass{glass_tint, glass_border, 1, BORDER_RADIUS};
        rectangle message_input{colour(255, 255, 255, 30), accent_color, 2, 25.0F};

        text input_placeholder{};
        text input_text{};

        float max_animation_width = 4.0f;
        float current_animation_width = 0.0f;
        float animation_speed = 600.0f;

        float target_animated_default_text_pos_y = 0.0f;
        float animated_default_text_pos_y = 0.0f;
        float vertical_entry_animation_speed = 300.0f;

        int character_total_max = 120;

        float line_height_px = 0.0f;
        float max_lines_visible = 1.0f;

        float current_text_scroll_y_offset = 0.0f;
        float target_text_scroll_y_offset = 0.0f;
        float text_scroll_animation_speed = 900.0f;

        bool is_hovered = false;
        bool is_typing = false;

        std::string current_text;

        float cursor_blink_timer = 0.0f;
        float cursor_blink_speed = 0.06f;
        bool show_cursor = true;

        float default_text_pos_x = 0.0f;
        const float ORIGINAL_FONT_SIZE_OFFSET = 128.0f * 0.1f / 2.0f;

        void update_text_scroll_target();

        void send_message();

    public:

        void initialize(float chat_x, float input_height, float scene_height, float scene_width);

        void draw_glassy();

        void draw();

        void on_mouse_move(zcg_mouse_pos_t pos_physical);

        void on_mouse_down(zcg_mouse_pos_t mouse_pos_physical, int button);

        void on_key_down(zcg_key_event_t key_event);

        void on_char_input(unsigned int char_code);

        void set_text(const std::string &text);

        void clear_text();

        void set_typing(bool typing);

        [[nodiscard]] bool get_is_typing() const { return is_typing; }

        [[nodiscard]] const std::string &get_current_text() const { return current_text; }

        [[nodiscard]] bool get_is_hovered() const { return is_hovered; }};
}
