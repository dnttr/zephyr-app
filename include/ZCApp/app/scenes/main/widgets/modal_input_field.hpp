#pragma once

#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

#include <string>
#include <functional>
#include <cctype>

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

        void set_active(bool active)
        {
            is_active = active;

            if (is_active)
            {
                background_shape.set_outline_color(accent_color);
            }
            else
            {
                background_shape.set_outline_color(colour(255, 255, 255, 80));
            }
        }

        [[nodiscard]] bool is_active_field() const { return is_active; }

        void handle_char_typed(unsigned int char_code)
        {
            if (!is_active || !std::isprint(char_code)) return;

            current_text_value += static_cast<char>(char_code);
            cursor_pos = current_text_value.length();
            input_text_display.set_text(current_text_value);
        }

        void handle_key_down(const zcg_key_event_t &key_event)
        {
            if (!is_active || !key_event.is_pressed) return;

            if (key_event.key_code == ZCG_KEY_BACKSPACE)
            {
                if (!current_text_value.empty())
                {
                    current_text_value.pop_back();
                    cursor_pos = current_text_value.length();
                    input_text_display.set_text(current_text_value);
                }
            }
        }

        void handle_mouse_down(const zcg_mouse_pos_t &mouse_pos)
        {
            const float scale = perspective_util::get_current_display_config().scale / 2;
            const float mouse_x = round(mouse_pos.x / scale);
            const float mouse_y = round(mouse_pos.y / scale);

            const bool clicked_on_field = mouse_x >= x_pos && mouse_x <= x_pos + width &&
                mouse_y >= y_pos && mouse_y <= y_pos + height;

            set_active(clicked_on_field);
        }

        void draw()
        {
            background_shape.draw();

            if (current_text_value.empty())
            {
                placeholder_text_display.render();
            }

            input_text_display.render();
        }

        [[nodiscard]] std::string get_text() const { return current_text_value; }

        void clear_text()
        {
            current_text_value.clear();
            input_text_display.set_text("");

            cursor_pos = 0;
        }
    };
}