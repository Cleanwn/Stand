#include "CommandSpoofHostToken.hpp"

#include "AbstractPlayer.hpp"
#include "CNetworkPlayerMgr.hpp"
#include "CNetworkVoice.hpp"
#include "CPlayerInfo.hpp"
#include "CrossState.hpp"
#include "gta_ped.hpp"
#include "FiberPool.hpp"
#include "rlGamerInfo.hpp"
#include "pointers.hpp"
#include "Script.hpp"

namespace Stand
{
	CommandSpoofHostToken::CommandSpoofHostToken(CommandList* const parent)
		: CommandToggle(parent, LOC("SPFHT_D"), CMDNAMES("hosttokenspoofing", "hostokenspoofing"), LOC("SPFHT_H"))
	{
		instance = this;
	}

	void CommandSpoofHostToken::onChange(Click& click)
	{
		if (!click.isBasicEdition())
		{
			return onChangeImplUnavailable(click);
		}
		if (!click.isAuto())
		{
			if (m_on)
			{
				auto click_ = click.deriveAuto();
				setStateBool(click_, false);
				showWarning(click, LANG_GET_W("SPFHT_W"), 0, [this](ThreadContext thread_context)
				{
					Click click(CLICK_AUTO, thread_context);
					setStateBool( click, true);
					click.setSpoofUpdate();
				});
				return;
			}
			click.setSpoofUpdate();
		}
		if (!change_queued)
		{
			change_queued = true;
			FiberPool::queueJob([]
			{
				while (true)
				{
					if (*pointers::CLoadingScreens_ms_Context == 0)
					{
						auto m = *pointers::network_player_mgr;
						if (!(m && m->localPlayer))
						{
							break;
						}
					}
					Script::current()->yield();
				}
				change_queued = false;
				instance->applyValue();
			});
		}
	}

	using OriginalHostToken = uint64_t;

	void CommandSpoofHostToken::applyValue() const
	{
		const rage::netPeerId old_peerId = *pointers::local_peer_id;
		if (m_on)
		{
			if (!CrossState::get().struct_map.isStructInMap(OriginalHostToken))
			{
				CrossState::get().struct_map.addStructToMap(OriginalHostToken, static_cast<OriginalHostToken>(old_peerId));
			}
			changePeerId(old_peerId, spoofed_host_token);
		}
		else
		{
			if (CrossState::get().struct_map.isStructInMap(OriginalHostToken))
			{
				changePeerId(old_peerId, CrossState::get().struct_map.getStructFromMapConst(OriginalHostToken));
			}
		}
	}

	void CommandSpoofHostToken::changePeerId(const rage::netPeerId old_peerId, const rage::netPeerId peerId)
	{
		*pointers::local_peer_id = peerId;
		changePeerId(old_peerId, peerId, &(*pointers::ped_factory)->m_local_ped->player_info->gamer_info.peer.id);
		changePeerId(old_peerId, peerId, &pointers::rlPresence_m_ActingGamerInfo->peer.id);
		changePeerId(old_peerId, peerId, &pointers::rlPresence_GamerPresences_0_GamerInfo->peer.id);

		rage::VoiceChat* const vc_data = &(*pointers::voice_chat_data)->m_VoiceChat;
		for (uint32_t i = 0; i < vc_data->m_NumTalkers; ++i)
		{
			changePeerId(old_peerId, peerId, &vc_data->m_Talkers[i]->peer.id);
		}
	}

	void CommandSpoofHostToken::changePeerId(const rage::netPeerId old_peerId, const rage::netPeerId peerId, rage::netPeerId* addr)
	{
		if (*addr == old_peerId)
		{
			*addr = peerId;
		}
	}
}
