//
// Created by Damian Netter on 06/07/2025.
//

#pragma once

#include <string>
#include <future>

namespace zc_kit
{
    class test_runner
    {
    public:
        static void run_test_harness(const std::string &executable_path, const std::string &main_jar);

        static void run_client_a();
        static void run_client_b();

        static void setup_test_callbacks(const std::string &role);

        static void reset_promises();

    private:
        static std::promise<void> g_a_ready_for_identification_promise;
        static std::promise<void> g_a_identification_success_promise;
        static std::promise<std::string> g_a_identification_failure_promise;
        static std::promise<void> g_a_relay_established_promise;
        static std::promise<void> g_a_relay_refused_promise;
        static std::promise<std::string> g_a_relay_terminated_promise;

        static std::promise<void> g_b_ready_for_identification_promise;
        static std::promise<void> g_b_identification_success_promise;
        static std::promise<std::string> g_b_identification_failure_promise;
        static std::promise<std::string> g_b_incoming_relay_promise;
        static std::promise<std::string> g_b_chat_message_promise;
    };
}
