//
// Created by Damian Netter on 07/07/2025.
//

#include "ZCApp/app/scenes/main/widgets/modal_input_field.hpp"

#include "ZCApp/app/scenes/apperance.hpp"

namespace zc_app
{
    void modal_input_field::initialize(const container &cont, const std::string &placeholder,
        const font_manager::font *font, const text_style &style)
    {
        field_container = cont;
        placeholder_value = placeholder;

        background_shape.set_container(field_container);

        text_style input_style = style;
        input_style.text_size_magnification = 0.09F;
        input_style.text_color = colour(255, 255, 255, 255);
        input_style.shadow_enable = false;

        text_style placeholder_style = style;
        placeholder_style.text_size_magnification = 0.09F;
        placeholder_style.text_color = colour(150, 150, 150, 150);
        placeholder_style.shadow_enable = false;

        const container input_text_container(field_container.get_x() + PADDING,
                                             field_container.get_y() + (field_container.get_height() - input_style.text_size_magnification * 128.0f) / 2.0f);

        input_text_display.initialize(
            "",
            input_text_container,
            *font,
            input_style
        );

        const container placeholder_text_display_container(field_container.get_x() + PADDING, field_container.get_y() + (field_container.get_height() - placeholder_style.text_size_magnification * 128.0f) / 2.0f);
        placeholder_text_display.initialize(
            placeholder_value,
            placeholder_text_display_container,
            *font,
            placeholder_style
        );
    }

    void modal_input_field::set_char_validator(std::function<bool(unsigned int)> validator)
    {
        char_validator = std::move(validator);
    }

    void modal_input_field::set_active(bool active)
    {
        if (is_active == active) return;
        is_active = active;
        focus_animation_progress = 0.0f;

        if (is_active)
        {
            show_cursor = true;
            cursor_blink_timer = 0.0f;
        }
    }

    void modal_input_field::handle_char_typed(unsigned int char_code)
    {
        if (!is_active)
        {
            return;
        }

        const auto code = static_cast<char>(char_code);

        if (const bool is_valid_char = char_validator ? char_validator(char_code) : std::isprint(code); !is_valid_char)
        {
            return;
        }

        if (current_text_value.length() >= 30)
        {
            return;
        }

        current_text_value.insert(cursor_pos, 1, static_cast<char>(char_code));
        cursor_pos++;

        show_cursor = true;
        cursor_blink_timer = 0.0f;
    }

    void modal_input_field::handle_key_down(const zcg_key_event_t &key_event)
    {
        if (!is_active || !key_event.is_pressed)
        {
            return;
        }

        switch (key_event.key_code)
        {
        case ZCG_KEY_BACKSPACE:
            if (cursor_pos > 0 && !current_text_value.empty())
            {
                current_text_value.erase(cursor_pos - 1, 1);
                cursor_pos--;
            }

            break;
        case ZCG_KEY_DELETE:
            if (cursor_pos < current_text_value.length())
            {
                current_text_value.erase(cursor_pos, 1);
            }

            break;
        case ZCG_KEY_LEFT:
            if (cursor_pos > 0)
            {
                cursor_pos--;
            }

            break;
        case ZCG_KEY_RIGHT:
            if (cursor_pos < current_text_value.length())
            {
                cursor_pos++;
            }

            break;
        case ZCG_KEY_HOME:
            cursor_pos = 0;

            break;
        case ZCG_KEY_END:
            cursor_pos = current_text_value.length();

            break;
        default:
        }

        show_cursor = true;
        cursor_blink_timer = 0.0f;
    }

    void modal_input_field::handle_mouse_down(const zcg_mouse_pos_t &mouse_pos)
    {
        set_active(is_in_area(field_container, mouse_pos));
    }

    void modal_input_field::update(float delta_time)
    {
        if(is_active)
        {
            cursor_blink_timer += delta_time;
            if (cursor_blink_timer >= CURSOR_BLINK_INTERVAL)
            {
                show_cursor = !show_cursor;
                cursor_blink_timer = 0.0f;
            }
        }

        total_elapsed_time += delta_time;

        if (focus_animation_progress < 1.0f)
        {
            focus_animation_progress += FOCUS_ANIMATION_SPEED * delta_time;

            if (focus_animation_progress > 1.0f) {
                focus_animation_progress = 1.0f;
            }
        }

        constexpr colour inactive_color(255, 255, 255, 80);
        constexpr colour active_color(0, 150, 255, 200);

        const float t = is_active ? focus_animation_progress : (1.0f - focus_animation_progress);

        const auto r_inactive = static_cast<float>(inactive_color.get_red_u8());
        const auto g_inactive = static_cast<float>(inactive_color.get_green_u8());
        const auto b_inactive = static_cast<float>(inactive_color.get_blue_u8());
        const auto a_inactive = static_cast<float>(inactive_color.get_alpha_u8());

        const auto r_active = static_cast<float>(active_color.get_red_u8());
        const auto g_active = static_cast<float>(active_color.get_green_u8());
        const auto b_active = static_cast<float>(active_color.get_blue_u8());
        const auto a_active = static_cast<float>(active_color.get_alpha_u8());

        const auto r = static_cast<uint8_t>(r_inactive * (1.0f - t) + r_active * t);
        const auto g = static_cast<uint8_t>(g_inactive * (1.0f - t) + g_active * t);
        const auto b = static_cast<uint8_t>(b_inactive * (1.0f - t) + b_active * t);
        const auto a = static_cast<uint8_t>(a_inactive * (1.0f - t) + a_active * t);

        background_shape.set_outline_color(colour(r, g, b, a));
    }

    void modal_input_field::draw()
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

    std::string modal_input_field::get_text() const
    { return current_text_value; }

    void modal_input_field::clear_text()
    {
        current_text_value.clear();
        cursor_pos = 0;
    }
}
