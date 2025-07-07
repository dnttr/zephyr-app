//
// Created by Damian Netter on 07/07/2025.
//

#include "ZCApp/app/scenes/main/widgets/chat_area.hpp"

#include "ZCApp/app/scenes/utils.hpp"
#include "ZCApp/graphics/utils/scissor.hpp"
#include "ZCApp/graphics/utils/time_util.hpp"
#include "ZCKit/bridge.hpp"

namespace zc_app
{
    void chat_area::setup_chat_text_styles()
    {
        const std::string chat_title_str = "No conversation selected";
        const auto chat_title_str_len = static_cast<float>(chat_title_str.length());

        const auto roboto_medium = font_manager::get_font("Roboto-Medium");
        const auto roboto_regular = font_manager::get_font("Roboto-Regular");

        constexpr float font_default_size = 128.0F;

        text_style default_style;
        default_style.text_size_magnification = 0.07F;
        default_style.text_color = colour(20, 20, 20, 255);
        default_style.outline_enable = false;
        default_style.shadow_enable = true;
        default_style.shadow_offset = {2.0F, 2.0F};
        default_style.shadow_color = colour(0, 0, 0, 100);

        text_style status_style = default_style;
        status_style.text_size_magnification = 0.05F;
        status_style.text_color = colour(20, 20, 20, 200);

        text_style chat_header_style = default_style;
        chat_header_style.text_size_magnification = 0.08F;

        text_style icon_style = default_style;
        icon_style.text_size_magnification = 0.09F;

        const float chat_title_width = font_default_size * default_style.text_size_magnification * chat_title_str_len /
            2.0F;

        const float chat_title_x = chat_header.get_container().get_x() + chat_title_width - PADDING * 3.5F;
        const float chat_title_y = chat_header.get_container().get_y() + chat_header.get_container().get_height() / 2.0F -
            PADDING;

        container chat_title_container(chat_title_x, chat_title_y);
        chat_title.initialize(
            chat_title_str,
            chat_title_container,
            roboto_medium,
            chat_header_style
        );

        container last_seen_text_container(chat_title_x, chat_title_y + PADDING);
        last_seen_text.initialize(
            "",
            last_seen_text_container,
            roboto_regular,
            status_style
        );

        container send_icon_container(send_button.get_container().get_x() + 18, send_button.get_container().get_y() + 12);
        send_icon.initialize(
            "",
            send_icon_container,
            roboto_regular,
            icon_style
        );

        container attach_icon_container(attach_button.get_container().get_x() + 18,
                                        attach_button.get_container().get_y() + 12);
        attach_icon.initialize(
            "", attach_icon_container,
            roboto_regular,
            icon_style
        );
    }

    void chat_area::update_scrollbar()
    {
        if (total_messages_height <= visible_messages_height)
        {
            scrollbar_track.set_container(container(0, 0, 0, 0));
            scrollbar_thumb.set_container(container(0, 0, 0, 0));
            return;
        }

        constexpr float scrollbar_width = 12.0F;

        const float scrollbar_x = chat_area_glass.get_container().get_x() + chat_area_glass.get_container().get_width() -
            PADDING;
        const float scrollbar_y = messages_content_start_y;
        const float scrollbar_height = visible_messages_height;

        scrollbar_track.set_container(container(scrollbar_x, scrollbar_y, scrollbar_width, scrollbar_height));

        const float thumb_ratio = visible_messages_height / total_messages_height;
        const float thumb_height = std::max(30.0F, scrollbar_height * thumb_ratio);

        const float scroll_ratio = total_messages_height > visible_messages_height
                                       ? messages_scroll_y / (total_messages_height - visible_messages_height)
                                       : 0.0F;

        const float thumb_y = scrollbar_y + (scrollbar_height - thumb_height) * scroll_ratio;
        scrollbar_thumb.set_container(container(scrollbar_x, thumb_y, scrollbar_width, thumb_height));
    }

    void chat_area::rebuild_message_display(bool animate_new_messages)
    {
        std::vector<animated_message> new_animated_messages;

        if (current_conversation_id.empty() || current_conversation_cache.contact_name.empty())
        {
            total_messages_height = 0.0F;
            animated_messages.clear();

            update_scrollbar();

            return;
        }

        float chat_content_width = chat_area_glass.get_container().get_width() - PADDING * 2 - 20;
        const auto &conversation = current_conversation_cache;

        float current_relative_y = PADDING;

        const auto existing_msg_count = animated_messages.size();
        const auto conversation_msg_count = conversation.messages.size();

        const auto font = font_manager::get_font("Roboto-Regular");

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

                float desired_content_width = estimated_wrapped_text_width_px + MESSAGE_TEXT_LEFT_RIGHT_PADDING * 2 +
                    TIMESTAMP_HEIGHT_IN_BUBBLE + 20;

                calculated_bubble_width = std::min(bubble_max_container_width,
                                                   std::max(MIN_BUBBLE_WIDTH_SENT, desired_content_width));
            }

            const float bubble_text_height = static_cast<float>(num_lines) * ESTIMATED_LINE_HEIGHT;

            float bubble_height = MESSAGE_TEXT_TOP_BOTTOM_PADDING * 2 + bubble_text_height + TIMESTAMP_HEIGHT_IN_BUBBLE;
            bubble_height = std::max(bubble_height, MESSAGE_BUBBLE_BASE_HEIGHT);

            float bubble_x;

            if (is_sent)
            {
                bubble_x = chat_area_glass.get_container().get_x() + chat_area_glass.get_container().get_width() - PADDING -
                    calculated_bubble_width - 10;
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
                          messages_content_start_y + current_relative_y + MESSAGE_TEXT_TOP_BOTTOM_PADDING), font,
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
                font,
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

    std::string chat_area::format_timestamp(const std::chrono::system_clock::time_point &timestamp)
    {
        const auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        const auto tm = *std::localtime(&time_t);

        char buffer[16];
        std::strftime(buffer, sizeof(buffer), "%H:%M", &tm);

        return {buffer};
    }

    void chat_area::animate_scroll()
    {
        if (std::abs(messages_scroll_y - target_messages_scroll_y) > 0.5f)
        {
            const float delta = time_util::get_delta_time();
            const float step = SCROLL_ANIMATION_SPEED * delta;

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

    void chat_area::handle_scrollbar_drag(const zcg_mouse_pos_t &pos)
    {
        if (!scrollbar_dragging)
        {
            return;
        }

        const float scale = perspective_util::get_current_display_config().scale / 2;
        const float mouse_y = round(pos.y / scale);

        const auto &track_container = scrollbar_track.get_container();
        const float relative_y = mouse_y - track_container.get_y() - scrollbar_drag_offset;

        const float scroll_area_height = track_container.get_height() - scrollbar_thumb.get_container().get_height();

        float scroll_ratio = (scroll_area_height > 0) ? (relative_y / scroll_area_height) : 0.0f;
        scroll_ratio = std::max(0.0f, std::min(1.0f, scroll_ratio));

        target_messages_scroll_y = scroll_ratio * (total_messages_height - visible_messages_height);
        messages_scroll_y = target_messages_scroll_y;

        update_scrollbar();
    }

    void chat_area::initialize(const float start_pos_x, data_manager *data_mgr)
    {
        scene_width = 960;
        scene_height = 540;

        p_data_manager = data_mgr;

        chat_area_glass.set_container(container(
            start_pos_x, PADDING,
            static_cast<float>(scene_width) - start_pos_x - PADDING,
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
            start_pos_x,
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

    void chat_area::clear_chat(const std::string &system_message)
    {
        current_conversation_cache = conversation_data();
        current_conversation_id = "";

        chat_title.set_text("No conversation selected");
        last_seen_text.set_text("");

        add_message(system_message, false);
    }

    void chat_area::switch_conversation(const std::string &conversation_partner_name)
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

    void chat_area::scroll_to_bottom(bool instant)
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

    void chat_area::add_message(const std::string &content, const bool is_sent, const bool auto_scroll)
    {
        if ((current_conversation_id.empty() && is_sent) || !p_data_manager)
        {
            return;
        }

        const std::string conv_id = current_conversation_id.empty() ? "SYSTEM" : current_conversation_id;

        if (!p_data_manager->has_conversation(conv_id))
        {
            p_data_manager->create_conversation(conv_id, "System", "");
        }

        const message_data new_msg(content, is_sent, is_sent ? "You" : current_conversation_cache.contact_name);
        p_data_manager->add_message_to_conversation(conv_id, new_msg);

        if (conv_id == "SYSTEM")
        {
            current_conversation_cache = p_data_manager->fetch_conversation_by_id(conv_id);
        }
        else
        {
            current_conversation_cache.messages.push_back(new_msg);
        }

        rebuild_message_display(true);

        if (auto_scroll)
        {
            scroll_to_bottom(false);
        }
    }

    const std::string &chat_area::get_current_conversation_id() const
    {
        return current_conversation_id;
    }

    void chat_area::remove_conversation(const std::string &id)
    {
        if (current_conversation_id == id)
        {
            current_conversation_id.clear();

            chat_title.set_text("No conversation selected");
            last_seen_text.set_text("");

            rebuild_message_display(false);
        }
    }

    void chat_area::send_current_message()
    {
        if (const std::string &message = input_button.get_current_text(); !message.empty())
        {
            add_message(message, true);
            zc_kit::bridge::client_message(message);
            input_button.clear_text();
        }
    }

    void chat_area::draw()
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

            for (auto &[bubble, message_text, message_timestamp, is_sent, animation_state, animation_timer, animation_duration, original_bubble_container, original_bubble_fill_color, original_text_x, original_text_y, original_text_color, original_timestamp_x, original_timestamp_y, original_timestamp_color] : animated_messages)
            {
                const float offset_y = -messages_scroll_y;

                if (animation_state == message_animation_state::ANIMATING_IN)
                {
                    animation_timer += delta_time;

                    const float progress = std::min(1.0f, animation_timer / animation_duration);
                    const float eased_progress = 1.0F - static_cast<float>(std::pow(1.0f - progress, 2));

                    const float alpha_bubble = static_cast<float>(original_bubble_fill_color.get_alpha_u8()) *
                        eased_progress;
                    const float alpha_text = static_cast<float>(original_text_color.get_alpha_u8()) *
                        eased_progress;
                    const float alpha_timestamp = static_cast<float>(original_timestamp_color.get_alpha_u8()) *
                        eased_progress;

                    colour current_bubble_color = original_bubble_fill_color;
                    colour current_timestamp_color = original_timestamp_color;
                    colour current_text_color = original_text_color;

                    current_timestamp_color.set_alpha_u8(static_cast<uint8_t>(alpha_timestamp));
                    current_bubble_color.set_alpha_u8(static_cast<uint8_t>(alpha_bubble));
                    current_text_color.set_alpha_u8(static_cast<uint8_t>(alpha_text));

                    bubble->set_colour(current_bubble_color);
                    message_text->set_color(current_text_color);
                    message_timestamp->set_color(current_timestamp_color);

                    constexpr float slide_distance = 100.0f;
                    const float current_slide_offset = slide_distance * (1.0f - eased_progress);

                    float bubble_x_animated;

                    if (is_sent)
                    {
                        bubble_x_animated = original_bubble_container.get_x() + current_slide_offset;
                    }
                    else
                    {
                        bubble_x_animated = original_bubble_container.get_x() - current_slide_offset;
                    }

                    bubble->set_container(container(
                        bubble_x_animated,
                        original_bubble_container.get_y() + offset_y,
                        original_bubble_container.get_width(),
                        original_bubble_container.get_height()
                    ));

                    message_text->set_properties_position(
                        original_text_x + (is_sent ? current_slide_offset : -current_slide_offset),
                        original_text_y + offset_y);

                    message_timestamp->set_properties_position(original_timestamp_x +
                                                                  (is_sent
                                                                       ? current_slide_offset
                                                                       : -current_slide_offset),
                                                                  original_timestamp_y + offset_y);

                    if (progress >= 1.0f)
                    {
                        animation_state = message_animation_state::IDLE;
                    }
                }
                else
                {
                    bubble->set_colour(original_bubble_fill_color);
                    message_text->set_color(original_text_color);
                    message_timestamp->set_color(original_timestamp_color);

                    bubble->set_container(container(
                        original_bubble_container.get_x(),
                        original_bubble_container.get_y() + offset_y,
                        original_bubble_container.get_width(),
                        original_bubble_container.get_height()
                    ));

                    message_text->set_properties_position(
                        original_text_x,
                        original_text_y + offset_y
                    );

                    message_timestamp->set_properties_position(
                        original_timestamp_x,
                        original_timestamp_y + offset_y
                    );
                }

                if (bubble->get_container().get_y() + bubble->get_container().get_height() >=
                    messages_content_start_y
                    && bubble->get_container().get_y() <= messages_content_start_y + visible_messages_height)
                {
                    bubble->draw();
                    message_text->render();
                    message_timestamp->render();
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

    void chat_area::draw_glassy()
    {
        chat_area_glass.draw();
        input_button.draw_glassy();
    }

    void chat_area::on_mouse_down(const zcg_mouse_pos_t mouse_pos, const int button)
    {
        if (button == ZCG_MOUSE_BUTTON_LEFT)
        {
            if (is_in_area(scrollbar_thumb.get_container(), mouse_pos))
            {
                const float scale = perspective_util::get_current_display_config().scale / 2;
                const float mouse_y = round(mouse_pos.y / scale);

                scrollbar_drag_offset = mouse_y - scrollbar_thumb.get_container().get_y();

                scrollbar_dragging = true;
            }
            else if (is_in_area(send_button.get_container(), mouse_pos))
            {
                send_current_message();
            }
            else
            {
                input_button.on_mouse_down(mouse_pos, button);
            }
        }
    }

    void chat_area::on_mouse_up(const zcg_mouse_pos_t mouse_pos, const int button)
    {
        if (button == ZCG_MOUSE_BUTTON_LEFT)
        {
            scrollbar_dragging = false;
        }
    }

    void chat_area::on_char_typed(unsigned int char_code)
    {
        if (input_button.get_is_typing())
        {
            input_button.on_char_input(char_code);
        }
    }

    void chat_area::on_key_down(const zcg_key_event_t key_event)
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

    void chat_area::on_mouse_move(const zcg_mouse_pos_t &mouse_pos)
    {
        scrollbar_hovered = is_in_area(scrollbar_thumb.get_container(), mouse_pos);
        handle_scrollbar_drag(mouse_pos);

        input_button.on_mouse_move(mouse_pos);
    }

    void chat_area::on_scroll(const zcg_scroll_event_t &scroll_event)
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

    std::vector<std::string> chat_area::get_conversation_ids() const
    {
        if (!p_data_manager)
        {
            return {};
        }

        return p_data_manager->fetch_all_conversation_ids();
    }

    const conversation_data *chat_area::get_conversation(const std::string &id) const
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
}