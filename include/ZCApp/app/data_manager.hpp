#pragma once

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <stdexcept>

namespace zc_app
{
    struct message_data
    {
        std::chrono::system_clock::time_point timestamp;

        std::string sender_name;
        std::string content;

        bool is_sent;

        message_data(std::string msg, bool sent, std::string sender = "");
    };

    struct conversation_data
    {
        std::string contact_name;
        std::string contact_avatar;
        std::string last_seen;
        std::vector<message_data> messages;
        bool is_online;

        conversation_data();

        conversation_data(const std::string &name, const std::string &avatar, const std::string &last_seen_text,
                          bool online = false);
    };

    class data_manager
    {
        std::unordered_map<std::string, conversation_data> all_conversations;

    public:
        data_manager() = default;

        [[nodiscard]] bool has_conversation(const std::string &id) const;

        void create_conversation(const std::string &id, const std::string &name, const std::string &status);

        void add_message_to_conversation(const std::string &conversation_id, const message_data &new_message);

        [[nodiscard]] std::vector<std::string> fetch_all_conversation_ids() const;

        [[nodiscard]] conversation_data fetch_conversation_by_id(const std::string &id) const;

        void reset();
    };
}
