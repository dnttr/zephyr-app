#pragma once

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <functional>

#include "ZCGKit/ipc/client.hpp"

namespace zc_kit
{
    class bridge
    {
        static constexpr std::string IPC_SEND_STATUS = "SEND_STATUS:";
        static constexpr std::string IPC_SEND_DESCRIPTION = "SEND_DESCRIPTION:";
        static constexpr std::string IPC_SEND_CHAT = "SEND_CHAT:";
        static constexpr std::string IPC_ANSWER_RELAY_ACCEPT = "ANSWER_RELAY:accept";
        static constexpr std::string IPC_ANSWER_RELAY_REFUSE = "ANSWER_RELAY:refuse";
        static constexpr std::string IPC_REQUEST_RELAY = "REQUEST_RELAY:";
        static constexpr std::string IPC_IDENTIFY = "IDENTIFY:";
        static constexpr std::string IPC_CONNECT_NETTY = "CONNECT_NETTY:";


        static constexpr std::string IPC_GET_USERS = "GET_USERS";
        static constexpr std::string IPC_DISCONNECT = "DISCONNECT_NETTY";
        static constexpr std::string IPC_SHUTDOWN = "SHUTDOWN";
        static constexpr std::string IPC_REQUEST_RESOURCES = "REQUEST_RESOURCES";

        enum events
        {
            EV_PUSH_SHADER,
            EV_PUSH_TEXTURE,
            EV_PUSH_FONT,
            EV_FINISH_LOADING,
            EV_DAEMON_READY,
            EV_READY_FOR_IDENTIFY,
            EV_IDENTIFY_SUCCESS,
            EV_IDENTIFY_FAIL,
            EV_INCOMING_RELAY_REQUEST,
            EV_RELAY_ESTABLISHED,
            EV_RELAY_REFUSED,
            EV_RELAY_TERMINATED,
            EV_INCOMING_CHAT,
            EV_INCOMING_STATUS,
            EV_INCOMING_DESCRIPTION,
            EV_PUSH_USER_LIST,
            EV_CONNECTION_FAILED,
            EV_DISCONNECTED
        };

    public:
        static void internal_initialize(const std::string &jar_path);
        static void internal_terminate();
        static void internal_request_resources();

        static void client_connect(const std::string &ip, int port);
        static void client_disconnect();
        static void client_identify(const std::string &name);
        static void client_request_relay(const std::string &target_name);
        static void client_answer_relay(bool accept);
        static void client_get_users();

        static void client_message(const std::string &msg);
        static void client_status(int status_code);
        static void client_description(const std::string &description);

        static std::function<void()> on_ready_for_identification;
        static std::function<void(bool success, const std::string &reason)> on_identification_result;
        static std::function<void(const std::string &name)> on_incoming_relay_request;
        static std::function<void()> on_relay_established;
        static std::function<void()> on_relay_refused;
        static std::function<void(const std::string &reason)> on_relay_terminated;
        static std::function<void(const std::string &message)> on_chat_message_received;
        static std::function<void(int status)> on_status_received;
        static std::function<void(const std::string &description)> on_description_received;
        static std::function<void(const std::string &json_payload)> on_user_list_received;
        static std::function<void(const std::string &)> on_connection_failed;
        static std::function<void()> on_disconnected;

    private:
        static void _loop();

        static const std::unordered_map<std::string, events> events_map;

        static std::unique_ptr<std::thread> ipc_read_thread;
        static std::unique_ptr<zcg_kit::client> daemon;

        static std::atomic_bool shutdown_requested;
        static std::atomic_bool is_running;
    };
}
