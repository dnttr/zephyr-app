//
// Created by Damian Netter on 07/07/2025.
//

#include "ZCApp/app/data_manager.hpp"

#include <ranges>

zc_app::message_data::message_data(std::string msg, const bool sent, std::string sender): content(std::move(msg)), is_sent(sent), timestamp(std::chrono::system_clock::now()), sender_name(std::move(sender))
{
}

zc_app::conversation_data::conversation_data(): is_online(false)
{
}

zc_app::conversation_data::conversation_data(const std::string &name, const std::string &avatar,
    const std::string &last_seen_text, const bool online): contact_name(name), contact_avatar(avatar), last_seen(last_seen_text), is_online(online)
{
}

bool zc_app::data_manager::has_conversation(const std::string &id) const
{
    return all_conversations.contains(id);
}

void zc_app::data_manager::create_conversation(const std::string &id, const std::string &name,
    const std::string &status)
{
    if (!has_conversation(id))
    {
        all_conversations[id] = conversation_data(name, "avatar.png", status, true);
    }
}

void zc_app::data_manager::add_message_to_conversation(const std::string &conversation_id,
    const message_data &new_message)
{
    if (const auto it = all_conversations.find(conversation_id); it != all_conversations.end())
    {
        it->second.messages.push_back(new_message);
    }
}

std::vector<std::string> zc_app::data_manager::fetch_all_conversation_ids() const
{
    std::vector<std::string> ids;
    ids.reserve(all_conversations.size());

    for (const auto &key : all_conversations | std::views::keys)
    {
        ids.push_back(key);
    }
    return ids;
}

zc_app::conversation_data zc_app::data_manager::fetch_conversation_by_id(const std::string &id) const
{
    const auto it = all_conversations.find(id);

    if (it == all_conversations.end())
    {
        throw std::runtime_error("Conversation with ID " + id + " not found.");
    }

    return it->second;
}

void zc_app::data_manager::reset()
{
    all_conversations.clear();
}
