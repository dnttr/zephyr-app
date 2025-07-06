#include "ZCApp/app/app_runner.hpp"
#include "ZCApp/graphics/window.hpp"
#include "ZCKit/bridge.hpp"

#include <condition_variable>
#include <exception>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>

#include "ZCApp/app/test/test_runner.hpp"

namespace zc_kit
{
    std::mutex app_runner::mtx;
    std::condition_variable app_runner::condition;

    bool app_runner::ready = false;

    void app_runner::run_application() {
        zcg_kit::display::config display_cfg;
        display_cfg.window_width = 1920;
        display_cfg.window_height = 1080;
        display_cfg.virtual_width = 1920.0F;
        display_cfg.virtual_height = 1080.0F;

        zc_app::window window;

        window.allocate("Core Graphics", 0, 0, display_cfg);

        std::unique_lock lock(mtx);
        condition.wait(lock, [] { return app_runner::ready; });

        window.run();

    }

    void app_runner::run_test(const std::string& role) {
        std::cout << "--- RUNNING IN TEST MODE (" << role << ") ---" << std::endl;

        test_runner::setup_test_callbacks(role);
        test_runner::reset_promises();
        bridge::client_connect("127.0.0.1", 12345);

        if (role == "client_a") {
            test_runner::run_client_a();
        } else if (role == "client_b") {
            test_runner::run_client_b();
        }
    }
}
