//
// Created by Damian Netter on 20/06/2025.
//

#include "ZCKit/bridge.hpp"

#include <iostream>
#include <stdexcept>

#include "ZCApp/app/app_runner.hpp"
#include "ZCApp/graphics/textures/texture_loader.hpp"
#include "ZCApp/graphics/fonts/font_loader.hpp"


namespace zc_kit
{
    std::unique_ptr<zcg_kit::client> bridge::daemon_ipc_client = nullptr;
    std::unique_ptr<std::thread> bridge::ipc_read_thread = nullptr;
    std::atomic_bool bridge::ipc_running = false;

    void bridge::initialize_bridge(const std::string& daemon_jar_path)
    {
        daemon_ipc_client = std::make_unique<zcg_kit::client>();

          if (!daemon_ipc_client->start(daemon_jar_path)) {
            std::cerr << "Error: Failed to start Java daemon process at " << daemon_jar_path << std::endl;

              throw std::runtime_error("Failed to start Java daemon process.");
        }

        std::cout << "Java daemon process started successfully." << std::endl;

        ipc_running = true;
        ipc_read_thread = std::make_unique<std::thread>(ipc_listener_loop);

        std::cout << "IPC listener thread started." << std::endl;
    }

    void bridge::ipc_listener_loop()
    {
        std::cout << "IPC Listener: Thread started." << std::endl;

        while (ipc_running) {
            if (daemon_ipc_client && daemon_ipc_client->is_connected()) {
                const auto message = daemon_ipc_client->read();

                if (!message.empty()) {
                    std::cout << "IPC Listener: Received message from Java daemon: " << message;
                }
            } else {
                if (ipc_running) {
                    std::cerr << "IPC Listener: Daemon not connected or IPC client invalid. Stopping listener." << std::endl;
                }

                ipc_running = false;
            }

            usleep(10 * 1000);
        }

        std::cout << "IPC Listener: Thread stopping." << std::endl;
    }


    void bridge::push_shader(const std::string& name, const std::string& source)
    {
        std::cout << "Bridge (C++): Received command to push shader: Name=" << name << ", Source (truncated)=" << source.substr(0, 50) << "..." << std::endl;
    }

    void bridge::finish_loading()
    {
        std::cout << "Bridge (C++): Received command to finish loading." << std::endl;
    }

    void bridge::push_texture(const std::string& name, const std::vector<char>& buffer, int width, int height)
    {
        std::cout << "Bridge (C++): Received command to push texture: Name=" << name << ", Width=" << width << ", Height=" << height << ", Buffer Size=" << buffer.size() << std::endl;
    }

    void bridge::push_font(const std::string& name, const std::vector<char>& bytes)
    {
        std::cout << "Bridge (C++): Received command to push font: Name=" << name << ", Bytes Size=" << bytes.size() << std::endl;
    }

    void bridge::terminate()
    {
        std::cout << "Bridge: Initiating termination sequence." << std::endl;
        ipc_running = false;

        if (ipc_read_thread && ipc_read_thread->joinable()) {
            std::cout << "Bridge: Joining IPC listener thread." << std::endl;
            ipc_read_thread->join();
        }

        if (daemon_ipc_client) {
            std::cout << "Bridge: Disconnecting daemon IPC client." << std::endl;
            daemon_ipc_client->disconnect();
        } else {
            std::cout << "Bridge: Daemon IPC client not initialized. No disconnect needed." << std::endl;
        }
    }
}
