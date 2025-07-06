//
// Created by Damian Netter on 06/07/2025.
//

#include "ZCApp/app/test/test_runner.hpp"

#include "ZCKit/bridge.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <future>

namespace zc_kit
{
    std::promise<void> test_runner::g_a_ready_for_identification_promise;
    std::promise<void> test_runner::g_a_identification_success_promise;
    std::promise<std::string> test_runner::g_a_identification_failure_promise;
    std::promise<void> test_runner::g_a_relay_established_promise;
    std::promise<void> test_runner::g_a_relay_refused_promise;
    std::promise<std::string> test_runner::g_a_relay_terminated_promise;

    std::promise<void> test_runner::g_b_ready_for_identification_promise;
    std::promise<void> test_runner::g_b_identification_success_promise;
    std::promise<std::string> test_runner::g_b_identification_failure_promise;
    std::promise<std::string> test_runner::g_b_incoming_relay_promise;
    std::promise<std::string> test_runner::g_b_chat_message_promise;

    void test_runner::reset_promises()
    {
        g_a_ready_for_identification_promise = std::promise<void>();
        g_a_identification_success_promise = std::promise<void>();
        g_a_identification_failure_promise = std::promise<std::string>();
        g_a_relay_established_promise = std::promise<void>();
        g_a_relay_refused_promise = std::promise<void>();
        g_a_relay_terminated_promise = std::promise<std::string>();

        g_b_ready_for_identification_promise = std::promise<void>();
        g_b_identification_success_promise = std::promise<void>();
        g_b_identification_failure_promise = std::promise<std::string>();
        g_b_incoming_relay_promise = std::promise<std::string>();
        g_b_chat_message_promise = std::promise<std::string>();
    }

    void test_runner::setup_test_callbacks(const std::string &role)
    {
        if (role == "client_a")
        {
            bridge::on_ready_for_identification = []
            {
                g_a_ready_for_identification_promise.set_value();
            };

            bridge::on_identification_result = [](const bool success, const std::string &reason)
            {
                if (success)
                {
                    g_a_identification_success_promise.set_value();
                }
                else
                {
                    g_a_identification_failure_promise.set_value(reason);
                }
            };

            bridge::on_relay_established = []
            {
                g_a_relay_established_promise.set_value();
            };

            bridge::on_relay_refused = []
            {
                g_a_relay_refused_promise.set_value();
            };

            bridge::on_relay_terminated = [](const std::string &reason)
            {
                g_a_relay_terminated_promise.set_value(reason);
            };
        }
        else if (role == "client_b")
        {
            bridge::on_ready_for_identification = []
            {
                g_b_ready_for_identification_promise.set_value();
            };

            bridge::on_identification_result = [](bool success, const std::string &reason)
            {
                if (success)
                {
                    g_b_identification_success_promise.set_value();
                }
                else
                {
                    g_b_identification_failure_promise.set_value(reason);
                }
            };

            bridge::on_incoming_relay_request = [](const std::string &sender_name)
            {
                g_b_incoming_relay_promise.set_value(sender_name);
            };

            bridge::on_chat_message_received = [](const std::string &msg)
            {
                g_b_chat_message_promise.set_value(msg);
            };
        }
    }

    void test_runner::run_test_harness(const std::string &executable_path, const std::string &main_jar)
    {
        std::cout << "--- RUNNING AS TEST HARNESS ---" << std::endl;

        const std::string client_a_cmd = executable_path + " --main " + main_jar + " --role client_a";
        const std::string client_b_cmd = executable_path + " --main " + main_jar + " --role client_b";

        std::thread client_b_thread([&]()
        {
            std::system(client_b_cmd.c_str());
        });

        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::thread client_a_thread([&]()
        {
            std::system(client_a_cmd.c_str());
        });

        client_a_thread.join();
        client_b_thread.join();

        std::cout << "--- TEST HARNESS FINISHED ---" << std::endl;
    }

    void test_runner::run_client_a()
    {
        const auto ready_future = g_a_ready_for_identification_promise.get_future();
        const auto success_future = g_a_identification_success_promise.get_future();
        const auto established_future = g_a_relay_established_promise.get_future();
        const auto refused_future = g_a_relay_refused_promise.get_future();
        const auto terminated_future = g_a_relay_terminated_promise.get_future();

        auto failure_future = g_a_identification_failure_promise.get_future();

        std::cout << "[A] Waiting for daemon to be ready for identification..." << std::endl;

        if (ready_future.wait_for(std::chrono::seconds(10)) != std::future_status::ready)
        {
            std::cerr << "[A] FAILED: Did not get READY_FOR_IDENTIFY." << std::endl;

            return;
        }

        std::cout << "[A] Daemon is ready for identification. Proceeding to identify." << std::endl;
        std::cout << "[A] Identifying as 'ClientA'..." << std::endl;

        bridge::client_identify("ClientA");

        std::cout << "[A] Identification sent. Waiting for server processing..." << std::endl;

        const auto ident_status = success_future.wait_for(std::chrono::seconds(5));

        if (ident_status == std::future_status::ready)
        {
            std::cout << "[A] Identification successful." << std::endl;
        }
        else
        {
            if (failure_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                std::cerr << "[A] FAILED: Identification failed: " << failure_future.get() << std::endl;
            }
            else
            {
                std::cerr << "[A] FAILED: Identification timed out." << std::endl;
            }

            return;
        }

        std::cout << "[A] Giving Client B time to connect and identify..." << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cout << "[A] Requesting relay to 'ClientB'..." << std::endl;

        bridge::client_request_relay("ClientB");

        std::cout << "[A] Waiting for relay response..." << std::endl;
        const auto status = established_future.wait_for(std::chrono::seconds(10));

        if (status == std::future_status::ready)
        {
            std::cout << "[A] PASSED: Relay established." << std::endl;

            bridge::client_message("Hello from Client A!");
            terminated_future.wait();

            std::cout << "[A] PASSED: Relay termination received." << std::endl;
        }
        else
        {
            if (refused_future.wait_for(std::chrono::seconds(1)) == std::future_status::ready)
            {
                std::cout << "[A] PASSED: Relay was correctly refused." << std::endl;
            }
            else
            {
                std::cerr << "[A] FAILED: Timed out waiting for relay response." << std::endl;
            }
        }
    }

    void test_runner::run_client_b()
    {
        const auto ready_future = g_b_ready_for_identification_promise.get_future();
        const auto success_future = g_b_identification_success_promise.get_future();

        auto failure_future = g_b_identification_failure_promise.get_future();
        auto incoming_future = g_b_incoming_relay_promise.get_future();
        auto chat_future = g_b_chat_message_promise.get_future();

        std::cout << "[B] Waiting for daemon to be ready for identification..." << std::endl;

        const auto ready_status = ready_future.wait_for(std::chrono::seconds(10));

        if (ready_status == std::future_status::ready)
        {
            std::cout << "[B] Daemon is ready for identification. Proceeding to identify." << std::endl;
            std::cout << "[B] Identifying as 'ClientB'..." << std::endl;

            bridge::client_identify("ClientB");

            std::cout << "[B] Identification sent. Waiting for server processing..." << std::endl;

            const auto ident_status = success_future.wait_for(std::chrono::seconds(5));

            if (ident_status == std::future_status::ready)
            {
                std::cout << "[B] Identification successful." << std::endl;
            }
            else
            {
                if (failure_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    std::cerr << "[B] FAILED: Identification failed: " << failure_future.get() << std::endl;
                }
                else
                {
                    std::cerr << "[B] FAILED: Identification timed out." << std::endl;
                }

                return;
            }
        }
        else
        {
            std::cerr << "[B] FAILED: Daemon did not signal READY_FOR_IDENTIFY in time." << std::endl;

            return;
        }

        std::cout << "[B] Waiting for incoming relay request..." << std::endl;

        if (incoming_future.wait_for(std::chrono::seconds(10)) != std::future_status::ready)
        {
            std::cerr << "[B] FAILED: Timed out waiting for relay request." << std::endl;

            return;
        }

        std::cout << "[B] Received relay request from '" << incoming_future.get() << "'. Accepting..." << std::endl;

        bridge::client_answer_relay(true);

        std::cout << "[B] Waiting for chat message..." << std::endl;

        if (chat_future.wait_for(std::chrono::seconds(5)) != std::future_status::ready)
        {
            std::cerr << "[B] FAILED: Timed out waiting for chat message." << std::endl;

            return;
        }

        const std::string msg = chat_future.get();

        if (msg == "Hello from Client A!")
        {
            std::cout << "[B] PASSED: Correct chat message received." << std::endl;
        }
        else
        {
            std::cerr << "[B] FAILED: Received incorrect message: '" << msg << "'" << std::endl;
        }
    }
}
