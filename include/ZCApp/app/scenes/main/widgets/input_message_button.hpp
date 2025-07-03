//
// Created by Damian Netter on 03/07/2025.
//

#pragma once

#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCGKit/zcg_interface.h"

#include <string>
#include <iostream>
#include <algorithm>

#include "ZCApp/graphics/fonts/font_manager.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/graphics/objects/text/text_style.hpp"
#include "ZCApp/graphics/utils/scissor.hpp"

namespace zc_app
{
    class input_message_button
    {
        rectangle input_area_glass{glass_tint, glass_border, 1, BORDER_RADIUS};
        rectangle message_input{colour(255, 255, 255, 30), accent_color, 2, 25.0F};

        std::vector<int> characters_per_line{};

        text input_placeholder{};
        text input_text{};

        float max_animation_width = 4.0f;
        float current_animation_width = 0.0f;
        float animation_speed = 600.0f;

        float target_animated_default_text_pos_y = 0.0f;
        float animated_default_text_pos_y = 0.0f;
        float vertical_entry_animation_speed = 300.0f;

        int characters_total = 0;
        int character_total_max = 120;
        int characters_per_line_max = 30;
        int current_line = 0;

        float default_text_pos_x = 0.0f;
        constexpr static float ORIGINAL_FONT_SIZE_OFFSET = 128.0f * 0.1f / 2.0f;

        float line_height_px = ORIGINAL_FONT_SIZE_OFFSET * 2.5f;

        float max_lines_visible = 1.0f;

        float current_text_scroll_y_offset = 0.0f;
        float target_text_scroll_y_offset = 0.0f;
        float text_scroll_animation_speed = 900.0f;

        bool is_hovered = false;
        bool is_typing = false;

        std::string current_text = "";

        float cursor_blink_timer = 0.0f;
        float cursor_blink_speed = 0.5f;
        bool show_cursor = true;

        size_t count_lines(const std::string &str)
        {
            if (str.empty()) return 1;
            size_t lines = 1;
            for (char c : str)
            {
                if (c == '\n')
                {
                    lines++;
                }
            }
            return lines;
        }

        void update_text_scroll_target()
        {
            size_t num_lines_in_text = count_lines(current_text);

            if (num_lines_in_text <= max_lines_visible)
            {
                target_text_scroll_y_offset = 0.0f;
            }
            else
            {
                target_text_scroll_y_offset = (num_lines_in_text - max_lines_visible) * line_height_px;
            }

            if (target_text_scroll_y_offset < 0.0f)
            {
                target_text_scroll_y_offset = 0.0f;
            }
        }

        void send_message()
        {
            if (!current_text.empty())
            {
                std::cout << "Message sent: " << current_text << std::endl;
                current_text.clear();
                update_text_scroll_target();
            }
        }

    public:
        void initialize(const float chat_x, float input_height, float scene_height, float scene_width)
        {
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

            text_style placeholder_style{};
            placeholder_style.text_color = colour(0, 0, 0, 180);
            placeholder_style.text_size_magnification = 0.1F;

            text_style input_style{};
            input_style.text_color = colour(0, 0, 0, 255);
            input_style.text_size_magnification = 0.1F;

            max_lines_visible = message_input.get_container().get_height() / line_height_px;

            if (max_lines_visible < 1.0f)
            {
                max_lines_visible = 1.0f;
            }

            default_text_pos_x = message_input.get_container().get_x() + 20;

            animated_default_text_pos_y = message_input.get_container().get_y() + message_input.get_container().
                get_height() / 2 - ORIGINAL_FONT_SIZE_OFFSET;
            target_animated_default_text_pos_y = animated_default_text_pos_y;

            input_placeholder.initialize(
                "Type a message...",
                container(default_text_pos_x, animated_default_text_pos_y),
                font_manager::get_font("Roboto-Regular"),
                placeholder_style
            );

            input_text.initialize(
                "",
                container(default_text_pos_x, animated_default_text_pos_y),
                font_manager::get_font("Roboto-Regular"),
                input_style
            );

            update_text_scroll_target();
        }

        void draw_glassy()
        {
            input_area_glass.draw();
        }

        void draw()
        {
            message_input.draw();

            const float deltaTime = time_util::get_delta_time();

            if (is_hovered)
            {
                const float increment_value = animation_speed * deltaTime;

                if (current_animation_width < max_animation_width)
                {
                    current_animation_width += increment_value;
                }
                else
                {
                    current_animation_width = max_animation_width;
                }
            }
            else
            {
                current_animation_width = std::max(current_animation_width - animation_speed * deltaTime, 0.0F);
            }

            float y_pos_when_idle = message_input.get_container().get_y() + message_input.get_container().get_height() /
                2 - ORIGINAL_FONT_SIZE_OFFSET;

            float y_pos_when_typing = y_pos_when_idle - 10.0f;

            target_animated_default_text_pos_y = is_typing ? y_pos_when_typing : y_pos_when_idle;

            if (animated_default_text_pos_y != target_animated_default_text_pos_y)
            {
                const float step = vertical_entry_animation_speed * deltaTime;
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
                cursor_blink_timer += deltaTime;

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

            if (current_text_scroll_y_offset != target_text_scroll_y_offset)
            {
                float scroll_delta = text_scroll_animation_speed * deltaTime;
                if (target_text_scroll_y_offset > current_text_scroll_y_offset)
                {
                    current_text_scroll_y_offset += scroll_delta;

                    if (current_text_scroll_y_offset > target_text_scroll_y_offset)
                    {
                        current_text_scroll_y_offset = target_text_scroll_y_offset;
                    }
                }
                else
                {
                    current_text_scroll_y_offset -= scroll_delta;
                    if (current_text_scroll_y_offset < target_text_scroll_y_offset)
                    {
                        current_text_scroll_y_offset = target_text_scroll_y_offset;
                    }
                }
            }


            const float final_text_y = animated_default_text_pos_y - current_text_scroll_y_offset;

            if (current_text.empty() && !is_typing)
            {
                input_placeholder.set_properties_position(default_text_pos_x + current_animation_width,
                                                          final_text_y);
                input_placeholder.render();
            }
            else
            {
                std::string display_text = current_text;
                if (is_typing && show_cursor)
                {
                    display_text += "|";
                }

                input_text.set_text(display_text);
                input_text.set_properties_position(default_text_pos_x + current_animation_width, final_text_y);

                scissor::glScissorOp([this]
                {
                    input_text.render();
                }, message_input.get_container(), {});
            }
        }

        bool is_inside_area(const container &area, const zcg_mouse_pos_t pos)
        {
            const float scale = perspective_util::get_current_display_config().scale / 2;

            const float mouse_x = round(pos.x / scale);
            const float mouse_y = round(pos.y / scale);

            if (mouse_x >= area.get_x() && mouse_x <= area.get_x() + area.get_width())
            {
                if (mouse_y >= area.get_y() && mouse_y <= area.get_y() + area.get_height())
                {
                    return true;
                }
            }
            return false;
        }

        void on_mouse_move(zcg_mouse_pos_t pos)
        {
            if (is_inside_area(message_input.get_container(), pos))
            {
                is_hovered = true;
            }
            else
            {
                is_hovered = false;
            }
        }

        void on_mouse_down(zcg_mouse_pos_t mouse_pos, int button)
        {
            if (is_inside_area(message_input.get_container(), mouse_pos) && button == ZCG_MOUSE_BUTTON_LEFT)
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

        void on_key_down(zcg_key_event_t key_event)
        {
            if (!is_typing || !key_event.is_pressed)
            {
                return;
            }

            const auto length = static_cast<int>(current_text.length());

            if (key_event.key_code == ZCG_KEY_BACKSPACE)
            {
                if (!current_text.empty())
                {
                    current_text.pop_back();

                    if (current_text.empty())
                    {
                        current_line = 0;
                        characters_per_line.assign(1, 0);
                        characters_total = 0;
                    } else if (current_text.back() == '\n')
                    {
                        if (current_line > 0)
                        {
                            current_line--;
                            if (!characters_per_line.empty())
                            {
                                characters_per_line.pop_back();
                            }
                        }
                    }
                    else
                    {
                        if (characters_per_line[current_line] > 0 && characters_total > 0)
                        {
                            characters_total--;
                            characters_per_line[current_line]--;
                        }
                    }
                }
            }
            else if (key_event.key_code == ZCG_KEY_ENTER)
            {
                if (count_lines(current_text) > 3 || length == character_total_max)
                {
                    return; //unsupported
                }

                current_text += '\n'; //debug it
                current_line++;
                characters_per_line.push_back(0);
            }
            else if (key_event.key_code >= 32 && key_event.key_code <= 126)
            {
                if (characters_per_line.size() == 0)
                {
                    characters_per_line.push_back(0);
                    current_line = 0;
                }

                if (characters_total + 1 > character_total_max)
                {
                    return; //duh
                }

                if (current_line < characters_per_line.size() && characters_per_line[current_line] >= characters_per_line_max)
                {
                     if (count_lines(current_text) >= max_lines_visible || characters_total == character_total_max)
                    {
                        return; //unsupported
                    }

                    current_text += '\n'; //no idea if it is classified as two or one lol
                    current_line++;
                    characters_per_line.push_back(0);
                }

                current_text += static_cast<char>(key_event.key_code);

                characters_total++;

                if (current_line < characters_per_line.size())
                {
                    characters_per_line[current_line]++;
                }
            }

            cursor_blink_timer = 0.0f;
            show_cursor = true;
            update_text_scroll_target();
        }

        void on_char_input(char c)
        {
            if (!is_typing) return;

            if (c >= 32 && c <= 126)
            {
                current_text += c;
                cursor_blink_timer = 0.0f;
                show_cursor = true;
                update_text_scroll_target();
            }
        }

        bool get_is_typing() const { return is_typing; }

        const std::string &get_current_text() const { return current_text; }

        bool get_is_hovered() const { return is_hovered; }

        void set_text(const std::string &text)
        {
            current_text = text;
            update_text_scroll_target();
        }

        void clear_text()
        {
            current_text.clear();
            update_text_scroll_target();
        }

        void set_typing(bool typing)
        {
            is_typing = typing;
            if (!is_typing)
            {
                show_cursor = false;
                cursor_blink_timer = 0.0f;
            }
        }
    };
}
