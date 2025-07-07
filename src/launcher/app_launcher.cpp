#include <iostream>
#include <filesystem>
#include <map>
#include <vector>
#include <mach-o/dyld.h>

#include "ZCApp/app/app_runner.hpp"
#include "ZCApp/app/test/test_runner.hpp"
#include "ZCKit/bridge.hpp"

#define GL_SILENCE_DEPRECATION

std::string get_executable_dir() {
    std::vector<char> path_buffer(1024);

    auto size = static_cast<uint32_t>(path_buffer.size());

    if (_NSGetExecutablePath(path_buffer.data(), &size) != 0) {
        path_buffer.resize(size);

        if (_NSGetExecutablePath(path_buffer.data(), &size) != 0) {
            throw std::runtime_error("Failed to get executable path.");
        }
    }

    const std::string path_str(path_buffer.data());
    return std::filesystem::path(path_str).parent_path().string();
}

struct command_line_args {
    std::string main_jar;
    std::string role = "app";
};

command_line_args parse_arguments(const int argc, char *argv[]) {
    command_line_args args;
    for (int i = 1; i < argc; ++i) {
        if (const std::string_view arg = argv[i]; arg == "--main" && i + 1 < argc) {
            args.main_jar = argv[++i];
        } else if (arg == "--role" && i + 1 < argc) {
            args.role = argv[++i];
        }
    }
    return args;
}

int main(const int argc, char *argv[]) {
    try {
        auto args = parse_arguments(argc, argv);

        if (args.main_jar.empty()) {

            std::string exe_dir = get_executable_dir();
            std::filesystem::path jar_path = std::filesystem::path(exe_dir) / "daemon.jar";

            if (std::filesystem::exists(jar_path)) {
                args.main_jar = jar_path.string();
                std::cout << "Using local daemon.jar: " << args.main_jar << std::endl;
            } else {
                throw std::runtime_error("Missing --main <path_to_jar> and daemon.jar not found in executable directory.");
            }
        }

        if (args.role == "test_harness") {
            zc_kit::test_runner::run_test_harness(argv[0], args.main_jar);
            return 0;
        }

        std::string daemon_path = get_executable_dir() + "/zc_daemon";

        zc_kit::bridge::internal_initialize(args.main_jar, daemon_path);

        if (args.role == "client_a" || args.role == "client_b") {
            zc_kit::app_runner::run_test(args.role);
        } else {
            zc_kit::app_runner::run_application();
        }

        zc_kit::bridge::internal_terminate();
    }
    catch (const std::exception &e) {
        std::cerr << "App Runner Error: " << e.what() << std::endl;
        zc_kit::bridge::internal_terminate();
    }

    return 0;
}
