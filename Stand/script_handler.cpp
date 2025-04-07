#include "script_handler.hpp"

#include "AbstractPlayer.hpp"
#include "CNetGamePlayer.hpp"
#include "net_array.hpp"
#include "Script.hpp"
#include "scriptHandlerMgr.hpp"
#include "script_packets.hpp"

namespace rage
{
    using namespace Stand;

    bool scriptHandlerNetComponent::amHost() const noexcept
    {
        return host_data == nullptr
            || host_data->net_player == g_player.getCNetGamePlayer();
            ;
    }

    bool scriptHandlerNetComponent::isHost(CNetGamePlayer* player) const noexcept
    {
        return host_data != nullptr
            && host_data->net_player == player
            ;
    }

    void scriptHandlerNetComponent::takeHost() noexcept
    {
		MigrateScriptHost(*g_player.getCNetGamePlayer());
	}

    void scriptHandlerNetComponent::giveHost(CNetGamePlayer* player) noexcept
    {
        if (!isHost(player))
        {
            if (!amHost())
            {
                takeHost();
                Script::current()->yield();
            }
            giveHostRaw(player);
        }
    }

    void scriptHandlerNetComponent::giveHostRaw(CNetGamePlayer* player) noexcept
    {
		rage::msgScriptMigrateHost msg{};
		msg.script_id = &m_script_handler->GetScriptId();
		msg.host_token = this->host_token;
		msg.net_array_data_size = this->net_array_data_size + player->m_sizeOfNetArrayData + 1;
		rage::scriptHandlerMgr::SendMessageToPlayer(*player, msg);
    }

    netHostBroadcastDataHandlerBase* scriptHandlerNetComponent::GetHostBroadcastDataHandler(uint32_t handlerNum)
    {
        // original also has a range check against m_NumHostBroadcastData
        return reinterpret_cast<netArrayManager_Script*>(*pointers::net_array_mgr)->GetScriptHostBroadcastDataArrayHandler(
            m_script_handler->GetScriptId(),
            handlerNum
        );
    }
}
