//
// Created by Damian Netter on 20/06/2025.
//

#include "ZCKit/bridge.hpp"

#include <algorithm>
#include <atomic>
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
    std::map<std::string, std::unique_ptr<std::vector<char>>> bridge::texture_data_buffers;

    std::unique_ptr<zcg_kit::client> bridge::daemon = nullptr;
    std::unique_ptr<std::thread> bridge::ipc_read_thread = nullptr;

    std::atomic_bool bridge::is_running = false;

    void bridge::initialize_bridge(const std::string& daemon_jar_path) {
        daemon = std::make_unique<zcg_kit::client>();

        if (!daemon->start(daemon_jar_path)) {
            std::cerr << "Error: Failed to start Java daemon process at " << daemon_jar_path << std::endl;

            throw std::runtime_error("Failed to start Java daemon process.");
        }

        is_running = true;
        ipc_read_thread = std::make_unique<std::thread>(ipc_listener_loop);

        std::cout << "Bridge: Java daemon process started." << std::endl;
    }

    void bridge::ipc_listener_loop() {
        while (is_running) {
            if (daemon && daemon->is_connected()) {
                std::string message = daemon->read();

                if (message.empty()) {
                    if (daemon->is_connected())
                    {
                        continue;
                    }

                    break;
                }

                message = zc_app::string_util::trim(message);

                if (message.rfind("DAEMON_READY", 0) == 0) {
                    //bruh x2, todo: add some logic here,
                } else if (message.rfind("CONNECT_SUCCESS", 0) == 0) {
                    //bruh x2, todo: add some logic here,
                } else if (message.rfind("CONNECT_FAIL:", 0) == 0) {
                    std::cerr << "Bridge ERROR: Daemon Netty connection failed: " << message.substr(13) << std::endl;
                } else if (message.rfind("DAEMON_SHUTDOWN_ACK", 0) == 0) {
                    is_running = false;
                } else if (message.rfind("RESOURCE_PUSH_FAIL:", 0) == 0) {
                    std::cerr << "Bridge ERROR: Daemon failed to push resources: " << message.substr(19) << std::endl;
                } else if (message.rfind("PUSH_SHADER:", 0) == 0) {
                    std::string args = message.substr(12);

                    if (size_t first_colon = args.find(':'); first_colon != std::string::npos) {
                        push_shader(args.substr(0, first_colon), args.substr(first_colon + 1));
                    }
                } else if (message.rfind("FINISH_LOADING", 0) == 0) {
                    finish_loading();
                } else if (message.rfind("PUSH_TEXTURE:", 0) == 0) {
                    std::string args = message.substr(13);

                    size_t p1 = args.find(':');
                    size_t p2 = p1 != std::string::npos ? args.find(':', p1 + 1) : std::string::npos;

                    if (size_t p3 = p2 != std::string::npos ? args.find(':', p2 + 1) : std::string::npos; p3 != std::string::npos) {
                        try {
                            std::string name_str = args.substr(0, p1);
                            std::string decoded_str = base64_decode(args.substr(p1 + 1, p2 - (p1 + 1)));

                            int width = std::stoi(args.substr(p2 + 1, p3 - (p2 + 1)));
                            int height = std::stoi(args.substr(p3 + 1));

                            auto buffer_ptr = std::make_unique<std::vector<char>>(decoded_str.begin(), decoded_str.end());
                            texture_data_buffers[name_str] = std::move(buffer_ptr);

                            push_texture(name_str, texture_data_buffers[name_str]->data(), width, height);
                        } catch (const std::exception& e) {
                            std::cerr << "Bridge ERROR: Failed to decode PUSH_TEXTURE: " << e.what() << std::endl;
                        }
                    }
                } else if (message.rfind("PUSH_FONT:", 0) == 0) {
                    std::string args = message.substr(10);

                    if (size_t first_colon = args.find(':'); first_colon != std::string::npos) {
                        try {
                            std::string decoded_str = base64_decode(args.substr(first_colon + 1));

                            std::vector<int8_t> bytes(decoded_str.begin(), decoded_str.end());

                            push_font(args.substr(0, first_colon), std::move(bytes));
                        } catch (const std::exception& e) {
                            std::cerr << "Bridge ERROR: Failed to decode PUSH_FONT: " << e.what() << std::endl;
                        }
                    }
                } else {
                    std::cerr << "Bridge WARNING: Unrecognized command from daemon: " << message << std::endl;
                }
            } else {
                is_running = false;
            }
        }
    }

    void bridge::push_shader(const std::string& name, const std::string& source) {
        zc_app::shaders::manager::add_shader(name, source);
    }

    void bridge::finish_loading() {
        std::lock_guard lock(app_runner::mtx);
        app_runner::ready = true;
        app_runner::cv.notify_all();
    }

    void bridge::push_texture(const std::string& name, void* buffer, int width, int height) {
        const zc_app::texture_loader::texture_info info{buffer, width, height};
        zc_app::texture_loader::push(name, info);
    }

    void bridge::push_font(const std::string& name, std::vector<int8_t> bytes) {
        zc_app::font_loader::push_font(name, std::move(bytes));
    }

    void bridge::connect_netty(const std::string& ip, int port) {
        if (daemon && daemon->is_connected()) {
            daemon->send_message("CONNECT_NETTY:" + ip + ":" + std::to_string(port));
        } else {
            std::cerr << "Bridge ERROR: Cannot send CONNECT_NETTY, IPC client not connected." << std::endl;
        }
    }

    void bridge::run() {
    }

    void bridge::terminate() {
        std::cout << "Bridge: Terminating daemon process." << std::endl;
        is_running = false;

        if (daemon && daemon->is_connected()) {
            daemon->send_message("SHUTDOWN");
        }

        if (ipc_read_thread && ipc_read_thread->joinable()) {
            ipc_read_thread->join();
        }

        if (daemon) {
            daemon->disconnect();
        }

        texture_data_buffers.clear();
        fflush(stdout);
        exit(0);
    }
}
