//
// Created by Damian Netter on 03/07/2025.
//

#pragma once

#include <functional>
#include <string>

#include "ZCApp/app/scenes/utils.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCGKit/zcg_interface.h"

namespace zc_app
{
    class modal_input_field
    {
        std::function<bool(unsigned int)> char_validator;

        rectangle background_shape{colour(255, 255, 255, 30), colour(255, 255, 255, 80), 1, 10.0F};

        text input_text_display;
        text placeholder_text_display;

        container field_container;

        std::string current_text_value;
        std::string placeholder_value;

        bool is_active = false;
        size_t cursor_pos = 0;

        float total_elapsed_time = 0.0f;
        float focus_animation_progress = 0.0f;

        float cursor_blink_timer = 0.0f;
        bool show_cursor = true;

        static constexpr float FOCUS_ANIMATION_SPEED = 4.0f;
        static constexpr float CURSOR_BLINK_INTERVAL = 0.02f;

    public:
        void initialize(const container &cont, const std::string &placeholder,
                        const font_manager::font *font, const text_style &style);

        void update(float delta_time);

        void draw();

        void handle_char_typed(unsigned int char_code);

        void handle_key_down(const zcg_key_event_t &key_event);

        void handle_mouse_down(const zcg_mouse_pos_t &mouse_pos);

        void clear_text();

        void set_active(bool active);

        void set_char_validator(std::function<bool(unsigned int)> validator);

        [[nodiscard]] bool is_active_field() const { return is_active; }

        [[nodiscard]] std::string get_text() const;
    };
}
