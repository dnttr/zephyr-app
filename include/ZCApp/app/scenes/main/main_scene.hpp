#pragma once

#include "widgets/friend_button.hpp"
#include "widgets/friend_list.hpp"
#include "widgets/input_message_button.hpp"
#include "widgets/chat_area.hpp"
#include "widgets/connection_modal.hpp"
#include "ZCApp/app/scenes/apperance.hpp"
#include "ZCApp/graphics/effects/partial_blur.hpp"
#include "ZCApp/graphics/objects/background.hpp"
#include "ZCApp/graphics/objects/shapes/rectangle.hpp"
#include "ZCApp/graphics/objects/text/text.hpp"
#include "ZCApp/graphics/scene/scene.hpp"
#include "ZCApp/graphics/textures/fan_texture.hpp"
#include "ZCApp/graphics/textures/texture.hpp"
#include "ZCApp/graphics/effects/fullscreen_blur.hpp"

namespace zc_app
{
    class main_scene final : public scene
    {
        fullscreen_blur blur_background{};
        partial_blur blur_effect{};
        background bg{};

        rectangle sidebar_glass{glass_tint, glass_border, 1, BORDER_RADIUS};
        rectangle profile_section{dark_glass, glass_border, 1, BORDER_RADIUS};
        rectangle search_bar{colour(255, 255, 255, 20), colour(255, 255, 255, 60), 1, 20.0F};
        rectangle friends_header{dark_glass, colour(255, 255, 255, 60), 1, 8.0F};

        fan_texture user_avatar{"avatar.png", 2.0F, 128};

        text search_placeholder, search_icon;
        text username_text, user_status_text;
        text friends_title;

        int scene_width = 0;
        int scene_height = 0;

        data::data_manager app_data_manager;

        friend_list f_list;
        chat_area chat_area_widget;
        connection_modal connection_modal_widget{};

    public:
        void initialize(const int scene_width, const int scene_height)
        {
            auto [width, height] = perspective_util::get_effective_virtual_dimensions();
            blur_background.setup(width, height);

            this->scene_width = scene_width;
            this->scene_height = scene_height;

            float sidebar_width = std::min(320.0f, static_cast<float>(scene_width) * 0.25f);

            sidebar_glass.set_container(container(
                PADDING, PADDING,
                sidebar_width - PADDING,
                static_cast<float>(scene_height) - PADDING * 2
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

            chat_area_widget.initialize(chat_x, &app_data_manager);
            connection_modal_widget.initialize(960, 540);

            setup_sidebar_text_styles();

            bg.setup();
            user_avatar.setup();

            f_list.initialize(friends_header.get_container().get_y() + 50, sidebar_width,
                              sidebar_glass.get_container().get_height(), &chat_area_widget);

            connection_modal_widget.set_on_connect_callback(
                [this](const std::string &username, const std::string &ip, const std::string &port)
                {
                    bool success = app_data_manager.attempt_connection(ip, port);
                    if (success) {
                        connection_modal_widget.on_connection_result(true, "Connected!");
                        chat_area_widget.add_message("Successfully connected to " + ip, false, false);
                    } else {
                        connection_modal_widget.on_connection_result(false, "Connection failed. Try again.");
                    }
                }
            );

            if (!app_data_manager.is_client_connected) {
                connection_modal_widget.set_visible(true);
            }
        }

        void setup_sidebar_text_styles()
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
        }

        void on_char_typed(unsigned int char_code)
        {
            if (connection_modal_widget.is_modal_visible())
            {
                connection_modal_widget.on_char_typed(char_code);
            }
            else
            {
                chat_area_widget.on_char_typed(char_code);
            }
        }

        void on_mouse_down(const zcg_mouse_pos_t mouse_pos, const int button)
        {
            if (connection_modal_widget.is_modal_visible())
            {
                connection_modal_widget.on_mouse_down(mouse_pos, button);
            }
            else
            {
                chat_area_widget.on_mouse_down(mouse_pos, button);
                f_list.on_mouse_down(mouse_pos, button);

                if (button == ZCG_MOUSE_BUTTON_LEFT && zc_app::is_in_area(user_avatar.get_container(), mouse_pos))
                {
                    connection_modal_widget.set_visible(true);
                }
            }
        }

        void on_key_up(zcg_key_event_t key_event)
        {
        }

        void on_mouse_up(const zcg_mouse_pos_t mouse_pos, const int button)
        {
            if (connection_modal_widget.is_modal_visible())
            {
            }
            else
            {
                chat_area_widget.on_mouse_up(mouse_pos, button);
            }
        }

        void on_key_down(const zcg_key_event_t key_event)
        {
            if (connection_modal_widget.is_modal_visible())
            {
                connection_modal_widget.on_key_down(key_event);
            }
            else if (!app_data_manager.is_client_connected && key_event.key_code == ZCG_KEY_ESCAPE) {
                connection_modal_widget.set_visible(true);
            }
            else
            {
                chat_area_widget.on_key_down(key_event);
            }
        }

        void render()
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
                    chat_area_widget.draw_glassy();
                },
                bg_gradient_start
            );

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            if (connection_modal_widget.is_modal_visible())
            {
                blur_background.draw(scene_width, scene_height, [this]
                {
                    draw_components();
                });
            }
            else
            {
                draw_components();
            }

            connection_modal_widget.draw();
        }

        void draw_components()
        {
            profile_section.draw();
            search_bar.draw();
            friends_header.draw();

            user_avatar.draw();

            username_text.render();
            user_status_text.render();
            search_placeholder.render();
            friends_title.render();

            f_list.draw(sidebar_glass.get_container().get_y());

            chat_area_widget.draw();
        }

        void update()
        {
            f_list.update(time_util::get_delta_time());
            chat_area_widget.update();
        }

        void scroll(const zcg_scroll_event_t &scroll_event)
        {
            if (connection_modal_widget.is_modal_visible())
            {
            }
            else
            {
                f_list.scroll(scroll_event);
                chat_area_widget.on_scroll(scroll_event);
            }
        }

        void on_mouse_move(const zcg_mouse_pos_t &mouse_pos)
        {
            if (connection_modal_widget.is_modal_visible())
            {
                connection_modal_widget.on_mouse_move(mouse_pos);
            }
            else
            {
                chat_area_widget.on_mouse_move(mouse_pos);
                f_list.on_mouse_move(mouse_pos);
            }
        }

        void resize(const int width, const int height)
        {
            scene_width = width;
            scene_height = height;
            bg.reshape(width, height);
            blur_background.reshape(width, height);
            blur_effect.reshape(width, height);
        }

        void destroy()
        {
        }

        void show_connection_modal()
        {
            connection_modal_widget.set_visible(true);
        }

        void hide_connection_modal()
        {
            connection_modal_widget.set_visible(false);
        }
    };
}