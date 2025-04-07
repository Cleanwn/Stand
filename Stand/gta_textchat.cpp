#include "gta_textchat.hpp"

#include <algorithm>

using namespace Stand;

void CNetworkTextChat::PlayerChatHistory::compileHistory(std::vector<ChatHistoryMessage>& messages) const
{
    for (const auto& line : history)
    {
        if (line.messsage[0] != 0)
        {
            messages.emplace_back(ChatHistoryMessage{ gamerHandle.rockstar_id, line.messsage, line.timestamp });
        }
    }
}

std::vector<ChatHistoryMessage> CNetworkTextChat::compileHistory() const
{
    std::vector<ChatHistoryMessage> messages;

    // Emplace of all messages sent by everyone
    for (int i = 0; i != m_ChatHistory.m_Slots; ++i)
    {
        auto e = m_ChatHistory.m_Hash[i];
        while (e)
        {
            e->data.compileHistory(messages);
            e = e->next;
        }
    }

    // Sort by timestamp
    std::sort(messages.begin(), messages.end(), [](const ChatHistoryMessage& a, const ChatHistoryMessage& b)
    {
        return a.timestamp < b.timestamp;
    });

    return messages;
}
