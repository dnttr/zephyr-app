//
// Created by Damian Netter on 20/06/2025.
//

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>

#include "ZCGKit/ipc/client.hpp"

namespace zc_app
{
    namespace shaders
    {
        class manager;
    }

    namespace texture_loader
    {
        struct texture_info;
    }

    namespace font_loader
    {
    }

    namespace app_runner
    {
        extern std::mutex mtx;
        extern std::condition_variable cv;
        extern bool ready;
    }
}


namespace zc_kit
{
    class bridge
    {
    public:

        static std::unique_ptr<zcg_kit::client> daemon_ipc_client;
        static std::unique_ptr<std::thread> ipc_read_thread;
        static std::atomic_bool ipc_running;

        static void initialize_bridge(const std::string &daemon_jar_path);

        static void ipc_listener_loop();

        static void push_shader(const std::string &name, const std::string &source);
        static void finish_loading();
        static void push_texture(const std::string &name, const std::vector<char> &buffer, int width, int height);
        static void push_font(const std::string &name, const std::vector<char> &bytes);

        static void terminate();
    };
}
