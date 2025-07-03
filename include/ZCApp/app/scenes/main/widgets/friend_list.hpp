//
// Created by Damian Netter on 03/07/2025.
//

#pragma once

#include <vector>

#include "friend_button.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/framebuffer.hpp"
#include "ZCApp/graphics/utils/math_util.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"
#include "ZCApp/graphics/utils/time_util.hpp"

#define SCROLLBAR_WIDTH 2.0F

namespace zc_app
{
    class friend_list
    {
        std::vector<friend_button> friends_list;

        const float default_item_height = 65.0f;
        const float default_item_spacing = 8.0f;

        float current_scroll_offset_y = 0.0f;
        float target_scroll_offset_y = 0.0f;
        bool is_scrolling_smoothly = true;

        float scroll_animation_duration = 0.3f;
        float scroll_animation_timer = 0.0f;
        float scroll_start_offset_y = 0.0f;
        float scroll_sensitivity_multiplier = 2.0f;

        rectangle scrollbar_thumb_rect{colour(130, 130, 130, 40), colour(0, 0, 0, 200), 1, 7.5F};
        rectangle scrollbar_track_rect{colour(130, 0, 240, 100), 7.5f };

        const float min_thumb_height = 20.0f;

        container friends_container{};

        void create_friends_list(const float begin_y, const float sidebar_width, const float sidebar_height)
        {
            const float start_y = begin_y;
            const float item_height = default_item_height;

            friends_container.set_x(PADDING * 2);
            friends_container.set_y(start_y);
            friends_container.set_width(sidebar_width - PADDING * 4);
            friends_container.set_height(sidebar_height - start_y);

            const std::vector<std::string> friend_names = {
                "Sarah Wilson", "Mike Chen", "Emma Davis",
                "Alex Turner", "Lisa Park", "David Kim",
                "Rachel Green", "Tom Brown"
            };

            for (int i = 0; i < friend_names.size() && i < 8; ++i)
            {
                friend_button button;
                container container = friends_container;

                container.set_height(item_height);
                button.setup(container);
                friends_list.push_back(button);
            }
        }

    public:
        void initialize(const float begin_y, const float sidebar_width, const float sidebar_height)
        {
            create_friends_list(begin_y, sidebar_width, sidebar_height);
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

            constexpr float scroll_boundary_top = PADDING;

            const float scroll_boundary_bottom = content_height <= viewport_height
                                                     ? PADDING
                                                     : viewport_height - PADDING - content_height;

            current_scroll_offset_y = std::max(current_scroll_offset_y, scroll_boundary_bottom);
            current_scroll_offset_y = std::min(current_scroll_offset_y, scroll_boundary_top);
        }


        rectangle rect{colour(0, 0, 0, 255), colour(0, 0, 0, 200), 1, 7.5F};

        void draw(const float sidebar_glass_y)
        {
            glEnable(GL_SCISSOR_TEST);

            const float scale = (perspective_util::get_current_display_config().dpi_scale *
                perspective_util::get_current_display_config().scale) / 2;

            const auto scissor_x = GL_INT_C(friends_container.get_x() * scale);
            const auto scissor_y = GL_INT_C(sidebar_glass_y * scale);
            const auto scissor_w = GL_INT_C(friends_container.get_width() * scale);
            const auto scissor_h = GL_INT_C((friends_container.get_height() + PADDING) * scale);

            glScissor(scissor_x, scissor_y, scissor_w, scissor_h);

            const float container_screen_y = friends_container.get_y();

            for (size_t i = 0; i < friends_list.size(); ++i)
            {
                auto &button = friends_list[i];
                auto &container = button.get_container();

                const float item_base_relative_y = static_cast<float>(i) * (default_item_height + default_item_spacing);
                const float item_screen_y = container_screen_y + item_base_relative_y + current_scroll_offset_y;
                container.set_y(item_screen_y);

                button.render();
            }

            glDisable(GL_SCISSOR_TEST);

            const float content_height = get_total_content_height();
            const float viewport_height = friends_container.get_height();

            if (content_height > viewport_height)
            {
                scrollbar_track_rect.get_container().set_x(friends_container.get_x() + friends_container.get_width() + 2);
                scrollbar_track_rect.get_container().set_y(friends_container.get_y());
                scrollbar_track_rect.get_container().set_width(SCROLLBAR_WIDTH);
                scrollbar_track_rect.get_container().set_height(friends_container.get_height());

                scrollbar_track_rect.draw();

                const float thumb_proportional_height = viewport_height / content_height;
                float thumb_height = viewport_height * thumb_proportional_height;

                thumb_height = std::max(thumb_height, min_thumb_height);

                constexpr float scroll_boundary_top = 0;
                const float scroll_boundary_bottom = viewport_height - PADDING - content_height;

                const float scroll_range = scroll_boundary_top - scroll_boundary_bottom;
                const float current_scroll_from_top = scroll_boundary_top - current_scroll_offset_y;

                float normalized_scroll = 0.0f;

                if (scroll_range > 0.0f)
                {
                    normalized_scroll = current_scroll_from_top / scroll_range;
                }

                normalized_scroll = std::max(0.0f, std::min(1.0f, normalized_scroll));

                const float thumb_movable_range = friends_container.get_height() - thumb_height;
                const float thumb_y_offset_in_track = normalized_scroll * thumb_movable_range;

                scrollbar_thumb_rect.get_container().set_x(scrollbar_track_rect.get_container().get_x());
                scrollbar_thumb_rect.get_container().set_y(scrollbar_track_rect.get_container().get_y() + thumb_y_offset_in_track);
                scrollbar_thumb_rect.get_container().set_width(scrollbar_track_rect.get_container().get_width());
                scrollbar_thumb_rect.get_container().set_height(thumb_height + static_cast<float>(PADDING) / 2.0F);
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

        [[nodiscard]] float get_min_scroll_y()
        {
            return 0;
        }

        [[nodiscard]] bool is_in_scrollable_area(const float x, const float y) const
        // It would be appropriate to test this on different displays (non-retina)
        {
            const float scale = perspective_util::get_current_display_config().scale / 2;

            const float mouse_x = x / scale;
            const float mouse_y = y / scale;

            if (mouse_x < friends_container.get_x() ||
                mouse_x > friends_container.get_x() + friends_container.get_width() ||
                mouse_y < friends_container.get_y() ||
                mouse_y > friends_container.get_y() + friends_container.get_height())
            {
                return false;
            }

            return true;
        }

        void scroll(const zcg_scroll_event_t &scroll_event)
        {
            if (!is_in_scrollable_area(scroll_event.mouse_x, scroll_event.mouse_y))
            {
                return;
            }

            const float immediate_desired_offset_y = current_scroll_offset_y + scroll_event.delta_y *
                scroll_sensitivity_multiplier;

            constexpr float scroll_boundary_top = 0;
            const float content_height = get_total_content_height() - PADDING;
            const float viewport_height = friends_container.get_height();
            float scroll_boundary_bottom = content_height <= viewport_height
                                               ? PADDING
                                               : viewport_height - PADDING - content_height;


            float clamped_target_y = std::max(immediate_desired_offset_y, scroll_boundary_bottom);
            clamped_target_y = std::min(clamped_target_y, scroll_boundary_top);

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
