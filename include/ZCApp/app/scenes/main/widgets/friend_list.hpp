#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>

#include "chat_area.hpp"
#include "friend_button.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/utils/math_util.hpp"
#include "ZCApp/graphics/utils/scissor.hpp"
#include "ZCKit/bridge.hpp"

namespace zc_app
{
    class chat_area;
}

#define SCROLLBAR_WIDTH 2.0F

namespace zc_app
{
    struct connection_data
    {
        std::string name;
        int status;
    };

    class friend_list
    {
        std::vector<std::unique_ptr<friend_button>> friends_list;

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

        container friends_container{};

        std::function<void(const std::string&)> on_friend_selected_callback;

        void create_friends_list_ui_container(const float begin_y, const float sidebar_width,
                                              const float sidebar_height)
        {
            const float start_y = begin_y;

            friends_container.set_x(PADDING * 2);
            friends_container.set_y(start_y);
            friends_container.set_width(sidebar_width - PADDING * 4);
            friends_container.set_height(sidebar_height - start_y);
        }

    public:
        void initialize(const float begin_y, const float sidebar_width, const float sidebar_height)
        {
            create_friends_list_ui_container(begin_y, sidebar_width, sidebar_height);
        }

        void set_on_friend_selected_callback(std::function<void(const std::string&)> callback)
        {
            on_friend_selected_callback = std::move(callback);
        }

        void populate_friends(const std::vector<connection_data>& new_friends)
        {
            friends_list.clear();
            float current_item_y_relative_to_friends_container_top = 0.0f;

            for (const auto& friend_info : new_friends)
            {
                std::string status_string = "Offline";
                switch (friend_info.status) {
                    case 1: status_string = "Online"; break;
                    case 2: status_string = "In Chat"; break;
                }

                auto new_button = std::make_unique<friend_button>(
                    friend_info.name,
                    friend_info.name,
                    status_string,
                    "avatar.png"
                );

                container button_container(
                    friends_container.get_x(),
                    friends_container.get_y() + current_item_y_relative_to_friends_container_top,
                    friends_container.get_width(),
                    default_item_height
                );

                new_button->setup(button_container);
                new_button->set_on_click_callback([this](const std::string& id) {
                    if(on_friend_selected_callback)
                    {
                        on_friend_selected_callback(id);
                    }
                });

                friends_list.push_back(std::move(new_button));
                current_item_y_relative_to_friends_container_top += default_item_height + default_item_spacing;
            }
        }

        void update(const float delta_time)
        {
            if (!is_scrolling_smoothly)
            {
                return;
            }

            scroll_animation_timer += delta_time;
            const float t_norm = scroll_animation_timer / scroll_animation_duration;

            if (t_norm >= 1.0f)
            {
                current_scroll_offset_y = target_scroll_offset_y;
                is_scrolling_smoothly = false;
                return;
            }

            const float eased_progress = math_util::ease_out(t_norm);

            current_scroll_offset_y = scroll_start_offset_y + (target_scroll_offset_y - scroll_start_offset_y) *
                eased_progress;

            const float content_height = get_total_content_height();
            const float viewport_height = friends_container.get_height();

            constexpr float scroll_boundary_top = 0.0f;
            const float scroll_boundary_bottom = content_height <= viewport_height
                                                     ? 0.0f
                                                     : -(content_height - viewport_height);


            current_scroll_offset_y = std::max(current_scroll_offset_y, scroll_boundary_bottom);
            current_scroll_offset_y = std::min(current_scroll_offset_y, scroll_boundary_top);
        }

        void draw(const float sidebar_glass_y_for_scissor_calc)
        {
            scissor::glScissorOp([this]
            {
                for (auto &button_ptr : friends_list)
                {
                    container current_button_container_for_draw = button_ptr->get_container();

                    current_button_container_for_draw.set_y(
                        current_button_container_for_draw.get_y() + current_scroll_offset_y);

                    if (current_button_container_for_draw.get_y() + current_button_container_for_draw.get_height() >=
                        friends_container.get_y() &&
                        current_button_container_for_draw.get_y() <= friends_container.get_y() + friends_container.
                        get_height())
                    {
                        container original_button_shape_container = button_ptr->button_shape.get_container();
                        button_ptr->button_shape.set_container(current_button_container_for_draw);
                        button_ptr->render();
                        button_ptr->button_shape.set_container(original_button_shape_container);
                    }
                }
            }, friends_container, {});

            const float content_height = get_total_content_height();
            const float viewport_height = friends_container.get_height();

            if (content_height > viewport_height)
            {
                scrollbar_track_rect.set_container(container(
                    friends_container.get_x() + friends_container.get_width() + 2,
                    friends_container.get_y(),
                    SCROLLBAR_WIDTH,
                    friends_container.get_height()
                ));
                scrollbar_track_rect.draw();

                const float thumb_proportional_height = viewport_height / content_height;
                float thumb_height = viewport_height * thumb_proportional_height;
                thumb_height = std::max(thumb_height, min_thumb_height);

                const float max_scroll_y = get_max_scroll_y();
                const float normalized_scroll = (max_scroll_y != 0) ? (current_scroll_offset_y / max_scroll_y) : 0;

                const float thumb_movable_range_in_track = friends_container.get_height() - thumb_height;
                const float thumb_y_offset_in_track = normalized_scroll * thumb_movable_range_in_track;

                scrollbar_thumb_rect.set_container(container(
                    scrollbar_track_rect.get_container().get_x(),
                    scrollbar_track_rect.get_container().get_y() + thumb_y_offset_in_track,
                    scrollbar_track_rect.get_container().get_width(),
                    thumb_height
                ));
                scrollbar_thumb_rect.draw();
            }
        }

        [[nodiscard]] float get_total_content_height() const
        {
            if (friends_list.empty())
            {
                return 0.0F;
            }

            const auto friends_list_size = static_cast<float>(friends_list.size());

            return friends_list_size * default_item_height +
                (friends_list_size > 0 ? (friends_list_size - 1) * default_item_spacing : 0.0F);
        }

        [[nodiscard]] float get_max_scroll_y() const
        {
            const float viewport_height = friends_container.get_height();
            const float content_height = get_total_content_height();

            if (content_height <= viewport_height)
            {
                return 0.0F;
            }

            return -(content_height - viewport_height);
        }

        [[nodiscard]] float get_min_scroll_y() const
        {
            return 0.0f;
        }

        void on_mouse_down(const zcg_mouse_pos_t &mouse_pos_physical, int button)
        {
            if (!is_in_area(friends_container, mouse_pos_physical))
            {
                return;
            }

            for (auto &btn_ptr : friends_list)
            {
                btn_ptr->on_mouse_down(mouse_pos_physical, button);
            }
        }

        void on_mouse_move(const zcg_mouse_pos_t &mouse_pos_physical)
        {
            for (auto &btn_ptr : friends_list)
            {
                btn_ptr->on_mouse_move(mouse_pos_physical);
            }
        }

        void scroll(const zcg_scroll_event_t &scroll_event)
        {
            zcg_mouse_pos_t physical_scroll_pos = {scroll_event.mouse_x, scroll_event.mouse_y};
            if (!zc_app::is_in_area(friends_container, physical_scroll_pos))
            {
                return;
            }

            const float scroll_amount_virtual = scroll_event.delta_y * scroll_sensitivity_multiplier;

            float immediate_desired_offset_y = current_scroll_offset_y + scroll_amount_virtual;

            const float max_scroll_offset = 0.0f;
            const float min_scroll_offset = get_max_scroll_y();

            float clamped_target_y = std::max(immediate_desired_offset_y, min_scroll_offset);
            clamped_target_y = std::min(clamped_target_y, max_scroll_offset);

            if (std::abs(clamped_target_y - current_scroll_offset_y) < 0.1F && !is_scrolling_smoothly)
            {
                return;
            }

            target_scroll_offset_y = clamped_target_y;
            scroll_start_offset_y = current_scroll_offset_y;
            scroll_animation_timer = 0.0F;

            is_scrolling_smoothly = true;
        }
    };
}