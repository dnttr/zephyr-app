#pragma once

#include "ZCApp/app/scenes/main/widgets/input_message_button.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/graphics/utils/scissor.hpp"
#include "ZCApp/graphics/utils/time_util.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"
#include "ZCKit/bridge.hpp"

#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <cmath>

#include "ZCApp/app/data_manager.hpp"

namespace zc_app
{
    enum class message_animation_state
    {
        IDLE,
        ANIMATING_IN
    };

    struct animated_message
    {
        std::unique_ptr<rectangle> bubble;
        std::unique_ptr<text> message_text;
        std::unique_ptr<text> message_timestamp;

        bool is_sent{};

        message_animation_state animation_state = message_animation_state::IDLE;

        float animation_timer = 0.0f;
        float animation_duration{};

        container original_bubble_container;
        colour original_bubble_fill_color;

        float original_text_x{}, original_text_y{};

        colour original_text_color;

        float original_timestamp_x{}, original_timestamp_y{};

        colour original_timestamp_color;
    };

    class chat_area
    {
        rectangle chat_area_glass{glass_tint, glass_border, 1, BORDER_RADIUS};
        rectangle chat_header{colour(glass_tint, 60), glass_border, 1, BORDER_RADIUS};
        rectangle send_button{accent_color, colour(255, 255, 255, 100), 1, 20.0F};
        rectangle attach_button{glass_tint, glass_border, 1, 20.0F};

        rectangle scrollbar_track{colour(255, 255, 255, 30), colour(255, 255, 255, 60), 1, 8.0F};
        rectangle scrollbar_thumb{colour(255, 255, 255, 120), colour(255, 255, 255, 180), 1, 8.0F};

        fan_texture chat_avatar{"avatar.png", 2.0F, 32};

        text send_icon, attach_icon;
        text chat_title, last_seen_text;

        std::vector<animated_message> animated_messages;

        input_message_button input_button;

        data::data_manager *p_data_manager = nullptr;

        std::string current_conversation_id;
        conversation_data current_conversation_cache;

        float messages_scroll_y = 0.0f;
        float target_messages_scroll_y = 0.0f;
        float scroll_animation_speed = 8000.0f;
        float total_messages_height = 0.0f;
        float visible_messages_height = 0.0f;
        float messages_content_start_y = 0.0f;

        bool scrollbar_dragging = false;
        float scrollbar_drag_offset = 0.0f;
        bool scrollbar_hovered = false;

        int scene_width = 0;
        int scene_height = 0;

        const float TOP_BAR_HEIGHT = 70.0f;
        const float INPUT_AREA_HEIGHT = 80.0f;
        const float CHAT_HEADER_VERTICAL_PADDING = PADDING;
        const float CHAT_HEADER_TO_MESSAGES_PADDING = PADDING;
        const float MESSAGE_BUBBLE_BASE_HEIGHT = 45.0f;
        const float BUTTON_RIGHT_PADDING = 10.0f;
        const float MESSAGE_SPACING_VERTICAL = 60.0f;

        const float MESSAGE_TEXT_LEFT_RIGHT_PADDING = 15.0f;
        const float MESSAGE_TEXT_TOP_BOTTOM_PADDING = 12.0f;
        const float TIMESTAMP_HEIGHT_IN_BUBBLE = 15.0f;
        const float FONT_SIZE_MAGNIFICATION_MESSAGE = 0.06F;

        const float ESTIMATED_LINE_HEIGHT = (128.0f * FONT_SIZE_MAGNIFICATION_MESSAGE * 1.5f);
        const float AVG_CHAR_WIDTH_FACTOR_MESSAGE = 0.6f;

        const float MIN_BUBBLE_WIDTH_SENT = 100.0f;

        const float MESSAGE_ANIMATION_DURATION = 0.02f;

        [[nodiscard]] float get_estimated_text_width_px(const std::string &line_text) const
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

        [[nodiscard]] std::string wrap_text_by_chars(const std::string &text, float max_allowed_pixel_width) const
        {
            if (text.empty()) return "";

            std::string wrapped_result;
            std::string current_line_content;

            const float estimated_char_width_px = 128.0f * FONT_SIZE_MAGNIFICATION_MESSAGE *
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

                bool is_explicit_newline = (word_end < text.length() && text[word_end] == '\n');

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
                    if (!current_line_content.empty() && (current_line_content.length() + 1 + word.length() >
                        static_cast<size_t>(effective_max_chars)) && !is_explicit_newline)
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

        void setup_chat_text_styles()
        {
            text_style default_style;

            default_style.text_size_magnification = 0.07F;
            default_style.text_color = colour(255, 255, 255, 240);
            default_style.outline_enable = false;
            default_style.shadow_enable = true;
            default_style.shadow_offset = {2.0F, 2.0F};
            default_style.shadow_color = colour(0, 0, 0, 100);

            text_style status_style = default_style;

            status_style.text_size_magnification = 0.05F;
            status_style.text_color = colour(180, 180, 180, 200);

            text_style chat_header_style = default_style;
            chat_header_style.text_size_magnification = 0.08F;

            const std::string chat_title_str = "No conversation selected";

            const float chat_title_width = 128.0f * default_style.text_size_magnification * chat_title_str.length() / 2.0f;

            const float chat_title_x = chat_header.get_container().get_x() + chat_title_width - PADDING * 3.5;
            const float chat_title_y = chat_header.get_container().get_y() + chat_header.get_container().get_height() / 2.0f - PADDING;

            chat_title.initialize(
                chat_title_str,
                container(chat_title_x, chat_title_y),
                font_manager::get_font("Roboto-Medium"),
                chat_header_style
            );

            last_seen_text.initialize(
                "",
                container(chat_header.get_container().get_x() + 60, chat_header.get_container().get_y() + 35),
                font_manager::get_font("Roboto-Regular"),
                status_style
            );

            text_style icon_style = default_style;
            icon_style.text_size_magnification = 0.09F;

            send_icon.initialize(
                "",
                container(send_button.get_container().get_x() + 18, send_button.get_container().get_y() + 12),
                font_manager::get_font("Roboto-Regular"),
                icon_style
            );

            attach_icon.initialize(
                "",
                container(attach_button.get_container().get_x() + 18, attach_button.get_container().get_y() + 12),
                font_manager::get_font("Roboto-Regular"),
                icon_style
            );
        }

        void update_scrollbar()
        {
            if (total_messages_height <= visible_messages_height)
            {
                scrollbar_track.set_container(container(0, 0, 0, 0));
                scrollbar_thumb.set_container(container(0, 0, 0, 0));
                return;
            }

            constexpr float scrollbar_width = 12.0f;

            const float scrollbar_x = chat_area_glass.get_container().get_x() + chat_area_glass.get_container().get_width() - PADDING;
            const float scrollbar_y = messages_content_start_y;
            const float scrollbar_height = visible_messages_height;

            scrollbar_track.set_container(container(scrollbar_x, scrollbar_y, scrollbar_width, scrollbar_height));

            const float thumb_ratio = visible_messages_height / total_messages_height;
            const float thumb_height = std::max(30.0f, scrollbar_height * thumb_ratio);

            const float scroll_ratio = (total_messages_height > visible_messages_height)
                                           ? (messages_scroll_y / (total_messages_height - visible_messages_height))
                                           : 0.0f;

            const float thumb_y = scrollbar_y + (scrollbar_height - thumb_height) * scroll_ratio;
            scrollbar_thumb.set_container(container(scrollbar_x, thumb_y, scrollbar_width, thumb_height));
        }

        void rebuild_message_display(bool animate_new_messages = false)
        {
            std::vector<animated_message> new_animated_messages;

            if (current_conversation_id.empty() || current_conversation_cache.contact_name.empty())
            {
                total_messages_height = 0.0f;
                animated_messages.clear();

                update_scrollbar();

                return;
            }

            float chat_content_width = chat_area_glass.get_container().get_width() - PADDING * 2 - 20;
            const auto &conversation = current_conversation_cache;

            float current_relative_y = PADDING;

            size_t existing_msg_count = animated_messages.size();
            size_t conversation_msg_count = conversation.messages.size();

            for (size_t i = 0; i < conversation_msg_count; ++i)
            {
                const auto &msg = conversation.messages[i];
                const bool is_sent = msg.is_sent;

                const float bubble_max_container_width = std::min(400.0f, chat_content_width * 0.7f);

                const auto wrapped_content = wrap_text_by_chars(msg.content, bubble_max_container_width);
                const auto num_lines = count_lines_in_wrapped_text(wrapped_content);

                float calculated_bubble_width = bubble_max_container_width;

                if (is_sent)
                {
                    float estimated_wrapped_text_width_px = 0.0f;

                    size_t start = 0;
                    size_t end = wrapped_content.find('\n');

                    while (true)
                    {
                        std::string line = wrapped_content.substr(start, end - start);
                        estimated_wrapped_text_width_px = std::max(estimated_wrapped_text_width_px,
                                                                   get_estimated_text_width_px(line));

                        if (end == std::string::npos)
                        {
                            break;
                        }

                        start = end + 1;
                        end = wrapped_content.find('\n', start);
                    }

                    float desired_content_width = estimated_wrapped_text_width_px + MESSAGE_TEXT_LEFT_RIGHT_PADDING * 2 + TIMESTAMP_HEIGHT_IN_BUBBLE + 20;

                    calculated_bubble_width = std::min(bubble_max_container_width, std::max(MIN_BUBBLE_WIDTH_SENT, desired_content_width));
                }

                const float bubble_text_height = static_cast<float>(num_lines) * ESTIMATED_LINE_HEIGHT;

                float bubble_height = MESSAGE_TEXT_TOP_BOTTOM_PADDING * 2 + bubble_text_height + TIMESTAMP_HEIGHT_IN_BUBBLE;
                bubble_height = std::max(bubble_height, MESSAGE_BUBBLE_BASE_HEIGHT);

                float bubble_x;

                if (is_sent)
                {
                    bubble_x = chat_area_glass.get_container().get_x() + chat_area_glass.get_container().get_width() - PADDING - calculated_bubble_width - 10;
                }
                else
                {
                    bubble_x = chat_area_glass.get_container().get_x() + PADDING + 10;
                }

                animated_message am;

                am.is_sent = is_sent;
                am.animation_duration = MESSAGE_ANIMATION_DURATION;

                colour bubble_color = is_sent ? accent_color : colour(255, 255, 255, 80);
                colour bubble_border_color = is_sent ? colour(255, 255, 255, 100) : colour(255, 255, 255, 60);

                am.bubble = std::make_unique<rectangle>(
                    bubble_color,
                    bubble_border_color,
                    1, 18.0f
                );

                am.bubble->set_container(container(bubble_x, messages_content_start_y + current_relative_y,
                                                   calculated_bubble_width, bubble_height));
                am.original_bubble_container = am.bubble->get_container();
                am.original_bubble_fill_color = bubble_color;

                text_style message_style;
                message_style.text_size_magnification = FONT_SIZE_MAGNIFICATION_MESSAGE;
                message_style.text_color = colour(0, 0, 0, 255);
                message_style.shadow_enable = true;
                message_style.shadow_offset = {1.0F, 1.0F};
                message_style.shadow_color = colour(0, 0, 0, 0);

                am.message_text = std::make_unique<text>();
                am.message_text->initialize(
                    wrapped_content,
                    container(bubble_x + MESSAGE_TEXT_LEFT_RIGHT_PADDING,
                              messages_content_start_y + current_relative_y + MESSAGE_TEXT_TOP_BOTTOM_PADDING),
                    font_manager::get_font("Roboto-Regular"),
                    message_style
                );

                am.original_text_x = bubble_x + MESSAGE_TEXT_LEFT_RIGHT_PADDING;
                am.original_text_y = messages_content_start_y + current_relative_y + MESSAGE_TEXT_TOP_BOTTOM_PADDING;
                am.original_text_color = message_style.text_color;

                text_style timestamp_style;
                timestamp_style.text_size_magnification = 0.04F;
                timestamp_style.text_color = colour(120, 120, 120, 200);
                timestamp_style.shadow_enable = false;

                am.message_timestamp = std::make_unique<text>();

                const auto time_str = format_timestamp(msg.timestamp);
                float timestamp_x = is_sent
                                        ? bubble_x + calculated_bubble_width - MESSAGE_TEXT_LEFT_RIGHT_PADDING -
                                        PADDING
                                        : bubble_x + MESSAGE_TEXT_LEFT_RIGHT_PADDING;

                am.message_timestamp->initialize(
                    time_str,
                    container(timestamp_x,
                              messages_content_start_y + current_relative_y + bubble_height -
                              TIMESTAMP_HEIGHT_IN_BUBBLE),
                    font_manager::get_font("Roboto-Regular"),
                    timestamp_style
                );

                am.original_timestamp_x = timestamp_x;
                am.original_timestamp_y = messages_content_start_y + current_relative_y + bubble_height -
                    TIMESTAMP_HEIGHT_IN_BUBBLE;
                am.original_timestamp_color = timestamp_style.text_color;

                if (animate_new_messages && i >= existing_msg_count)
                {
                    am.animation_state = message_animation_state::ANIMATING_IN;
                    am.animation_timer = 0.0f;
                }

                new_animated_messages.push_back(std::move(am));

                current_relative_y += bubble_height + PADDING;
            }

            animated_messages = std::move(new_animated_messages);
            total_messages_height = current_relative_y;

            if (total_messages_height <= visible_messages_height)
            {
                target_messages_scroll_y = 0.0f;
                messages_scroll_y = 0.0f;
            }

            update_scrollbar();
        }

        static std::string format_timestamp(const std::chrono::system_clock::time_point &timestamp)
        {
            const auto time_t = std::chrono::system_clock::to_time_t(timestamp);
            const auto tm = *std::localtime(&time_t);

            char buffer[16];
            std::strftime(buffer, sizeof(buffer), "%H:%M", &tm);

            return {buffer};
        }

        void animate_scroll()
        {
            if (std::abs(messages_scroll_y - target_messages_scroll_y) > 0.5f)
            {
                const float delta = time_util::get_delta_time();
                const float step = scroll_animation_speed * delta;

                if (target_messages_scroll_y > messages_scroll_y)
                {
                    messages_scroll_y = std::min(messages_scroll_y + step, target_messages_scroll_y);
                }
                else
                {
                    messages_scroll_y = std::max(messages_scroll_y - step, target_messages_scroll_y);
                }

                update_scrollbar();
            }
        }

        bool is_inside_scrollbar(const zcg_mouse_pos_t &pos)
        {
            const float scale = perspective_util::get_current_display_config().scale / 2;
            const float mouse_x = round(pos.x / scale);
            const float mouse_y = round(pos.y / scale);

            const auto &thumb_container = scrollbar_thumb.get_container();

            return mouse_x >= thumb_container.get_x() &&
                mouse_x <= thumb_container.get_x() + thumb_container.get_width() &&
                mouse_y >= thumb_container.get_y() &&
                mouse_y <= thumb_container.get_y() + thumb_container.get_height();
        }

        void handle_scrollbar_drag(const zcg_mouse_pos_t &pos)
        {
            if (!scrollbar_dragging)
            {
                return;
            }

            const float scale = perspective_util::get_current_display_config().scale / 2;
            const float mouse_y = round(pos.y / scale);

            const auto &track_container = scrollbar_track.get_container();
            const float relative_y = mouse_y - track_container.get_y() - scrollbar_drag_offset;

            const float scroll_area_height = track_container.get_height() - scrollbar_thumb.get_container().
                get_height();

            float scroll_ratio = (scroll_area_height > 0) ? (relative_y / scroll_area_height) : 0.0f;
            scroll_ratio = std::max(0.0f, std::min(1.0f, scroll_ratio));

            target_messages_scroll_y = scroll_ratio * (total_messages_height - visible_messages_height);
            messages_scroll_y = target_messages_scroll_y;

            update_scrollbar();
        }

    public:
        void initialize(const float chat_x_start_pos, data::data_manager *data_mgr)
        {
            scene_width = 960;
            scene_height = 540;

            p_data_manager = data_mgr;

            chat_area_glass.set_container(container(
                chat_x_start_pos, PADDING,
                static_cast<float>(scene_width) - chat_x_start_pos - PADDING,
                static_cast<float>(scene_height) - INPUT_AREA_HEIGHT - PADDING * 3
            ));

            chat_header.set_container(container(
                chat_area_glass.get_container().get_x() + CHAT_HEADER_VERTICAL_PADDING,
                chat_area_glass.get_container().get_y() + CHAT_HEADER_VERTICAL_PADDING,
                chat_area_glass.get_container().get_width() - CHAT_HEADER_VERTICAL_PADDING * 2,
                TOP_BAR_HEIGHT
            ));

            constexpr float avatar_width = 40.0F;
            constexpr float avatar_height = 40.0F;

            chat_avatar.set_container(container(
                chat_header.get_container().get_x() + (avatar_width + PADDING) / 2,
                chat_header.get_container().get_y() + (chat_header.get_container().get_height() - PADDING) / 2,
                avatar_width,
                avatar_height
            ));

            input_button.initialize(
                chat_x_start_pos,
                INPUT_AREA_HEIGHT,
                static_cast<float>(scene_height),
                static_cast<float>(scene_width)
            );

            send_button.set_container(container(
                chat_area_glass.get_container().get_x() + chat_area_glass.get_container().get_width() - PADDING -
                BUTTON_RIGHT_PADDING - 50.0F,
                static_cast<float>(scene_height) - INPUT_AREA_HEIGHT - PADDING + INPUT_AREA_HEIGHT / 2.0F - 25.0F,
                50.0f,
                50.0f
            ));

            attach_button.set_container(container(
                send_button.get_container().get_x() - PADDING - BUTTON_RIGHT_PADDING - 50.0F,
                send_button.get_container().get_y(),
                50.0f,
                50.0f
            ));

            messages_content_start_y = chat_header.get_container().get_y() + chat_header.get_container().get_height() +
                CHAT_HEADER_TO_MESSAGES_PADDING;

            visible_messages_height = chat_area_glass.get_container().get_height() - (chat_header.get_container().
                get_height() + CHAT_HEADER_VERTICAL_PADDING + CHAT_HEADER_TO_MESSAGES_PADDING + PADDING);

            visible_messages_height = std::max(0.0f, visible_messages_height);

            setup_chat_text_styles();
            chat_avatar.setup();
        }

        void clear_chat(const std::string& system_message)
        {
            current_conversation_id = "";
            current_conversation_cache = conversation_data();
            chat_title.set_text("No conversation selected");
            last_seen_text.set_text("");
            add_message(system_message, false);
        }

        void switch_conversation(const std::string& conversation_partner_name)
        {
            current_conversation_id = conversation_partner_name;

            if (p_data_manager->has_conversation(current_conversation_id))
            {
                 current_conversation_cache = p_data_manager->fetch_conversation_by_id(current_conversation_id);
            }
            else
            {
                p_data_manager->create_conversation(current_conversation_id, conversation_partner_name, "Online");
                current_conversation_cache = p_data_manager->fetch_conversation_by_id(current_conversation_id);
            }

            chat_title.set_text(current_conversation_cache.contact_name);
            last_seen_text.set_text(current_conversation_cache.is_online ? "Online" : current_conversation_cache.last_seen);
            rebuild_message_display(false);
            scroll_to_bottom(true);
        }

        void scroll_to_bottom(bool instant)
        {
            if (total_messages_height > visible_messages_height)
            {
                target_messages_scroll_y = total_messages_height - visible_messages_height;

                if (instant)
                {
                    messages_scroll_y = target_messages_scroll_y;
                    update_scrollbar();
                }
            }
        }

        void add_message(const std::string &content, bool is_sent, bool auto_scroll = true)
        {
            if (current_conversation_id.empty() && is_sent) return;
            if (!p_data_manager) return;

            std::string conv_id = current_conversation_id.empty() ? "SYSTEM" : current_conversation_id;

            if (!p_data_manager->has_conversation(conv_id)) {
                p_data_manager->create_conversation(conv_id, "System", "");
            }

            const message_data new_msg(content, is_sent, is_sent ? "You" : current_conversation_cache.contact_name);
            p_data_manager->add_message_to_conversation(conv_id, new_msg);

            if (conv_id == "SYSTEM") {
                current_conversation_cache = p_data_manager->fetch_conversation_by_id(conv_id);
            } else {
                current_conversation_cache.messages.push_back(new_msg);
            }


            rebuild_message_display(true);

            if (auto_scroll)
            {
                scroll_to_bottom(false);
            }
        }

        const std::string& get_current_conversation_id() const
        {
            return current_conversation_id;
        }

        void remove_conversation(const std::string &id)
        {
            if (current_conversation_id == id)
            {
                current_conversation_id.clear();
                chat_title.set_text("No conversation selected");
                last_seen_text.set_text("");
                rebuild_message_display(false);
            }
        }

        void send_current_message()
        {
            const std::string& message = input_button.get_current_text();
            if (!message.empty())
            {
                add_message(message, true);
                zc_kit::bridge::client_message(message);
                input_button.clear_text();
            }
        }

        void draw()
        {
            animate_scroll();

            chat_header.draw();
            input_button.draw();
            send_button.draw();
            attach_button.draw();

            if (total_messages_height > visible_messages_height)
            {
                scrollbar_track.draw();
                scrollbar_thumb.draw();
            }

            scissor::glScissorOp([this]
            {
                const float delta_time = time_util::get_delta_time();

                for (auto & am : animated_messages)
                {
                    const float offset_y = -messages_scroll_y;

                    if (am.animation_state == message_animation_state::ANIMATING_IN)
                    {
                        am.animation_timer += delta_time;

                        const float progress = std::min(1.0f, am.animation_timer / am.animation_duration);
                        const float eased_progress = 1.0F - static_cast<float>(std::pow(1.0f - progress, 2));

                        const float alpha_bubble = static_cast<float>(am.original_bubble_fill_color.get_alpha_u8()) *
                            eased_progress;
                        const float alpha_text = static_cast<float>(am.original_text_color.get_alpha_u8()) *
                            eased_progress;
                        const float alpha_timestamp = static_cast<float>(am.original_timestamp_color.get_alpha_u8()) *
                            eased_progress;

                        colour current_bubble_color = am.original_bubble_fill_color;
                        colour current_timestamp_color = am.original_timestamp_color;
                        colour current_text_color = am.original_text_color;

                        current_timestamp_color.set_alpha_u8(static_cast<uint8_t>(alpha_timestamp));
                        current_bubble_color.set_alpha_u8(static_cast<uint8_t>(alpha_bubble));
                        current_text_color.set_alpha_u8(static_cast<uint8_t>(alpha_text));

                        am.bubble->set_colour(current_bubble_color);
                        am.message_text->set_color(current_text_color);
                        am.message_timestamp->set_color(current_timestamp_color);

                        constexpr float slide_distance = 100.0f;
                        const float current_slide_offset = slide_distance * (1.0f - eased_progress);

                        float bubble_x_animated;

                        if (am.is_sent)
                        {
                            bubble_x_animated = am.original_bubble_container.get_x() + current_slide_offset;
                        }
                        else
                        {
                            bubble_x_animated = am.original_bubble_container.get_x() - current_slide_offset;
                        }

                        am.bubble->set_container(container(
                            bubble_x_animated,
                            am.original_bubble_container.get_y() + offset_y,
                            am.original_bubble_container.get_width(),
                            am.original_bubble_container.get_height()
                        ));

                        am.message_text->set_properties_position(
                            am.original_text_x + (am.is_sent ? current_slide_offset : -current_slide_offset),
                            am.original_text_y + offset_y);

                        am.message_timestamp->set_properties_position(am.original_timestamp_x +
                                                                   (am.is_sent
                                                                        ? current_slide_offset
                                                                        : -current_slide_offset),
                                                                   am.original_timestamp_y + offset_y);

                        if (progress >= 1.0f)
                        {
                            am.animation_state = message_animation_state::IDLE;
                        }
                    }
                    else
                    {
                        am.bubble->set_colour(am.original_bubble_fill_color);
                        am.message_text->set_color(am.original_text_color);
                        am.message_timestamp->set_color(am.original_timestamp_color);

                        am.bubble->set_container(container(
                            am.original_bubble_container.get_x(),
                            am.original_bubble_container.get_y() + offset_y,
                            am.original_bubble_container.get_width(),
                            am.original_bubble_container.get_height()
                        ));

                        am.message_text->set_properties_position(
                            am.original_text_x,
                            am.original_text_y + offset_y
                        );

                        am.message_timestamp->set_properties_position(
                            am.original_timestamp_x,
                            am.original_timestamp_y + offset_y
                        );
                    }

                    if (am.bubble->get_container().get_y() + am.bubble->get_container().get_height() >=
                        messages_content_start_y
                        && am.bubble->get_container().get_y() <= messages_content_start_y + visible_messages_height)
                    {
                        am.bubble->draw();
                        am.message_text->render();
                        am.message_timestamp->render();
                    }
                }
            }, container(chat_area_glass.get_container().get_x(), messages_content_start_y,
                         chat_area_glass.get_container().get_width() - PADDING - 10,
                         visible_messages_height), {});

            chat_avatar.draw();
            chat_title.render();
            last_seen_text.render();
            send_icon.render();
            attach_icon.render();
        }

        void draw_glassy()
        {
            chat_area_glass.draw();
            input_button.draw_glassy();
        }

        void update()
        {
        }

        void on_mouse_down(const zcg_mouse_pos_t mouse_pos, const int button)
        {
            if (button == ZCG_MOUSE_BUTTON_LEFT)
            {
                if (is_inside_scrollbar(mouse_pos))
                {
                    const float scale = perspective_util::get_current_display_config().scale / 2;
                    const float mouse_y = round(mouse_pos.y / scale);

                    scrollbar_drag_offset = mouse_y - scrollbar_thumb.get_container().get_y();

                    scrollbar_dragging = true;
                }
                else if (send_button.get_container().get_x() <= mouse_pos.x
                    && mouse_pos.x <= send_button.get_container().get_x() + send_button.get_container().get_width()
                    && send_button.get_container().get_y() <= mouse_pos.y
                    && mouse_pos.y <= send_button.get_container().get_y() + send_button.get_container().get_height())
                {
                    send_current_message();
                }
                else
                {
                    input_button.on_mouse_down(mouse_pos, button);
                }
            }
        }

        void on_mouse_up(const zcg_mouse_pos_t mouse_pos, const int button)
        {
            if (button == ZCG_MOUSE_BUTTON_LEFT)
            {
                scrollbar_dragging = false;
            }
        }

        void on_char_typed(unsigned int char_code)
        {
            if (input_button.get_is_typing())
            {
                input_button.on_char_input(char_code);
            }
        }

        void on_key_down(const zcg_key_event_t key_event)
        {
            if (key_event.key_code == ZCG_KEY_ENTER && key_event.is_pressed)
            {
                send_current_message();
            }
            else
            {
                input_button.on_key_down(key_event);
            }
        }

        void on_mouse_move(const zcg_mouse_pos_t &mouse_pos)
        {
            scrollbar_hovered = is_inside_scrollbar(mouse_pos);
            handle_scrollbar_drag(mouse_pos);

            input_button.on_mouse_move(mouse_pos);
        }

        void on_scroll(const zcg_scroll_event_t &scroll_event)
        {
            if (!is_in_area(chat_area_glass.get_container(), scroll_event))
            {
                return;
            }

            if (total_messages_height > visible_messages_height)
            {
                const float scroll_amount = scroll_event.delta_y * 50.0f;

                target_messages_scroll_y = std::max(0.0f, std::min(target_messages_scroll_y + scroll_amount,
                                                                   total_messages_height - visible_messages_height));
            }
        }

        [[nodiscard]] std::vector<std::string> get_conversation_ids() const
        {
            if (!p_data_manager)
            {
                return {};
            }

            return p_data_manager->fetch_all_conversation_ids();
        }

        [[nodiscard]] const conversation_data *get_conversation(const std::string &id) const
        {
            if (!p_data_manager)
            {
                return nullptr;
            }

            try
            {
                static conversation_data temporary;
                temporary = p_data_manager->fetch_conversation_by_id(id);

                return &temporary;
            }
            catch ([[maybe_unused]] const std::runtime_error &e)
            {
                return nullptr;
            }
        }
    };
}