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
        partial_blur blur_effect{};
        background bg{};

        rectangle sidebar_glass{glass_tint, glass_border, 1, BORDER_RADIUS};
        rectangle profile_section{glass_tint, glass_border, 1, BORDER_RADIUS};
        rectangle search_bar{glass_tint, colour(255, 255, 255, 255), 1, 20.0F};
        rectangle friends_header{glass_tint, colour(255, 255, 255, 255), 1, 12.0F};

        fan_texture user_avatar{"avatar.png", 2.0F, 128};

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

        data::data_manager app_data_manager;

        friend_list f_list;
        chat_area chat_area_widget;
        connection_modal connection_modal_widget{};
        request_modal request_modal_widget{};

        std::mutex main_thread_action_mutex;
        std::vector<std::function<void()>> main_thread_actions;

        void queue_main_thread_action(std::function<void()> action)
        {
            std::lock_guard lock(main_thread_action_mutex);
            main_thread_actions.push_back(std::move(action));
        }

        void process_main_thread_actions()
        {
            std::vector<std::function<void()>> actions_to_run;
            {
                std::lock_guard lock(main_thread_action_mutex);
                if (main_thread_actions.empty())
                {
                    return;
                }
                actions_to_run.swap(main_thread_actions);
            }

            for (const auto &action : actions_to_run)
            {
                action();
            }
        }

        void setup_bridge_callbacks()
        {
            zc_kit::bridge::on_ready_for_identification = [this]()
            {
                my_username = connection_modal_widget.get_username();
                if (my_username.empty())
                {
                    my_username = "Guest" + std::to_string(rand() % 1000);
                }

                queue_main_thread_action([this]
                {
                    username_text.set_text(my_username);
                    user_status_text.set_text("Online");
                });

                zc_kit::bridge::client_identify(my_username);
            };

            zc_kit::bridge::on_identification_result = [this](bool success, const std::string &reason)
            {
                queue_main_thread_action([this, success, reason]
                {
                    if (success)
                    {
                        is_identified = true;
                        connection_modal_widget.on_connection_result(true, "Identification successful!");
                        zc_kit::bridge::client_get_users();
                    }
                    else
                    {
                        is_identified = false;
                        connection_modal_widget.on_connection_result(false, reason);

                        zc_kit::bridge::client_disconnect();
                    }
                });
            };

            zc_kit::bridge::on_user_list_received = [this](const std::string &json_payload)
            {
                queue_main_thread_action([this, json_payload]
                {
                    try
                    {
                        std::vector<friend_data> friends;
                        auto json = nlohmann::json::parse(json_payload);
                        for (const auto &user_obj : json)
                        {
                            if (user_obj.value("name", "") == my_username)
                            {
                                continue;
                            }

                            friends.push_back({
                                .name = user_obj.value("name", "Unknown"),
                                .status = user_obj.value("status", 0)
                            });
                        }

                        f_list.populate_friends(friends);
                    }
                    catch (const nlohmann::json::parse_error &e)
                    {
                        std::cerr << "JSON parse error: " << e.what() << std::endl;
                    }
                });
            };

            zc_kit::bridge::on_incoming_relay_request = [this](const std::string &sender_name)
            {
                active_chat_partner = sender_name;
                queue_main_thread_action([this, sender_name]
                {
                    request_modal_widget.show(sender_name);
                });
            };

            zc_kit::bridge::on_relay_established = [this]()
            {
                is_awaiting_relay_response = false;
                queue_main_thread_action([this]
                {
                    chat_area_widget.switch_conversation(active_chat_partner);
                });
            };

            zc_kit::bridge::on_relay_refused = [this]()
            {
                is_awaiting_relay_response = false;
                active_chat_partner.clear();
                std::cout << "Relay request was refused." << std::endl;
            };

            zc_kit::bridge::on_chat_message_received = [this](const std::string &message)
            {
                queue_main_thread_action([this, message]
                {
                    if (chat_area_widget.get_current_conversation_id() == active_chat_partner)
                    {
                        chat_area_widget.add_message(message, false);
                    }
                });
            };

            zc_kit::bridge::on_relay_terminated = [this](const std::string &reason)
            {
                queue_main_thread_action([this]
                {
                    if (!active_chat_partner.empty())
                    {
                        chat_area_widget.clear_chat("'" + active_chat_partner + "' has left the chat.");
                        active_chat_partner.clear();
                        zc_kit::bridge::client_get_users();
                    }
                });
            };

            zc_kit::bridge::on_status_received = [this](int status_code)
            {
                std::cout << "Received status update: " << status_code << std::endl;
            };
        }

    public:
        void initialize(const int scene_width, const int scene_height)
        {
            profile_section.set_colour(colour(glass_tint, 60));
            friends_header.set_colour(colour(glass_tint, 60));
            search_bar.set_colour(colour(glass_tint, 60));

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
                70.0f
            ));

            user_avatar.set_container(container(
                profile_section.get_container().get_x() + PADDING * 2,
                profile_section.get_container().get_y() + (profile_section.get_container().get_height() - PADDING) / 2.0f,
                40.0f, 40.0f
            ));

            search_bar.set_container(container(
                PADDING * 3, profile_section.get_container().get_y() + 100,
                sidebar_width - PADDING * 6,
                1
            ));

            friends_header.set_container(container(
                PADDING * 2, search_bar.get_container().get_y() + PADDING ,
                sidebar_width - PADDING * 4,
                30.0f
            ));

            float chat_x = sidebar_width + PADDING;

            chat_area_widget.initialize(chat_x, &app_data_manager);
            connection_modal_widget.initialize(scene_width, scene_height);
            request_modal_widget.initialize(scene_width, scene_height);

            setup_sidebar_text_styles();

            bg.setup();
            user_avatar.setup();

            f_list.initialize(friends_header.get_container().get_y() + 50, sidebar_width,
                              sidebar_glass.get_container().get_height());

            f_list.set_on_friend_selected_callback([this](const std::string &target_username)
            {
                if (is_awaiting_relay_response || active_chat_partner == target_username) return;

                is_awaiting_relay_response = true;
                active_chat_partner = target_username;
                zc_kit::bridge::client_request_relay(target_username);
            });

            setup_bridge_callbacks();

            connection_modal_widget.set_on_connect_callback(
                [](const std::string &username, const std::string &ip, const std::string &port)
                {
                    zc_kit::bridge::client_connect(ip, std::stoi(port));
                }
            );


            request_modal_widget.set_on_answered_callback([](bool accepted)
            {
                zc_kit::bridge::client_answer_relay(accepted);
            });

            if (!is_identified)
            {
                connection_modal_widget.set_visible(true);
            }
        }

        void setup_sidebar_text_styles()
        {
            text_style default_style;
            default_style.text_size_magnification = 0.07F;
            default_style.text_color = colour(20, 20, 20, 255);
            default_style.outline_enable = false;
            default_style.shadow_enable = true;

            default_style.shadow_offset = {2.0F, 2.0F};
            default_style.shadow_color = colour(0, 0, 0, 100);

            const std::string username_text_str = "Not Connected";
            const float username_text_width = 128.0f * default_style.text_size_magnification * username_text_str.length() / 2.0f;
            const float username_text_x = profile_section.get_container().get_x() + username_text_width;
            const float username_text_y = profile_section.get_container().get_y() + profile_section.get_container().get_height() / 2.0f - PADDING;

            username_text.initialize(
                username_text_str,
                container(username_text_x, username_text_y),
                font_manager::get_font("Roboto-Medium"),
                default_style
            );

            text_style status_style = default_style;
            status_style.text_size_magnification = 0.05F;
            status_style.text_color = colour(20, 20, 20, 200);

            user_status_text.initialize(
                "Offline",
                container(username_text_x, username_text_y + PADDING),
                font_manager::get_font("Roboto-Regular"),
                status_style
            );

            text_style placeholder_style = status_style;
            placeholder_style.text_color = colour(20, 20, 20, 180);

            text_style header_style = default_style;
            header_style.text_size_magnification = 0.09F;
            header_style.text_color = colour(20, 20, 20, 180);

            const std::string friends_title_str = "Connections";

            float friends_title_width = (friends_title_str.length() * 128.0f * header_style.text_size_magnification) / 2.0f;

            friends_title.initialize(
                friends_title_str,
                container(friends_header.get_container().get_x() + friends_title_width,
                          friends_header.get_container().get_y() + (friends_header.get_container().get_height() - PADDING) / 2.0f),
                font_manager::get_font("Roboto-Medium"),
                header_style
            );
        }

        void on_char_typed(unsigned int char_code)
        {
            if (connection_modal_widget.is_modal_visible() || request_modal_widget.is_modal_visible())
            {
                if (connection_modal_widget.is_modal_visible())
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
            else if (request_modal_widget.is_modal_visible())
            {
                request_modal_widget.on_mouse_down(mouse_pos, button);
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
            if (connection_modal_widget.is_modal_visible() || request_modal_widget.is_modal_visible())
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
            else if (request_modal_widget.is_modal_visible())
            {
            }
            else if (!is_identified && key_event.key_code == ZCG_KEY_ESCAPE)
            {
                connection_modal_widget.set_visible(true);
            }
            else
            {
                chat_area_widget.on_key_down(key_event);
            }
        }

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
                    chat_area_widget.draw_glassy();
                },
                bg_gradient_start
            );

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            draw_components();

            connection_modal_widget.draw();
            request_modal_widget.draw();
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
            process_main_thread_actions();
            f_list.update(time_util::get_delta_time());
            chat_area_widget.update();
            connection_modal_widget.update(time_util::get_delta_time());
        }

        void scroll(const zcg_scroll_event_t &scroll_event)
        {
            if (connection_modal_widget.is_modal_visible() || request_modal_widget.is_modal_visible())
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
            if (connection_modal_widget.is_modal_visible() || request_modal_widget.is_modal_visible())
            {
                if (connection_modal_widget.is_modal_visible())
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
