//
// Created by Damian Netter on 06/07/2025.
//

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

        void populate_dummy_data()
        {
            conversation_data conv1("Joshua Hardstick", "avatar.png", "Last seen 5 minutes ago", true);
            conv1.messages.emplace_back("Hey! How was your weekend?", false, "Joshua Hardstick");
            conv1.messages.emplace_back("It was great! Went hiking with friends. How about you?", true, "You");
            conv1.messages.emplace_back("Nice! I stayed home and binged Netflix", false, "Joshua Hardstick");
            conv1.messages.emplace_back("Haha, sometimes that's exactly what you need!", true, "You");
            conv1.messages.emplace_back("Absolutely! Any show recommendations? Looking for something new to watch. Maybe a sci-fi or a mystery? This message is quite long and should definitely wrap around multiple lines for testing purposes. It contains many characters that might push the boundary of the bubble. Let's see if it fits nicely and grows the bubble correctly.", false, "Joshua Hardstick");
            conv1.messages.emplace_back("I just finished 'The Expanse' and it was amazing! Highly recommend it if you like sci-fi.", true, "You");
            conv1.messages.emplace_back("Oh, I've heard good things about that one! Is it on Netflix?", false, "Joshua Hardstick");
            conv1.messages.emplace_back("I think it's on Amazon Prime Video. Definitely worth checking out!", true, "You");
            conv1.messages.emplace_back("Cool, I'll add it to my watchlist. Thanks for the tip!", false, "Joshua Hardstick");
            conv1.messages.emplace_back("No problem! Enjoy!", true, "You");
            all_conversations["user1"] = std::move(conv1);

            conversation_data conv2("Sarah Miller", "avatar.png", "Last seen 2 hours ago", false);
            conv2.messages.emplace_back("Are we still on for dinner tonight?", false, "Sarah Miller");
            conv2.messages.emplace_back("Yes! Looking forward to it", true, "You");
            conv2.messages.emplace_back("Great! See you at 7", false, "Sarah Miller");
            all_conversations["user2"] = std::move(conv2);

            conversation_data conv3("John Doe", "avatar.png", "Last seen 1 day ago", false);
            conv3.messages.emplace_back("Hi there!", false, "John Doe");
            conv3.messages.emplace_back("Hey John! How are you?", true, "You");
            conv3.messages.emplace_back("I'm doing well, thanks! Just working on a new project.", false, "John Doe");
            conv3.messages.emplace_back("Sounds interesting! What kind of project?", true, "You");
            conv3.messages.emplace_back("It's a really complex one with lots of moving parts and requires a lot of concentration and long hours. Hopefully, it pays off in the end!", false, "John Doe");
            conv3.messages.emplace_back("Wow, that sounds intense! Good luck with it! Don't forget to take breaks.", true, "You");
            conv3.messages.emplace_back("I'll try! Maybe we can catch up once it's done?", false, "John Doe");
            conv3.messages.emplace_back("Definitely! Let me know when you're free. I'm always up for a coffee.", true, "You");
            conv3.messages.emplace_back("Sounds like a plan! I'll be in touch.", false, "John Doe");
            conv3.messages.emplace_back("Great, looking forward to it!", true, "You");
            all_conversations["user3"] = std::move(conv3);

            conversation_data conv4("Alice", "avatar.png", "Online", true);
            conv4.messages.emplace_back("Hello!", false, "Alice");
            conv4.messages.emplace_back("Hey!", true, "You");
            all_conversations["user4"] = std::move(conv4);
        }

    public:
        data_manager()
        {
            populate_dummy_data();
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

        void send_message_to_server(const std::string& conversation_id, const message_data& new_message)
        {
            const auto it = all_conversations.find(conversation_id);

            if (it != all_conversations.end())
            {
                it->second.messages.push_back(new_message);
            }
        }
    };
}
}