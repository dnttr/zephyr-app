#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <stdexcept>

namespace zc_app
{
    struct message_data
    {
        std::string content;
        bool is_sent;
        std::chrono::system_clock::time_point timestamp;
        std::string sender_name;

        message_data(const std::string &msg, bool sent, const std::string &sender = "")
            : content(msg), is_sent(sent), timestamp(std::chrono::system_clock::now()), sender_name(sender)
        {
        }
    };

    struct conversation_data
    {
        std::string contact_name;
        std::string contact_avatar;
        std::string last_seen;
        std::vector<message_data> messages;
        bool is_online;

        conversation_data() : is_online(false)
        {
        }

        conversation_data(const std::string &name, const std::string &avatar, const std::string &last_seen_text,
                          bool online = false)
            : contact_name(name), contact_avatar(avatar), last_seen(last_seen_text), is_online(online)
        {
        }
    };
}

namespace zc_app
{
namespace data
{
    class data_manager
    {
    private:
        std::unordered_map<std::string, conversation_data> all_conversations;

    public:
        bool is_client_connected = false;

        data_manager() = default;

        bool has_conversation(const std::string& id) const
        {
            return all_conversations.count(id);
        }

        void create_conversation(const std::string& id, const std::string& name, const std::string& status)
        {
            if (!has_conversation(id))
            {
                all_conversations[id] = conversation_data(name, "avatar.png", status, true);
            }
        }

        void add_message_to_conversation(const std::string& conversation_id, const message_data& new_message)
        {
            const auto it = all_conversations.find(conversation_id);
            if (it != all_conversations.end())
            {
                it->second.messages.push_back(new_message);
            }
        }

        bool attempt_connection(const std::string& ip, const std::string& port)
        {
            if (ip == "127.0.0.1" && !port.empty()) {
                is_client_connected = true;
                return true;
            }

            is_client_connected = false;
            return false;
        }

        std::vector<std::string> fetch_all_conversation_ids() const
        {
            std::vector<std::string> ids;
            ids.reserve(all_conversations.size());
            for (const auto& pair : all_conversations)
            {
                ids.push_back(pair.first);
            }
            return ids;
        }

        conversation_data fetch_conversation_by_id(const std::string& id) const
        {
            const auto it = all_conversations.find(id);

            if (it == all_conversations.end())
            {
                throw std::runtime_error("Conversation with ID " + id + " not found.");
            }

            return it->second;
        }
    };
}
}