#pragma once

#include "widgets/friend_button.hpp"
#include "widgets/friend_list.hpp"
#include "widgets/input_message_button.hpp"
#include "widgets/chat_area.hpp"
#include "widgets/connection_modal.hpp"
#include "widgets/request_modal.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/effects/partial_blur.hpp"
#include "ZCApp/graphics/objects/background.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/graphics/scene/scene.hpp"
#include "ZCApp/graphics/textures/fan_texture.hpp"
#include "ZCApp/graphics/textures/texture.hpp"
#include "ZCApp/graphics/effects/fullscreen_blur.hpp"
#include "ZCKit/bridge.hpp"
#include "nlohmann/json.hpp"
#include <mutex>
#include <vector>
#include <functional>

namespace zc_app
{
    class main_scene final : public scene
    {
        fullscreen_blur blur_background{};
        partial_blur blur_partial{};

        background effect_background{};

        rectangle sidebar_glass{glass_tint, glass_border, 1, BORDER_RADIUS};
        rectangle profile_section{colour(glass_tint, 60), glass_border, 1, BORDER_RADIUS};
        rectangle separator_bar{colour(glass_tint, 60), colour(255, 255, 255, 255), 1, 20.0F};
        rectangle connection_header{colour(glass_tint, 60), colour(255, 255, 255, 255), 1, 12.0F};

        fan_texture user_avatar_texture{"avatar.png", 2.0F, 128};

        text search_placeholder, search_icon;
        text username_text, user_status_text;
        text friends_title;

        int scene_width = 0;
        int scene_height = 0;

        bool is_identified = false;
        bool is_awaiting_relay_response = false;
        bool is_network_connected = false;

        std::string my_username;
        std::string active_chat_partner;

        data_manager app_data_manager;

        friend_list connections_list;
        chat_area chat_area_widget;
        connection_modal connection_modal_widget{};
        request_modal request_modal_widget{};

        std::mutex main_thread_action_mutex;
        std::vector<std::function<void()>> main_thread_actions;

        void queue_main_thread_action(std::function<void()> action);

        void process_main_thread_actions();

        void setup_bridge_callbacks();

        void setup_sidebar_text_styles();

        void draw_components();

    public:
        void initialize(int scene_width, int scene_height) override;

        void render() override;

        void update() override;

        void resize(int width, int height) override;

        void destroy() override;

        void scroll(const zcg_scroll_event_t &scroll_event);

        void on_mouse_up(zcg_mouse_pos_t mouse_pos, int button);

        void on_mouse_move(const zcg_mouse_pos_t &mouse_pos);

        void on_mouse_down(zcg_mouse_pos_t mouse_pos, int button);

        void on_key_up(zcg_key_event_t key_event);

        void on_key_down(zcg_key_event_t key_event);

        void on_char_typed(unsigned int char_code);
    };
}
