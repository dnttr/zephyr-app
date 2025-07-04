//
// Created by Damian Netter on 20/06/2025.
//

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <map>

#include "ZCGKit/ipc/client.hpp"

namespace zcg_kit {
    class client;
}

namespace zc_kit
{
    class bridge
    {
        static std::unique_ptr<zcg_kit::client> daemon;
        static std::unique_ptr<std::thread> ipc_read_thread;
        static std::atomic_bool is_running;
        static std::map<std::string, std::unique_ptr<std::vector<char>>> texture_data_buffers;

        static void ipc_listener_loop();

    public:
        static void initialize_bridge(const std::string& daemon_jar_path);
        static void connect_netty(const std::string& ip, int port);
        static void run();
        static void terminate();

        static void push_shader(const std::string& name, const std::string& source);
        static void finish_loading();
        static void push_texture(const std::string& name, void* buffer, int width, int height);
        static void push_font(const std::string& name, std::vector<int8_t> bytes);
    };
}