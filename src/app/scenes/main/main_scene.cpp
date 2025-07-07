//
// Created by Damian Netter on 07/07/2025.
//

#include "ZCApp/app/scenes/main/main_scene.hpp"

namespace zc_app
{
    void main_scene::initialize(const int scene_width, const int scene_height)
    {
        this->scene_width = scene_width;
        this->scene_height = scene_height;

        const float sidebar_width = std::min(320.0F, static_cast<float>(scene_width) * 0.25F);
        auto [width, height] = perspective_util::get_effective_virtual_dimensions();

        const container sidebar_container(PADDING, PADDING, sidebar_width - PADDING, static_cast<float>(scene_height) - PADDING * 2.0F);
        const container profile_container(PADDING * 2.0F, PADDING * 2.0F, sidebar_width - PADDING * 4.0F, 70.0F);
        const container avatar_container(profile_container.get_x() + PADDING * 2.0F, profile_container.get_y() + (profile_container.get_height() - PADDING) / 2.0F, 40.0F, 40.0F);
        const container search_container(PADDING * 3.0F, profile_container.get_y() + 100.0f, sidebar_width - PADDING * 6.0f, 1.0F);
        const container connections_container(PADDING * 2.0F, search_container.get_y() + PADDING, sidebar_width - PADDING * 4.0f, 30.0F);

        const float chat_x = sidebar_width + PADDING;

        blur_background.setup(static_cast<int>(width), static_cast<int>(height));

        {
            sidebar_glass.set_container(sidebar_container);
            profile_section.set_container(profile_container);
            user_avatar_texture.set_container(avatar_container);
            separator_bar.set_container(search_container);
            connection_header.set_container(connections_container);
        }

        {
            setup_sidebar_text_styles();

            chat_area_widget.initialize(chat_x, &app_data_manager);
            connection_modal_widget.initialize(scene_width, scene_height);
            request_modal_widget.initialize(scene_width, scene_height);

            effect_background.setup();
            user_avatar_texture.setup();

            connections_list.initialize(connection_header.get_container().get_y() + 50, sidebar_width, sidebar_glass.get_container().get_height());
        }

        connections_list.set_on_friend_selected_callback([this](const std::string &target_username)
        {
            if (is_awaiting_relay_response || active_chat_partner == target_username)
            {
                return;
            }

            is_awaiting_relay_response = true;
            active_chat_partner = target_username;

            zc_kit::bridge::client_request_relay(target_username);
        });

        setup_bridge_callbacks();

        connection_modal_widget.set_on_connect_callback(
            []([[maybe_unused]] const std::string &username, const std::string &ip, const std::string &port)
            {
                zc_kit::bridge::client_connect(ip, std::stoi(port));
            }
        );


        request_modal_widget.set_on_answered_callback([](const bool accepted)
        {
            zc_kit::bridge::client_answer_relay(accepted);
        });

        if (!is_identified)
        {
            connection_modal_widget.set_visible(true);
        }
    }

    void main_scene::setup_sidebar_text_styles()
    {
        const std::string username_text_str = "Not Connected";
        const std::string user_status_text_str = "Offline";
        const std::string friends_title_str = "Connections";

        const auto username_text_str_length = static_cast<float>(username_text_str.length());
        const auto friends_title_str_length = static_cast<float>(friends_title_str.length());

        const auto roboto_regular = font_manager::get_font("Roboto-Regular");
        const auto roboto_medium = font_manager::get_font("Roboto-Medium");

        constexpr float font_default_size = 128.0F;

        text_style username_style;
        username_style.text_size_magnification = 0.07F;
        username_style.text_color = colour(20, 20, 20, 255);
        username_style.outline_enable = false;
        username_style.shadow_enable = true;

        username_style.shadow_offset = {2.0F, 2.0F};
        username_style.shadow_color = colour(0, 0, 0, 100);

        text_style status_style = username_style;
        status_style.text_size_magnification = 0.05F;
        status_style.text_color = colour(20, 20, 20, 200);

        text_style header_style = username_style;
        header_style.text_size_magnification = 0.09F;
        header_style.text_color = colour(20, 20, 20, 180);

        const float username_text_width = font_default_size * username_style.text_size_magnification *
            username_text_str_length / 2.0F;
        const float friends_title_width = font_default_size * friends_title_str_length * header_style.
            text_size_magnification / 2.0F;

        const float username_text_x = profile_section.get_container().get_x() + username_text_width;
        const float username_text_y = profile_section.get_container().get_y() + profile_section.get_container().get_height()
            / 2.0F - PADDING;

        container username_text_container(username_text_x, username_text_y);
        username_text.initialize(
            username_text_str,
            username_text_container,
            roboto_medium,
            username_style
        );

        container user_status_text_container(username_text_x, username_text_y + PADDING);
        user_status_text.initialize(
            user_status_text_str,
            user_status_text_container,
            roboto_regular,
            status_style
        );

        container connections_container(connection_header.get_container().get_x() + friends_title_width, connection_header.get_container().get_y() + (connection_header.get_container().get_height() - PADDING) / 2.0F);
        friends_title.initialize(
            friends_title_str,
            connections_container,
            roboto_medium,
            header_style
        );
    }

    void main_scene::setup_bridge_callbacks()
    {
        zc_kit::bridge::on_ready_for_identification = [this]
        {
            my_username = connection_modal_widget.get_username();

            if (my_username.empty())
            {
                my_username = "Guest" + std::to_string(random() % 100);
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
                    std::vector<connection_data> connections;

                    for (auto json = nlohmann::json::parse(json_payload); const auto &user_obj : json)
                    {
                        if (user_obj.value("name", "") == my_username)
                        {
                            continue;
                        }

                        connections.push_back({
                            .name = user_obj.value("name", "Unknown"),
                            .status = user_obj.value("status", 0)
                        });
                    }

                    connections_list.populate_friends(connections);
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

    void main_scene::render()
    {
        blur_partial.setup();

        glClearColor(bg_gradient_start.get_red_direct(), bg_gradient_start.get_green_direct(), bg_gradient_start.get_blue_direct(), 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        blur_partial.capture_and_blur(
            scene_width, scene_height,
            glass_tint,
            [this]
            {
                effect_background.draw(scene_width, scene_height);
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

    void main_scene::draw_components()
    {
        profile_section.draw();
        separator_bar.draw();
        connection_header.draw();

        user_avatar_texture.draw();

        username_text.render();
        user_status_text.render();
        search_placeholder.render();
        friends_title.render();

        connections_list.draw(sidebar_glass.get_container().get_y());

        chat_area_widget.draw();
    }

    void main_scene::resize(const int width, const int height)
    {
        scene_width = width;
        scene_height = height;
        effect_background.reshape(width, height);
        blur_background.reshape(width, height);
        blur_partial.reshape(width, height);
    }

    void main_scene::update()
    {
        process_main_thread_actions();
        connections_list.update(time_util::get_delta_time());
        connection_modal_widget.update(time_util::get_delta_time());
    }

    void main_scene::destroy()
    {
    }

    void main_scene::process_main_thread_actions()
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

    void main_scene::queue_main_thread_action(std::function<void()> action)
    {
        std::lock_guard lock(main_thread_action_mutex);
        main_thread_actions.push_back(std::move(action));
    }

    void main_scene::on_char_typed(unsigned int char_code)
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

    void main_scene::on_mouse_down(const zcg_mouse_pos_t mouse_pos, const int button)
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
            connections_list.on_mouse_down(mouse_pos, button);

            if (button == ZCG_MOUSE_BUTTON_LEFT && is_in_area(user_avatar_texture.get_container(), mouse_pos))
            {
                connection_modal_widget.set_visible(true);
            }
        }
    }

    void main_scene::on_mouse_up(const zcg_mouse_pos_t mouse_pos, const int button)
    {
        if (connection_modal_widget.is_modal_visible() || request_modal_widget.is_modal_visible())
        {
        }
        else
        {
            chat_area_widget.on_mouse_up(mouse_pos, button);
        }
    }

    void main_scene::on_key_down(const zcg_key_event_t key_event)
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

    void main_scene::scroll(const zcg_scroll_event_t &scroll_event)
    {
        if (connection_modal_widget.is_modal_visible() || request_modal_widget.is_modal_visible())
        {
        }
        else
        {
            connections_list.scroll(scroll_event);
            chat_area_widget.on_scroll(scroll_event);
        }
    }

    void main_scene::on_mouse_move(const zcg_mouse_pos_t &mouse_pos)
    {
        if (connection_modal_widget.is_modal_visible() || request_modal_widget.is_modal_visible())
        {
            if (connection_modal_widget.is_modal_visible())
                connection_modal_widget.on_mouse_move(mouse_pos);
        }
        else
        {
            chat_area_widget.on_mouse_move(mouse_pos);
            connections_list.on_mouse_move(mouse_pos);
        }
    }
}
