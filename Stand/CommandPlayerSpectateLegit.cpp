#include "CommandPlayerSpectateLegit.hpp"

#include "AbstractEntity.hpp"
#include "Camgr.hpp"
#include "natives.hpp"
#include "PlayerProvider.hpp"
#include "pointers.hpp"
#include "Script.hpp"
#include "SpectateMgr.hpp"

namespace Stand
{
	CommandPlayerSpectateLegit::CommandPlayerSpectateLegit(CommandList* const parent)
		: CommandPlayerSpectate(parent, LOC("LGTM"))
	{
	}

	static soup::Bytepatch chat_patch{};

	void CommandPlayerSpectateLegit::onEnable(Click& click)
	{
		const AbstractPlayer p = PP_PTR->getPlayers(true).at(0);
		ensureYieldableScriptThread(click, [this, p]
		{
			if (dedup)
			{
				dedup = false;
				Script::current()->yield();
			}
			dedup = true;
			Camgr::stopAnyActiveCamCommand();
			registerScriptTickEventHandler(TC_SCRIPT_YIELDABLE, [this, p]()
			{
				const bool can_run = (m_on && dedup);
				if (can_run && p.exists())
				{
					SpectateMgr::onTickSpectating(p.getPed());
					if (pointers::UpdateTypers_call_IsInSpectatorMode)
					{
						uint8_t patch[] = {
							0x31, 0xc0, 0x90, 0x90, 0x90 // call ... -> xor eax, eax
						};
						chat_patch.initPatch(pointers::UpdateTypers_call_IsInSpectatorMode, patch, COUNT(patch));
					}
					NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(true, g_cam_ped);
					HUD::SET_MINIMAP_IN_SPECTATOR_MODE(true, g_cam_ped);
				}
				else
				{
					SpectateMgr::onStopSpectating();
					chat_patch.restore();
					NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(false, g_cam_ped);
					HUD::SET_MINIMAP_IN_SPECTATOR_MODE(false, g_cam_ped);
					NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(true, g_player_ped);
					HUD::SET_MINIMAP_IN_SPECTATOR_MODE(true, g_player_ped);
					NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(false, g_player_ped);
					HUD::SET_MINIMAP_IN_SPECTATOR_MODE(false, g_player_ped);
				}
				if (can_run)
				{
					SpectateMgr::onTickSpectating();
					return true;
				}
				if (m_on)
				{
					m_on = false;
					Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
					updateState(click);
				}
				return false;
			});
		});
	}
}