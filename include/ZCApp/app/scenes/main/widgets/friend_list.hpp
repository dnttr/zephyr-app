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

        container friends_container{};

        void create_friends_list(float begin_y, float sidebar_width)
        {
            float start_y = begin_y + 50;
            float item_height = default_item_height;

            friends_container.set_x(PADDING * 2);
            friends_container.set_y(start_y);
            friends_container.set_width(sidebar_width - PADDING * 4);
            friends_container.set_height(item_height);

            std::vector<std::string> friend_names = {
                "Sarah Wilson", "Mike Chen", "Emma Davis",
                "Alex Turner", "Lisa Park", "David Kim",
                "Rachel Green", "Tom Brown"
            };

            for (int i = 0; i < friend_names.size() && i < 8; ++i)
            {
                friend_button button;
                container container = friends_container;

                const float increment = friends_container.get_y() + i * (item_height + default_item_spacing);
                container.set_y(increment);

                button.setup(container);
                friends_list.push_back(button);

                container.set_height(increment);
            }
        }

    public:
        void initialize(const float begin_y, const float sidebar_width)
        {
            create_friends_list(begin_y, sidebar_width);
        }

        void draw(const float sidebar_glass_y, float scene_width, float scene_height)
        {
            glEnable(GL_SCISSOR_TEST);

            const float scale = (perspective_util::get_current_display_config().dpi_scale *
                perspective_util::get_current_display_config().scale) / 2;

            glScissor(friends_container.get_x() * scale, (sidebar_glass_y + PADDING) * scale,
                      friends_container.get_width() * scale,
                      (friends_container.get_y() + friends_container.get_height() + PADDING) * scale);

            for (auto &friend_btn : friends_list)
            {
                friend_btn.render();
            }

            glDisable(GL_SCISSOR_TEST);
        }
    };
}
