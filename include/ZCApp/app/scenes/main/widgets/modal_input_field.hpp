#pragma once

#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

#include <string>
#include <functional>
#include <cctype>
#include <cmath>

#include "ZCGKit/zcg_interface.h"

namespace zc_app
{
    class modal_input_field
    {
        rectangle background_shape{colour(255, 255, 255, 30), colour(255, 255, 255, 80), 1, 10.0F};
        text input_text_display;
        text placeholder_text_display;

        std::string current_text_value;
        std::string placeholder_value;
        bool is_active = false;
        size_t cursor_pos = 0;

        float x_pos, y_pos, width, height;

        std::function<bool(unsigned int)> char_validator;

        float total_elapsed_time = 0.0f;
        float focus_animation_progress = 0.0f;
        static constexpr float FOCUS_ANIMATION_SPEED = 4000.0f;

        float cursor_blink_timer = 0.0f;
        bool show_cursor = true;
        static constexpr float CURSOR_BLINK_INTERVAL = 0.01f;
    public:
        void initialize(float x, float y, float w, float h, const std::string &placeholder,
                        const font_manager::font *font, const text_style &style)
        {
            x_pos = x;
            y_pos = y;
            width = w;
            height = h;
            placeholder_value = placeholder;

            background_shape.set_container(container(x_pos, y_pos, width, height));

            text_style input_style = style;
            input_style.text_size_magnification = 0.09F;
            input_style.text_color = colour(255, 255, 255, 255);
            input_style.shadow_enable = false;

            input_text_display.initialize(
                "",
                container(x_pos + PADDING, y_pos + (height - input_style.text_size_magnification * 128.0f) / 2.0f),
                *font,
                input_style
            );

            text_style placeholder_style = style;
            placeholder_style.text_size_magnification = 0.09F;
            placeholder_style.text_color = colour(150, 150, 150, 150);
            placeholder_style.shadow_enable = false;

            placeholder_text_display.initialize(
                placeholder_value,
                container(x_pos + PADDING,
                          y_pos + (height - placeholder_style.text_size_magnification * 128.0f) / 2.0f),
                *font,
                placeholder_style
            );
        }

        void set_char_validator(std::function<bool(unsigned int)> validator)
        {
            char_validator = std::move(validator);
        }

        void set_active(bool active)
        {
            if (is_active == active) return;
            is_active = active;

            if (is_active)
            {
                show_cursor = true;
                cursor_blink_timer = 0.0f;
            }
        }

        [[nodiscard]] bool is_active_field() const { return is_active; }

        void handle_char_typed(unsigned int char_code)
        {
            if (!is_active) return;

            bool is_valid_char = char_validator ? char_validator(char_code) : std::isprint(char_code);
            if (!is_valid_char) return;

            current_text_value.insert(cursor_pos, 1, static_cast<char>(char_code));
            cursor_pos++;

            show_cursor = true;
            cursor_blink_timer = 0.0f;
        }

        void handle_key_down(const zcg_key_event_t &key_event)
        {
            if (!is_active || !key_event.is_pressed) return;

            switch (key_event.key_code)
            {
            case ZCG_KEY_BACKSPACE:
                if (cursor_pos > 0 && !current_text_value.empty())
                {
                    current_text_value.erase(cursor_pos - 1, 1);
                    cursor_pos--;
                }
                break;
            case ZCG_KEY_LEFT:
                if (cursor_pos > 0) cursor_pos--;
                break;
            case ZCG_KEY_RIGHT:
                if (cursor_pos < current_text_value.length()) cursor_pos++;
                break;
            case ZCG_KEY_HOME:
                cursor_pos = 0;
                break;
            case ZCG_KEY_END:
                cursor_pos = current_text_value.length();
                break;
            }

            show_cursor = true;
            cursor_blink_timer = 0.0f;
        }

        void handle_mouse_down(const zcg_mouse_pos_t &mouse_pos)
        {
            const float scale = perspective_util::get_current_display_config().scale / 2;
            const float mouse_x = round(mouse_pos.x / scale);
            const float mouse_y = round(mouse_pos.y / scale);

            const bool clicked_on_field = mouse_x >= x_pos && mouse_x <= x_pos + width &&
                mouse_y >= y_pos && mouse_y <= y_pos + height;

            if (clicked_on_field)
            {
                set_active(true);
            }
            else
            {
                set_active(false);
            }
        }

        void update(float delta_time)
        {
            cursor_blink_timer += delta_time;

            if (cursor_blink_timer >= CURSOR_BLINK_INTERVAL)
            {
                show_cursor = !show_cursor;
                cursor_blink_timer = 0.0f;
                std::cout << "!!! MIF CURSOR TOGGLED !!! New show_cursor: " << (show_cursor ? "True" : "False") <<
                    std::endl;
            }

            total_elapsed_time += delta_time;
            if (focus_animation_progress < 1.0f)
            {
                focus_animation_progress += FOCUS_ANIMATION_SPEED * delta_time;
                if (focus_animation_progress > 1.0f) focus_animation_progress = 1.0f;
            }

            const colour inactive_color(255, 255, 255, 80);
            const colour active_color(0, 150, 255, 200);

            const float t = focus_animation_progress;

            uint8_t r = static_cast<uint8_t>(inactive_color.get_red_u8() + (active_color.get_red_u8() - inactive_color.
                get_red_u8()) * t);
            uint8_t g = static_cast<uint8_t>(inactive_color.get_green_u8() + (active_color.get_green_u8() -
                inactive_color.get_green_u8()) * t);
            uint8_t b = static_cast<uint8_t>(inactive_color.get_blue_u8() + (active_color.get_blue_u8() - inactive_color
                .get_blue_u8()) * t);
            uint8_t a = static_cast<uint8_t>(inactive_color.get_alpha_u8() + (active_color.get_alpha_u8() -
                inactive_color.get_alpha_u8()) * t);

            background_shape.set_outline_color(colour(r, g, b, a));
        }

        void draw()
        {
            background_shape.draw();

            if (current_text_value.empty() && !is_active)
            {
                placeholder_text_display.render();
            }

            std::string display_text = current_text_value;

            if (is_active && show_cursor)
            {
                display_text.insert(cursor_pos, "|");
            }

            input_text_display.set_text(display_text);

            input_text_display.render();
        }

        [[nodiscard]] std::string get_text() const { return current_text_value; }

        void clear_text()
        {
            current_text_value.clear();
            cursor_pos = 0;
        }
    };
}
