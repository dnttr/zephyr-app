//
// Created by Damian Netter on 03/07/2025.
//

#pragma once

#include <vector>

#include "friend_button.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

namespace zc_app
{
    class friend_list
    {
        std::vector<friend_button> friends_list;

        const float default_item_height = 65.0f;
        const float default_item_spacing = 8.0f;

        float current_scroll_offset_y = 0.0f;
        bool is_scrolling_smoothly = false;
        float target_scroll_offset_y = 0.0f;

        container friends_container{};

        void create_friends_list(float begin_y, float sidebar_width, float sidebar_height)
        {
            float start_y = begin_y;
            float item_height = default_item_height;

            friends_container.set_x(PADDING * 2);
            friends_container.set_y(start_y);
            friends_container.set_width(sidebar_width - PADDING * 4);
            friends_container.set_height(sidebar_height - start_y);

            std::vector<std::string> friend_names = {
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

        void draw(const float sidebar_glass_y, float scene_width, float scene_height)
        {
            glEnable(GL_SCISSOR_TEST);

            const float scale = (perspective_util::get_current_display_config().dpi_scale *
                perspective_util::get_current_display_config().scale) / 2;

            glScissor(friends_container.get_x() * scale, sidebar_glass_y * scale,
                      friends_container.get_width() * scale,
                      (friends_container.get_height() + PADDING) * scale);

            float container_screen_y = friends_container.get_y();

            for (size_t i = 0; i < friends_list.size(); ++i)
            {
                auto &friend_btn = friends_list[i];
                auto &container = friend_btn.get_container();

                float item_base_relative_y = i * (default_item_height + default_item_spacing);
                float item_screen_y = container_screen_y + item_base_relative_y + current_scroll_offset_y;

                container.set_y(item_screen_y);

                friend_btn.render();
            }

            glDisable(GL_SCISSOR_TEST);
        }

        float get_total_content_height()
        {
            if (friends_list.empty())
            {
                return 0.0f;
            }

            return friends_list.size() * default_item_height +
                (friends_list.size() > 0 ? (friends_list.size() - 1) * default_item_spacing : 0.0f);
        }

        float get_max_scroll_y()
        {
            float viewport_height = friends_container.get_height();
            float content_height = get_total_content_height();

            if (content_height <= viewport_height)
            {
                return 0.0f;
            }

            return -(content_height - viewport_height + PADDING);
        }

        float get_min_scroll_y()
        {
            return PADDING;
        }

        void scroll(zcg_scroll_event_t scroll_event)
        {
            float desired_new_offset_y = current_scroll_offset_y + scroll_event.delta_y;

            float scroll_boundary_top = PADDING;

            float content_height = get_total_content_height();
            float viewport_height = friends_container.get_height();

            float scroll_boundary_bottom;

            if (content_height <= viewport_height)
            {
                scroll_boundary_bottom = PADDING;
            }
            else
            {
                scroll_boundary_bottom = viewport_height - PADDING - content_height;
            }

            current_scroll_offset_y = std::max(desired_new_offset_y, scroll_boundary_bottom);
            current_scroll_offset_y = std::min(current_scroll_offset_y, scroll_boundary_top);
        }
    };
}
