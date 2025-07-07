//
// Created by Damian Netter on 06/07/2025.
//

#pragma once

#include <algorithm>
#include <regex>

#include "ZCApp/graphics/objects/shapes/container.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"
#include "ZCGKit/zcg_interface.h"

namespace zc_app
{
    constexpr float MESSAGE_TEXT_LEFT_RIGHT_PADDING = 15.0f;
    constexpr float AVG_CHAR_WIDTH_FACTOR_MESSAGE = 0.6f;
    constexpr float FONT_SIZE_MAGNIFICATION_MESSAGE = 0.06F;
    constexpr float ESTIMATED_LINE_HEIGHT = 128.0f * FONT_SIZE_MAGNIFICATION_MESSAGE * 1.5f;

    constexpr int MAX_CHARS_PER_VISUAL_LINE = 30;

    [[nodiscard]] static bool is_in_area(const container &container, const float x, const float y)
    {
        const float scale = perspective_util::get_current_display_config().scale / 2;

        const float mouse_x = x / scale;
        const float mouse_y = y / scale;

        if (mouse_x < container.get_x() ||
            mouse_x > container.get_x() + container.get_width() ||
            mouse_y < container.get_y() ||
            mouse_y > container.get_y() + container.get_height())
        {
            return false;
        }

        return true;
    }

    static bool is_in_area(const container &container, const zcg_mouse_pos_t &pos)
    {
        return is_in_area(container, pos.x, pos.y);
    }

    static bool is_in_area(const container &container, const zcg_scroll_event_t &scroll_event)
    {
        return is_in_area(container, scroll_event.mouse_x, scroll_event.mouse_y);
    }

    [[nodiscard]] static float get_estimated_text_width_px(const std::string &line_text)
    {
        if (line_text.empty())
        {
            return 0.0f;
        }

        return static_cast<float>(line_text.length()) * (128.0f * FONT_SIZE_MAGNIFICATION_MESSAGE *
            AVG_CHAR_WIDTH_FACTOR_MESSAGE);
    }

    [[nodiscard]] static size_t count_lines_in_wrapped_text(const std::string &str)
    {
        if (str.empty())
        {
            return 1;
        }

        size_t lines = 1;

        for (const char c : str)
        {
            if (c == '\n')
            {
                lines++;
            }
        }

        return lines;
    }

    [[nodiscard]] static std::string wrap_text_by_chars(const std::string &text, float max_allowed_pixel_width)
    {
        if (text.empty())
        {
            return "";
        }

        std::string wrapped_result;
        std::string current_line_content;

        constexpr float estimated_char_width_px = 128.0f * FONT_SIZE_MAGNIFICATION_MESSAGE *
            AVG_CHAR_WIDTH_FACTOR_MESSAGE;
        const int max_chars_per_line_estimate = static_cast<int>((max_allowed_pixel_width -
            MESSAGE_TEXT_LEFT_RIGHT_PADDING * 2) / estimated_char_width_px);
        const int effective_max_chars = std::max(1, max_chars_per_line_estimate);

        size_t i = 0;

        while (i < text.length())
        {
            size_t word_start = i;
            size_t word_end = word_start;

            while (word_end < text.length() && text[word_end] != ' ' && text[word_end] != '\n')
            {
                word_end++;
            }

            std::string word = text.substr(word_start, word_end - word_start);

            bool is_explicit_newline = word_end < text.length() && text[word_end] == '\n';

            if (word.length() > static_cast<size_t>(effective_max_chars))
            {
                if (!current_line_content.empty())
                {
                    wrapped_result += current_line_content + '\n';

                    current_line_content.clear();
                }

                for (char c : word)
                {
                    current_line_content += c;

                    if (current_line_content.length() >= static_cast<size_t>(effective_max_chars))
                    {
                        wrapped_result += current_line_content + '\n';

                        current_line_content.clear();
                    }
                }
            }
            else
            {
                if (!current_line_content.empty() && current_line_content.length() + 1 + word.length() >
                    static_cast<size_t>(effective_max_chars) && !is_explicit_newline)
                {
                    wrapped_result += current_line_content + '\n';

                    current_line_content.clear();
                }
                if (!current_line_content.empty())
                {
                    current_line_content += ' ';
                }

                current_line_content += word;
            }

            i = word_end;

            if (i < text.length())
            {
                if (text[i] == '\n')
                {
                    wrapped_result += current_line_content + '\n';

                    current_line_content.clear();
                }
                i++;
            }
        }
        if (!current_line_content.empty())
        {
            wrapped_result += current_line_content;
        }

        return wrapped_result;
    }

    inline bool is_valid_username(const std::string &name)
    {
        if (name.length() > 20) return false;
        return std::ranges::all_of(name, [](char c)
        {
            return std::isalnum(c) || c == '_' || c == '-';
        });
    }

    inline bool is_valid_ip_address(const std::string &ip)
    {
        const std::regex ip_regex(R"(^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)");
        return std::regex_match(ip, ip_regex);
    }

    inline bool is_valid_port(const std::string &port_str)
    {
        if (!std::ranges::all_of(port_str, ::isdigit) || port_str.empty())
        {
            return false;
        }
        try
        {
            int port = std::stoi(port_str);
            return port > 0 && port <= 65535;
        }
        catch (const std::out_of_range &)
        {
            return false;
        }
    }

    static size_t count_display_lines(const std::string &str)
    {
        if (str.empty())
        {
            return 1;
        }

        size_t lines = 1;

        for (const char c : str)
        {
            if (c == '\n')
            {
                lines++;
            }
        }
        return lines;
    }

    /**
     * TODO: Use append instead of concatenation for better performance.
     */
    static std::string get_formatted_display_text(const std::string &current_text, const bool include_cursor)
    {
        std::string formatted_text;
        size_t chars_on_current_line = 0;

        for (size_t i = 0; i < current_text.length(); ++i)
        {
            if (const char c = current_text[i]; c == '\n')
            {
                formatted_text += '\n';
                chars_on_current_line = 0;
            }
            else
            {
                if (chars_on_current_line >= MAX_CHARS_PER_VISUAL_LINE)
                {
                    formatted_text += '\n';
                    chars_on_current_line = 0;
                }
                formatted_text += c;
                chars_on_current_line++;
            }
        }

        if (include_cursor)
        {
            formatted_text += "|";
        }

        return formatted_text;
    }
}
