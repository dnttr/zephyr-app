#pragma once

#include "widgets/friend_button.hpp"
#include "widgets/friend_list.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/effects/partial_blur.hpp"
#include "ZCApp/graphics/objects/background.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/graphics/scene/scene.hpp"
#include "ZCApp/graphics/textures/fan_texture.hpp"
#include "ZCApp/graphics/textures/texture.hpp"
#include "ZCApp/graphics/utils/perspective_util.hpp"

namespace zc_app
{
    class main_scene final : public scene
    {
        colour glass_tint{255, 255, 255, 25};
        colour glass_border{255, 255, 255, 40};
        colour dark_glass{0, 0, 0, 120};
        colour accent_color{106, 156, 234};

        colour bg_gradient_start{102, 126, 234};
        colour bg_gradient_end{118, 75, 162};

        partial_blur blur_effect{};
        background bg{};

        rectangle sidebar_glass{glass_tint, glass_border, 1, BORDER_RADIUS};
        rectangle chat_area_glass{glass_tint, glass_border, 1, BORDER_RADIUS};
        rectangle input_area_glass{glass_tint, glass_border, 1, BORDER_RADIUS};
        rectangle top_bar_glass{dark_glass, glass_border, 1, BORDER_RADIUS};
        rectangle profile_section{dark_glass, glass_border, 1, BORDER_RADIUS};
        rectangle message_input{colour(255, 255, 255, 30), accent_color, 2, 25.0F};
        rectangle send_button{accent_color, colour(255, 255, 255, 100), 1, 20.0F};
        rectangle attach_button{glass_tint, glass_border, 1, 20.0F};

        rectangle search_bar{colour(255, 255, 255, 20), colour(255, 255, 255, 60), 1, 20.0F};

        fan_texture user_avatar{"avatar.png", 2.0F, 32};
        fan_texture chat_avatar{"avatar.png", 2.0F, 32};

        text input_placeholder, send_icon, attach_icon;
        text search_placeholder, search_icon;
        text username_text, user_status_text;
        text chat_title, last_seen_text;
        text friends_title;

        rectangle friends_header{dark_glass, colour(255, 255, 255, 60), 1, 8.0F};
        rectangle chat_header{dark_glass, glass_border, 1, BORDER_RADIUS};

        std::vector<rectangle> message_bubbles;
        std::vector<std::unique_ptr<text>> message_texts;

        int scene_width = 0;
        int scene_height = 0;

        float typing_animation = 0.0f;
        bool show_typing = false;

        friend_list f_list;

    public:
        void initialize(int scene_width, int scene_height) override
        {
            this->scene_width = scene_width;
            this->scene_height = scene_height;

            float sidebar_width = std::min(320.0f, scene_width * 0.25f);
            float top_bar_height = 70.0f;
            float input_height = 80.0f;

            sidebar_glass.set_container(container(
                PADDING, PADDING,
                sidebar_width - PADDING,
                scene_height - PADDING * 2
            ));

            profile_section.set_container(container(
                PADDING * 2, PADDING * 2,
                sidebar_width - PADDING * 4,
                90.0f
            ));

            user_avatar.set_container(container(
                PADDING * 3, PADDING * 3,
                50.0f, 50.0f
            ));

            search_bar.set_container(container(
                PADDING * 2, profile_section.get_container().get_y() + 100,
                sidebar_width - PADDING * 4,
                40.0f
            ));

            friends_header.set_container(container(
                PADDING * 2, search_bar.get_container().get_y() + 55,
                sidebar_width - PADDING * 4,
                35.0f
            ));

            float chat_x = sidebar_width + PADDING;
            chat_area_glass.set_container(container(
                chat_x, PADDING,
                scene_width - chat_x - PADDING,
                scene_height - input_height - PADDING * 3
            ));

            chat_header.set_container(container(
                chat_x + PADDING, PADDING * 2,
                scene_width - chat_x - PADDING * 3,
                top_bar_height
            ));

            chat_avatar.set_container(container(
                chat_x + PADDING * 2, PADDING * 3,
                40.0f, 40.0f
            ));

            input_area_glass.set_container(container(
                chat_x, scene_height - input_height - PADDING,
                scene_width - chat_x - PADDING,
                input_height
            ));

            message_input.set_container(container(
                chat_x + PADDING * 2, scene_height - input_height - PADDING + input_height / 2 - 25,
                scene_width - chat_x - PADDING * 6 - 120,
                50.0f
            ));

            send_button.set_container(container(
                scene_width - PADDING * 3 - 50,
                scene_height - input_height - PADDING + input_height / 2 - 25,
                50.0f, 50.0f
            ));

            attach_button.set_container(container(
                scene_width - PADDING * 4 - 110,
                scene_height - input_height - PADDING + input_height / 2 - 25,
                50.0f, 50.0f
            ));

            setup_text_styles();

            create_test_messages(chat_x);

            setup_typing_animation(chat_x);

            bg.setup();
            user_avatar.setup();
            chat_avatar.setup();

            f_list.initialize(friends_header.get_container().get_y() + 50, sidebar_width);
        }

        void setup_text_styles()
        {
            text_style default_style;

            default_style.text_size_magnification = 0.07F;
            default_style.text_color = colour(255, 255, 255, 240);
            default_style.outline_enable = false;
            default_style.shadow_enable = true;
            default_style.shadow_offset = {2.0F, 2.0F};
            default_style.shadow_color = colour(0, 0, 0, 100);

            username_text.initialize(
                "Alex Johnson",
                container(PADDING * 3 + 60, PADDING * 3 + 5),
                font_manager::get_font("Roboto-Medium"),
                default_style
            );

            text_style status_style = default_style;
            status_style.text_size_magnification = 0.05F;
            status_style.text_color = colour(180, 180, 180, 200);

            user_status_text.initialize(
                "Online",
                container(PADDING * 3 + 60, PADDING * 3 + 25),
                font_manager::get_font("Roboto-Regular"),
                status_style
            );

            text_style placeholder_style = status_style;
            placeholder_style.text_color = colour(160, 160, 160, 180);

            search_placeholder.initialize(
                "Search conversation...",
                container(search_bar.get_container().get_x() + 15, search_bar.get_container().get_y() + 12),
                font_manager::get_font("Roboto-Regular"),
                placeholder_style
            );

            text_style header_style = default_style;
            header_style.text_size_magnification = 0.06F;
            header_style.text_color = colour(200, 200, 200, 220);
            friends_title.initialize(
                "FRIENDS",
                container(friends_header.get_container().get_x() + 15, friends_header.get_container().get_y() + 8),
                font_manager::get_font("Roboto-Medium"),
                header_style
            );

            text_style chat_header_style = default_style;
            chat_header_style.text_size_magnification = 0.08F;
            chat_title.initialize(
                "Joshua hardstick",
                container(chat_header.get_container().get_x() + 60, chat_header.get_container().get_y() + 10),
                font_manager::get_font("Roboto-Medium"),
                chat_header_style
            );

            last_seen_text.initialize(
                "Last seen 5 minutes ago",
                container(chat_header.get_container().get_x() + 60, chat_header.get_container().get_y() + 35),
                font_manager::get_font("Roboto-Regular"),
                status_style
            );

            input_placeholder.initialize(
                "Type a message...",
                container(message_input.get_container().get_x() + 20, message_input.get_container().get_y() + 15),
                font_manager::get_font("Roboto-Regular"),
                placeholder_style
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

        void create_test_messages(float chat_x)
        {
            float chat_content_x = chat_x + PADDING * 2;
            float chat_content_width = scene_width - chat_x - PADDING * 4;
            float message_y = chat_header.get_container().get_y() + 90;

            std::vector<std::pair<std::string, bool>> messages = {
                {"Hey! How was your weekend?", false},
                {"It was great! Went hiking with friends. How about you?", true},
                {"Nice! I stayed home and binged Netflix", false}, //i need to add suport for emojis
                {"Haha, sometimes that's exactly what you need!", true},
                {"Absolutely! Any show recommendations?", false}
            };

            text_style message_style;
            message_style.text_size_magnification = 0.06F;
            message_style.text_color = colour(0, 0, 0, 255);
            message_style.shadow_enable = true;
            message_style.shadow_offset = {1.0F, 1.0F};
            message_style.shadow_color = colour(0, 0, 0, 0);

            for (int i = 0; i < messages.size(); ++i)
            {
                bool is_sent = messages[i].second;
                float bubble_width = std::min(400.0f, chat_content_width * 0.7f);
                float bubble_height = 45.0f;

                float bubble_x = is_sent
                                     ? chat_content_x + chat_content_width - bubble_width - 20
                                     : chat_content_x + 20;

                rectangle bubble(
                    is_sent ? accent_color : colour(255, 255, 255, 80),
                    is_sent ? colour(255, 255, 255, 100) : colour(255, 255, 255, 60),
                    1, 18.0f
                );
                bubble.set_container(container(bubble_x, message_y + i * 60, bubble_width, bubble_height));
                message_bubbles.push_back(bubble);

                message_style.text_color = is_sent ? colour(0, 0, 0) : colour(0, 0, 0);
                text msg_text;

                auto msg_text_ptr = std::make_unique<text>();
                msg_text_ptr->initialize(
                    messages[i].first,
                    container(bubble_x + 15, message_y + i * 60 + 12),
                    font_manager::get_font("Roboto-Regular"),
                    message_style
                );
                message_texts.push_back(std::move(msg_text_ptr));
            }
        }

        void setup_typing_animation(float chat_x)
        {
        }

        rectangle rect{colour(0, 0, 0, 255), colour(0, 0, 0, 0), 1, 0.0F};

        void render() override
        {
            blur_effect.setup();

            glClearColor(bg_gradient_start.get_red_direct(), bg_gradient_start.get_green_direct(),
                         bg_gradient_start.get_blue_direct(), 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            blur_effect.capture_and_blur(
                scene_width, scene_height,
                glass_tint,
                [this]
                {
                    bg.draw(scene_width, scene_height);
                },
                [this]
                {
                    sidebar_glass.draw();
                    chat_area_glass.draw();
                    input_area_glass.draw();
                },
                bg_gradient_start
            );

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            profile_section.draw();
            search_bar.draw();
            friends_header.draw();
            chat_header.draw();
            message_input.draw();
            send_button.draw();
            attach_button.draw();

            for (auto &bubble : message_bubbles)
            {
                bubble.draw();
            }

            user_avatar.draw();
            chat_avatar.draw();

            //  glDisable(GL_SCISSOR_TEST);

            username_text.render();
            user_status_text.render();
            search_placeholder.render();
            friends_title.render();
            chat_title.render();
            last_seen_text.render();
            input_placeholder.render();
            send_icon.render();
            attach_icon.render();

            f_list.draw(sidebar_glass.get_container().get_y(), scene_width, scene_height);

            for (auto &msg_text : message_texts)
            {
                if (msg_text)
                {
                    msg_text->render();
                }
            }
        }

        void update() override
        {
            typing_animation += 0.1f;

            if (typing_animation > 6.28f) typing_animation = 0.0f;

            static int frame_count = 0;
            frame_count++;

            if (frame_count % 300 == 0)
            {
                show_typing = !show_typing;
            }
        }

        void scroll(const zcg_scroll_event_t &scroll_event)
        {
            f_list.scroll(scroll_event);
        }

        void resize(int width, int height) override
        {
            scene_width = width;
            scene_height = height;
            bg.reshape(width, height);
            blur_effect.reshape(width, height);
        }

        void destroy() override
        {
        }
    };
}
