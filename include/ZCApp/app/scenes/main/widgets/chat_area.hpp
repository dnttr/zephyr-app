//
// Created by Damian Netter on 03/07/2025.
//

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "ZCApp/app/data_manager.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/app/scenes/main/widgets/input_message_button.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/graphics/textures/fan_texture.hpp"

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
        std::vector<animated_message> animated_messages;

        rectangle chat_area_glass{glass_tint, glass_border, 1, BORDER_RADIUS};
        rectangle chat_header{colour(glass_tint, 60), glass_border, 1, BORDER_RADIUS};

        rectangle send_button{accent_color, colour(255, 255, 255, 100), 1, 20.0F};
        rectangle attach_button{glass_tint, glass_border, 1, 20.0F};

        rectangle scrollbar_track{colour(255, 255, 255, 30), colour(255, 255, 255, 60), 1, 8.0F};
        rectangle scrollbar_thumb{colour(255, 255, 255, 120), colour(255, 255, 255, 180), 1, 8.0F};

        fan_texture chat_avatar{"avatar.png", 2.0F, 32};

        text send_icon, attach_icon;
        text chat_title, last_seen_text;

        input_message_button input_button;


        std::string current_conversation_id;
        conversation_data current_conversation_cache;

        const float TOP_BAR_HEIGHT = 70.0f;
        const float INPUT_AREA_HEIGHT = 80.0f;
        const float CHAT_HEADER_VERTICAL_PADDING = PADDING;
        const float CHAT_HEADER_TO_MESSAGES_PADDING = PADDING;
        const float MESSAGE_BUBBLE_BASE_HEIGHT = 45.0f;
        const float BUTTON_RIGHT_PADDING = 10.0f;
        const float MESSAGE_SPACING_VERTICAL = 60.0f;

        const float MESSAGE_TEXT_TOP_BOTTOM_PADDING = 12.0f;
        const float TIMESTAMP_HEIGHT_IN_BUBBLE = 15.0f;

        const float MIN_BUBBLE_WIDTH_SENT = 100.0f;
        const float MESSAGE_ANIMATION_DURATION = 0.02f;

        const float SCROLL_ANIMATION_SPEED = 8000.0f;

        float messages_scroll_y = 0.0f;
        float target_messages_scroll_y = 0.0f;
        float total_messages_height = 0.0f;
        float visible_messages_height = 0.0f;
        float messages_content_start_y = 0.0f;

        bool scrollbar_dragging = false;
        float scrollbar_drag_offset = 0.0f;
        bool scrollbar_hovered = false;

        int scene_width = 0;
        int scene_height = 0;

        data_manager *p_data_manager = nullptr;

        void setup_chat_text_styles();

        void update_scrollbar();

        void rebuild_message_display(bool animate_new_messages = false);

        static std::string format_timestamp(const std::chrono::system_clock::time_point &timestamp);

        void animate_scroll();

        void handle_scrollbar_drag(const zcg_mouse_pos_t &pos);

        void scroll_to_bottom(bool instant);

        void send_current_message();
    public:
        void initialize(float start_pos_x, data_manager *data_mgr);

        void draw();

        void draw_glassy();

        void on_char_typed(unsigned int char_code);

        void on_key_down(zcg_key_event_t key_event);

        void on_mouse_move(const zcg_mouse_pos_t &mouse_pos);

        void on_mouse_down(zcg_mouse_pos_t mouse_pos, int button);

        void on_mouse_up(zcg_mouse_pos_t mouse_pos, int button);

        void on_scroll(const zcg_scroll_event_t &scroll_event);

        void clear_chat(const std::string& system_message);

        void switch_conversation(const std::string& conversation_partner_name);

        void remove_conversation(const std::string &id);

        void add_message(const std::string &content, bool is_sent, bool auto_scroll = true);

        [[nodiscard]] const std::string& get_current_conversation_id() const;

        [[nodiscard]] std::vector<std::string> get_conversation_ids() const;

        [[nodiscard]] const conversation_data *get_conversation(const std::string &id) const;
    };
}
