#include "ZCKit/bridge.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>
#include <OpenGL/gl3.h>

#include "ZCApp/app/app_runner.hpp"
#include "ZCApp/graphics/fonts/font_loader.hpp"
#include "ZCApp/graphics/shaders/shaders.hpp"
#include "ZCApp/graphics/textures/texture_loader.hpp"
#include "ZCApp/graphics/utils/string_util.hpp"
#include "ZCKit/internal/base64.hpp"

namespace zc_kit
{
    static std::map<std::string, std::unique_ptr<std::vector<char>>> texture_data_buffers;

    const std::unordered_map<std::string, bridge::events> bridge::events_map =
    {
        {"PUSH_SHADER", EV_PUSH_SHADER},
        {"PUSH_TEXTURE", EV_PUSH_TEXTURE},
        {"PUSH_FONT", EV_PUSH_FONT},
        {"FINISH_LOADING", EV_FINISH_LOADING},
        {"DAEMON_READY", EV_DAEMON_READY},
        {"READY_FOR_IDENTIFY", EV_READY_FOR_IDENTIFY},
        {"IDENTIFY_SUCCESS", EV_IDENTIFY_SUCCESS},
        {"IDENTIFY_FAIL", EV_IDENTIFY_FAIL},
        {"INCOMING_RELAY_REQUEST", EV_INCOMING_RELAY_REQUEST},
        {"RELAY_ESTABLISHED", EV_RELAY_ESTABLISHED},
        {"RELAY_REFUSED", EV_RELAY_REFUSED},
        {"RELAY_TERMINATED", EV_RELAY_TERMINATED},
        {"INCOMING_CHAT", EV_INCOMING_CHAT},
        {"INCOMING_STATUS", EV_INCOMING_STATUS},
        {"INCOMING_DESCRIPTION", EV_INCOMING_DESCRIPTION},
        {"PUSH_USER_LIST", EV_PUSH_USER_LIST},
        {"CONNECTION_FAILED", EV_CONNECTION_FAILED},
        {"DISCONNECTED", EV_DISCONNECTED}
    };

    std::unique_ptr<std::thread> bridge::ipc_read_thread = nullptr;
    std::unique_ptr<zcg_kit::client> bridge::daemon = nullptr;
    std::atomic_bool bridge::shutdown_requested = false;
    std::atomic_bool bridge::is_running = false;

    std::function<void(bool, const std::string &)> bridge::on_identification_result;
    std::function<void(const std::string &)> bridge::on_incoming_relay_request;
    std::function<void(const std::string &)> bridge::on_chat_message_received;
    std::function<void(const std::string &)> bridge::on_description_received;
    std::function<void(const std::string &)> bridge::on_relay_terminated;
    std::function<void()> bridge::on_ready_for_identification;
    std::function<void(int)> bridge::on_status_received;
    std::function<void()> bridge::on_relay_established;
    std::function<void()> bridge::on_relay_refused;
    std::function<void(const std::string &)> bridge::on_user_list_received;
    std::function<void(const std::string &)> bridge::on_connection_failed;
    std::function<void()> bridge::on_disconnected;

    void bridge::_loop()
    {
        while (!shutdown_requested)
        {
            try
            {
                std::string message = daemon->read();

                if (message.empty())
                {
                    break;
                }

                message = zc_app::string_util::trim(message);

                size_t pos = message.find(':');

                std::string cmd = (pos == std::string::npos) ? message : message.substr(0, pos);
                std::string payload = (pos == std::string::npos) ? "" : message.substr(pos + 1);

                auto it = events_map.find(cmd);

                if (it == events_map.end())
                {
                    std::cerr << "[Bridge] Unknown command received: " << cmd << " (Payload: " << payload << ")" <<
                        std::endl;

                    continue;
                }

                switch (it->second)
                {
                case EV_PUSH_SHADER:
                    {
                        if (const auto first_colon = payload.find(':'); first_colon != std::string::npos)
                        {
                            std::string shader_name = payload.substr(0, first_colon);
                            std::string encoded_data = payload.substr(first_colon + 1);
                            std::string shader_data = base64_decode(encoded_data);

                            zc_app::shaders::manager::add_shader(shader_name, shader_data);
                        }

                        break;
                    }
                case EV_PUSH_TEXTURE:
                    {
                        const auto p1 = payload.find(':');
                        const auto p2 = payload.find(':', p1 + 1);
                        const auto p3 = payload.find(':', p2 + 1);

                        if (p3 != std::string::npos)
                        {
                            std::string name = payload.substr(0, p1);
                            std::string data = base64_decode(payload.substr(p1 + 1, p2 - (p1 + 1)));

                            int w = std::stoi(payload.substr(p2 + 1, p3 - (p2 + 1)));
                            int h = std::stoi(payload.substr(p3 + 1));

                            auto buf = std::make_unique<std::vector<char>>(data.begin(), data.end());

                            texture_data_buffers[name] = std::move(buf);

                            zc_app::texture_loader::push(name, {texture_data_buffers[name]->data(), w, h});
                        }

                        break;
                    }
                case EV_PUSH_FONT:
                    {
                        size_t colon = payload.find(':');

                        if (colon != std::string::npos)
                        {
                            std::string font_name = payload.substr(0, colon);
                            std::string data = base64_decode(payload.substr(colon + 1));

                            zc_app::font_loader::push_font(font_name, {data.begin(), data.end()});
                        }

                        break;
                    }
                case EV_FINISH_LOADING:
                    {
                        std::lock_guard lock(app_runner::mtx);

                        app_runner::ready = true;
                        app_runner::condition.notify_all();

                        break;
                    }
                case EV_DAEMON_READY:
                    {
                        internal_request_resources();

                        break;
                    }
                case EV_READY_FOR_IDENTIFY:
                    {
                        if (on_ready_for_identification)
                        {
                            on_ready_for_identification();
                        }

                        break;
                    }
                case EV_IDENTIFY_SUCCESS:
                    {
                        if (on_identification_result)
                        {
                            on_identification_result(true, "");
                        }

                        break;
                    }
                case EV_IDENTIFY_FAIL:
                    {
                        if (on_identification_result)
                        {
                            on_identification_result(false, payload);
                        }

                        break;
                    }
                case EV_INCOMING_RELAY_REQUEST:
                    {
                        if (on_incoming_relay_request)
                        {
                            on_incoming_relay_request(payload);
                        }

                        break;
                    }

                case EV_RELAY_ESTABLISHED:
                    {
                        if (on_relay_established)
                        {
                            on_relay_established();
                        }

                        break;
                    }

                case EV_RELAY_REFUSED:
                    {
                        if (on_relay_refused)
                        {
                            on_relay_refused();
                        }

                        break;
                    }

                case EV_RELAY_TERMINATED:
                    {
                        if (on_relay_terminated)
                        {
                            on_relay_terminated(payload);
                        }

                        break;
                    }

                case EV_INCOMING_CHAT:
                    {
                        if (on_chat_message_received)
                        {
                            on_chat_message_received(payload);
                        }

                        break;
                    }

                case EV_INCOMING_STATUS:
                    {
                        if (on_status_received)
                        {
                            on_status_received(std::stoi(payload));
                        }

                        break;
                    }

                case EV_INCOMING_DESCRIPTION:
                    {
                        if (on_description_received)
                        {
                            on_description_received(payload);
                        }

                        break;
                    }

                case EV_PUSH_USER_LIST:
                    {
                        if (on_user_list_received)
                        {
                            on_user_list_received(payload);
                        }
                        break;
                    }

                case EV_CONNECTION_FAILED:
                    {
                        if (on_connection_failed)
                        {
                            on_connection_failed(payload);
                        }
                        break;
                    }

                case EV_DISCONNECTED:
                    {
                        if (on_disconnected)
                        {
                            on_disconnected();
                        }
                        break;
                    }

                default:
                    {
                        std::cerr << "[Bridge] Unknown command received: " << cmd << " (Payload: " << payload << ")" <<
                            std::endl;
                    }
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "[Bridge] FATAL Exception in IPC listener: " << e.what() << std::endl;
                break;
            }
        }
    }

    void bridge::internal_initialize(const std::string &path)
    {
        if (is_running)
        {
            std::cout << "[Bridge] Bridge already running, skipping initialization." << std::endl;
            return;
        }

        shutdown_requested = false;
        daemon = std::make_unique<zcg_kit::client>();

        if (!daemon->start(std::filesystem::absolute(path)))
        {
            throw std::runtime_error("Failed to start Java daemon.");
        }

        is_running = true;

        ipc_read_thread = std::make_unique<std::thread>(_loop);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    void bridge::internal_terminate()
    {
        if (shutdown_requested.exchange(true))
        {
            return;
        }

        if (daemon && daemon->is_connected())
        {
            daemon->send_message(IPC_SHUTDOWN);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (daemon)
        {
            daemon->disconnect();
        }

        if (ipc_read_thread && ipc_read_thread->joinable())
        {
            ipc_read_thread->join();
        }

        std::cout << "[Bridge] Bridge terminated successfully." << std::endl;

        fflush(stdout);

        exit(0);
    }

    void bridge::internal_request_resources()
    {
        if (daemon && daemon->is_connected())
        {
            daemon->send_message(IPC_REQUEST_RESOURCES);
        }
        else
        {
            std::cout << "[Bridge] WARNING: Cannot request resources - daemon not connected." << std::endl;
        }
    }

    void bridge::client_connect(const std::string &ip, const int port)
    {
        if (daemon && daemon->is_connected())
        {
            daemon->send_message("CONNECT_NETTY:" + ip + ":" + std::to_string(port));
        }
    }

    void bridge::client_disconnect()
    {
        if (daemon && daemon->is_connected())
        {
            daemon->send_message(IPC_DISCONNECT);
        }
        else
        {
            std::cerr << "[C++-DEBUG] WARNING: Cannot send DISCONNECT command - daemon not connected or null." <<
                std::endl;
        }
    }

    void bridge::client_identify(const std::string &name)
    {
        if (daemon && daemon->is_connected())
        {
            daemon->send_message("IDENTIFY:" + name);
        }
        else
        {
            std::cerr << "[C++-DEBUG] WARNING: Cannot send IDENTIFY command - daemon not connected or null." <<
                std::endl;
        }
    }

    void bridge::client_get_users()
    {
        if (daemon && daemon->is_connected())
        {
            daemon->send_message(IPC_GET_USERS);
        }
    }

    void bridge::client_request_relay(const std::string &target_name)
    {
        if (daemon && daemon->is_connected())
        {
            daemon->send_message(IPC_REQUEST_RELAY + target_name);
        }
    }

    void bridge::client_answer_relay(const bool accept)
    {
        if (daemon && daemon->is_connected())
            daemon->send_message(accept ? IPC_ANSWER_RELAY_ACCEPT : IPC_ANSWER_RELAY_REFUSE);
    }

    void bridge::client_message(const std::string &msg)
    {
        if (daemon && daemon->is_connected())
            daemon->send_message(IPC_SEND_CHAT + msg);
    }

    void bridge::client_status(const int code)
    {
        if (daemon && daemon->is_connected())
        {
            daemon->send_message(IPC_SEND_STATUS + std::to_string(code));
        }
    }

    void bridge::client_description(const std::string &description)
    {
        if (daemon && daemon->is_connected())
        {
            daemon->send_message(IPC_SEND_DESCRIPTION + description);
        }
    }
}
