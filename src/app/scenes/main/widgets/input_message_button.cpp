//
// Created by Damian Netter on 07/07/2025.
//

#include "ZCApp/app/scenes/main/widgets/input_message_button.hpp"

#include "ZCApp/app/scenes/utils.hpp"
#include "ZCApp/graphics/utils/scissor.hpp"
#include "ZCApp/graphics/utils/time_util.hpp"

namespace zc_app
{
    void input_message_button::update_text_scroll_target()
    {
        const auto num_display_lines = static_cast<float>(count_display_lines(
            get_formatted_display_text(current_text, false)));

        if (num_display_lines <= max_lines_visible)
        {
            target_text_scroll_y_offset = 0.0f;
        }
        else
        {
            target_text_scroll_y_offset = (num_display_lines - max_lines_visible) * line_height_px;
        }

        if (target_text_scroll_y_offset < 0.0f)
        {
            target_text_scroll_y_offset = 0.0f;
        }
    }

    void input_message_button::send_message()
    {
        if (!current_text.empty())
        {
            current_text.clear();

            update_text_scroll_target();

            cursor_blink_timer = 0.0f;
            show_cursor = true;
            is_typing = true;
        }
    }

    void input_message_button::initialize(const float chat_x, const float input_height, const float scene_height,
                                          const float scene_width)
    {
        const auto font = font_manager::get_font("Roboto-Regular");

        text_style placeholder_style{};
        placeholder_style.text_color = colour(0, 0, 0, 180);
        placeholder_style.text_size_magnification = 0.1F;

        text_style input_style{};
        input_style.text_color = colour(0, 0, 0, 255);
        input_style.text_size_magnification = 0.1F;

        input_area_glass.set_container(container(
            chat_x, scene_height - input_height - PADDING,
            scene_width - chat_x - PADDING,
            input_height
        ));

        message_input.set_container(container(
            chat_x + PADDING * 2, scene_height - input_height - PADDING + input_height / 2 - 25,
            scene_width - chat_x - PADDING * 6 - 120,
            50.0f
        ));

        line_height_px = ORIGINAL_FONT_SIZE_OFFSET * 2.5f;

        max_lines_visible = message_input.get_container().get_height() / line_height_px;

        if (max_lines_visible < 1.0f)
        {
            max_lines_visible = 1.0f;
        }

        default_text_pos_x = message_input.get_container().get_x() + 20;

        animated_default_text_pos_y = message_input.get_container().get_y() + message_input.get_container().get_height()
            / 2 - ORIGINAL_FONT_SIZE_OFFSET;
        target_animated_default_text_pos_y = animated_default_text_pos_y;

        input_placeholder.initialize(
            "Type a message...",
            container(default_text_pos_x, animated_default_text_pos_y),
            font,
            placeholder_style
        );

        input_text.initialize(
            "",
            container(default_text_pos_x, animated_default_text_pos_y),
            font,
            input_style
        );

        update_text_scroll_target();
    }

    void input_message_button::draw_glassy()
    {
        input_area_glass.draw();
    }

    void input_message_button::draw()
    {
        message_input.draw();

        const float delta_time = time_util::get_delta_time();

        if (is_hovered)
        {
            current_animation_width = std::min(current_animation_width + animation_speed * delta_time,
                                               max_animation_width);
        }
        else
        {
            current_animation_width = std::max(current_animation_width - animation_speed * delta_time, 0.0F);
        }

        const float y_pos_when_idle = message_input.get_container().get_y() + message_input.get_container().get_height()
            / 2 - ORIGINAL_FONT_SIZE_OFFSET;
        const float y_pos_when_typing = y_pos_when_idle - 10.0f;

        target_animated_default_text_pos_y = is_typing ? y_pos_when_typing : y_pos_when_idle;

        if (animated_default_text_pos_y != target_animated_default_text_pos_y)
        {
            const float step = vertical_entry_animation_speed * delta_time;

            if (target_animated_default_text_pos_y < animated_default_text_pos_y)
            {
                animated_default_text_pos_y = std::max(animated_default_text_pos_y - step,
                                                       target_animated_default_text_pos_y);
            }
            else
            {
                animated_default_text_pos_y = std::min(animated_default_text_pos_y + step,
                                                       target_animated_default_text_pos_y);
            }
        }

        if (is_typing)
        {
            cursor_blink_timer += delta_time;

            if (cursor_blink_timer >= cursor_blink_speed)
            {
                show_cursor = !show_cursor;
                cursor_blink_timer = 0.0f;
            }
        }
        else
        {
            show_cursor = false;
            cursor_blink_timer = 0.0f;
        }

        const float scroll_delta = text_scroll_animation_speed * delta_time;

        if (current_text_scroll_y_offset != target_text_scroll_y_offset)
        {
            if (target_text_scroll_y_offset > current_text_scroll_y_offset)
            {
                current_text_scroll_y_offset = std::min(current_text_scroll_y_offset + scroll_delta,
                                                        target_text_scroll_y_offset);
            }
            else
            {
                current_text_scroll_y_offset = std::max(current_text_scroll_y_offset - scroll_delta,
                                                        target_text_scroll_y_offset);
            }
        }

        const float final_text_y = animated_default_text_pos_y - current_text_scroll_y_offset;

        if (current_text.empty() && !is_typing)
        {
            input_placeholder.set_properties_position(default_text_pos_x + current_animation_width, final_text_y);
            input_placeholder.render();
        }
        else
        {
            const std::string display_text = get_formatted_display_text(current_text, is_typing && show_cursor);

            input_text.set_text(display_text);
            input_text.set_properties_position(default_text_pos_x + current_animation_width, final_text_y);

            scissor::glScissorOp([this]
            {
                input_text.render();
            }, message_input.get_container(), {});
        }
    }

    void input_message_button::on_mouse_move(zcg_mouse_pos_t pos_physical)
    {
        if (is_in_area(message_input.get_container(), pos_physical))
        {
            is_hovered = true;
        }
        else
        {
            is_hovered = false;
        }
    }

    void input_message_button::on_mouse_down(zcg_mouse_pos_t mouse_pos_physical, int button)
    {
        if (is_in_area(message_input.get_container(), mouse_pos_physical) && button == ZCG_MOUSE_BUTTON_LEFT)
        {
            is_typing = true;
            cursor_blink_timer = 0.0f;
            show_cursor = true;
        }
        else
        {
            is_typing = false;
        }
    }

    void input_message_button::on_key_down(zcg_key_event_t key_event)
    {
        if (!is_typing || !key_event.is_pressed)
        {
            return;
        }

        if (key_event.key_code == ZCG_KEY_BACKSPACE)
        {
            if (!current_text.empty())
            {
                current_text.pop_back();
            }
        }
        else if (key_event.key_code == ZCG_KEY_ENTER)
        {
            const auto val = static_cast<float>(count_display_lines(current_text));

            if (val < max_lines_visible || current_text.length() <
                character_total_max)
            {
                current_text += '\n';
            }
        }

        cursor_blink_timer = 0.0f;
        show_cursor = true;

        update_text_scroll_target();
    }

    void input_message_button::on_char_input(const unsigned int char_code)
    {
        const auto code = static_cast<int>(char_code);

        if (!is_typing || !std::isprint(code))
        {
            return;
        }

        if (current_text.length() < character_total_max)
        {
            current_text += static_cast<char>(char_code);
        }
        else
        {
            return;
        }

        cursor_blink_timer = 0.0f;
        show_cursor = true;

        update_text_scroll_target();
    }

    void input_message_button::set_text(const std::string &text)
    {
        current_text = text;
        update_text_scroll_target();
    }

    void input_message_button::clear_text()
    {
        current_text.clear();

        update_text_scroll_target();

        is_typing = false;
    }

    void input_message_button::set_typing(const bool typing)
    {
        is_typing = typing;

        if (!is_typing)
        {
            show_cursor = false;
            cursor_blink_timer = 0.0f;
        }
    }
}
