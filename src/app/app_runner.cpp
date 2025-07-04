//
// Created by Damian Netter on 20/06/2025.
//

#include "ZCApp/app/app_runner.hpp"

#include <ZNBKit/jni/signatures/method/void_method.hpp>
#include <ZNBKit/vm/vm_management.hpp>

#include "ZCApp/graphics/window.hpp"
#include "ZCKit/bridge.hpp"

namespace zc_kit
{
    bool app_runner::ready = false;

    std::mutex app_runner::mtx;
    std::condition_variable app_runner::cv;

    const std::string app_runner::executor_klass_name = "org/dnttr/zephyr/management/Loader";

    const std::string app_runner::executor_method_name = "load";
    const std::string app_runner::executor_method_signature = "()V";

    void app_runner::run(const std::string &daemon)
    {
        try {
            bridge::initialize_bridge(daemon); // Initialize bridge with daemon path
            bridge::run(); // Start the daemon and its IPC listener; no libraries to pass here anymore

            zcg_kit::display::config display_cfg;

            display_cfg.window_width = 1920;
            display_cfg.window_height = 1080;
            display_cfg.virtual_width = 1920.0F;
            display_cfg.virtual_height = 1080.0F;

            zc_app::window window;

            window.allocate("Core Graphics", 0, 0, display_cfg);

            std::unique_lock lock(mtx);
            cv.wait(lock, [] { return ready; }); // Wait for resources to be pushed and FINISH_LOADING

            window.run();

            std::cout << "Window closed" << std::endl;

            bridge::terminate(); // Gracefully terminate the bridge and daemon

        } catch (const std::exception& e) {
            std::cerr << "App Runner Error: Unhandled exception during run: " << e.what() << std::endl;
            // Potentially add more robust error handling or exit code
        }
    }
}
