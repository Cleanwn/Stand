#pragma once

#include "CommandPlayerToggle.hpp"

#include "gta_net_game_event.hpp"
#include "pointers.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandPlayerSoundSpam : public CommandPlayerToggle
	{
	private:
		static constexpr int AUD_MAX_SCRIPT_SOUNDS = 100;

		static constexpr std::pair<Hash, Hash> sound_candidates[] = {
			{ ATSTRINGHASH("07"), ATSTRINGHASH("DLC_GR_CS2_Sounds") },
			//{ ATSTRINGHASH("Fly_Loop"), ATSTRINGHASH("dlc_xm_avngr_sounds") },
			{ ATSTRINGHASH("Air_Defences_Activated"), ATSTRINGHASH("DLC_sum20_Business_Battle_AC_Sounds") },
		};

		CGameScriptId dummy_script{};

		time_t last_sound = 0;

		std::unordered_set<uint8_t> local_sounds{};
		std::unordered_set<uint8_t> remote_sounds{};

	public:
		explicit CommandPlayerSoundSpam(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("SNDSPM"), CMDNAMES("soundspam"), NOLABEL, COMMANDPERM_AGGRESSIVE)
		{
			dummy_script.m_unk = 0;
			dummy_script.m_hash = 0;
			dummy_script.m_name[0] = 0;
			dummy_script.m_timestamp = 0;
			dummy_script.instance_id = -1;
			dummy_script.m_position_hash = 0;
		}
		
		void onEnable(Click& click) final
		{
			const auto p = PP_PTR->getPlayer();
			registerScriptTickEventHandler([this, p]
			{
				HANDLER_CHECK(m_on && p.exists());

				for (auto i = local_sounds.begin(); i != local_sounds.end(); )
				{
					if (AUDIO::HAS_SOUND_FINISHED(*i))
					{
						AUDIO::RELEASE_SOUND_ID(*i);
						i = local_sounds.erase(i);
					}
					else
					{
						++i;
					}
				}

				if (GET_MILLIS_SINCE(last_sound) >= 250)
				{
					const auto& s = ARRAY_RAND(sound_candidates);
					if (auto ped = p.getPed(); ped.isValid())
					{
						playSound(p, ped, s.second, s.first);
						last_sound = get_current_time_millis();
					}
				}

				HANDLER_END;
			});
		}

		void onDisable(Click& click) final
		{
			const auto p = PP_PTR->getPlayer();

			if (p == g_player)
			{
				ensureScriptThread([this]
				{
					for (const auto id : local_sounds)
					{
						*pointers::stop_sound_dont_network = 1;
						AUDIO::STOP_SOUND(id);
						*pointers::stop_sound_dont_network = 0;
						AUDIO::RELEASE_SOUND_ID(id);
					}

					local_sounds.clear();
				});
			}
			else
			{
				ensureYieldableScriptThread([this, p]
				{
					if (is_session_started())
					{
						for (const auto id : remote_sounds)
						{
							while (!g_player.canSendNetEvents())
							{
								Script::current()->yield();
							}

							SOUP_IF_UNLIKELY (!p.exists())
							{
								break;
							}

							pointers::CStopSoundEvent_Trigger(id);

							SOUP_IF_LIKELY (auto evt = (*pointers::CNetwork_sm_EventMgr)->getLatestEvent())
							{
								g_hooking.stop_sound_whitelist.add(p, evt);
							}
						}
					}

					remote_sounds.clear();
				});
			}
		}

	private:
		void playSound(const AbstractPlayer& p, AbstractEntity& ped, Hash set, Hash sound)
		{
			if (p == g_player)
			{
				if (const auto id = AUDIO::GET_SOUND_ID(); id < AUD_MAX_SCRIPT_SOUNDS)
				{
					AUDIO::PLAY_SOUND_FROM_ENTITY_HASH(id, sound, ped, set, FALSE, 0); // NativeDB documentation for this native is totally incorrect.
					local_sounds.emplace(id);
				}
			}
			else if (is_session_started() && g_player.canSendNetEvents())
			{
				auto& target_ent = ped;
				auto veh = ped.getVehicle();

				if (veh.isValid() && soup::rand.coinflip()) // Randomly place it on their vehicle.
				{
					target_ent = veh;
				}

				const auto sound_id = soup::rand.t<int>(1, AUD_MAX_SCRIPT_SOUNDS - 1);
				remote_sounds.emplace(sound_id);

				if (target_ent.isValid())
				{
					pointers::CPlaySoundEvent_TriggerEnt((CEntity*)target_ent.getPointer(), set, sound, sound_id, &dummy_script, soup::rand.t<int>(1, 25));
				}

				SOUP_IF_LIKELY (auto evt = (*pointers::CNetwork_sm_EventMgr)->getLatestEvent())
				{
					g_hooking.play_sound_whitelist.add(p, evt);
				}
			}
		}
	};
}