//
// Created by Damian Netter on 03/07/2025.
//

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "ZCApp/app/scenes/main/widgets/chat_area.hpp"
#include "ZCApp/app/scenes/main/widgets/connection_button.hpp"

#define SCROLLBAR_WIDTH 2.0F

namespace zc_app
{
    struct connection_data
    {
        std::string name;
        int status;
    };

    class connection_list
    {
        std::vector<std::unique_ptr<connection_button>> connections_list;

        const float default_item_height = 42;
        const float default_item_spacing = 8.0f;

        float current_scroll_offset_y = 0.0f;
        float target_scroll_offset_y = 0.0f;
        bool is_scrolling_smoothly = true;

        float scroll_animation_duration = 0.3f;
        float scroll_animation_timer = 0.0f;
        float scroll_start_offset_y = 0.0f;
        float scroll_sensitivity_multiplier = 2.0f;

        rectangle scrollbar_thumb_rect{colour(130, 130, 130, 40), colour(0, 0, 0, 200), 1, 7.5F};
        rectangle scrollbar_track_rect{colour(130, 0, 240, 100), 7.5f};

        const float min_thumb_height = 20.0f;

        container connections_container{};

        std::function<void(const std::string&)> on_connection_selected_callback;

        void create_connections_list_uic(float begin_y, float sidebar_width, float sidebar_height);

    public:
        void initialize(float begin_y, float sidebar_width, float sidebar_height);

        void set_on_connection_selected_callback(std::function<void(const std::string&)> callback);

        void populate_connections(const std::vector<connection_data>& new_connections);

        void update(float delta_time);

        void draw();

        [[nodiscard]] float get_total_content_height() const;

        [[nodiscard]] float get_max_scroll_y() const;

        [[nodiscard]] float get_min_scroll_y() const;

        void on_mouse_down(const zcg_mouse_pos_t &mouse_pos_physical, int button) const;

        void on_mouse_move(const zcg_mouse_pos_t &mouse_pos_physical) const;

        void scroll(const zcg_scroll_event_t &scroll_event);
    };
}