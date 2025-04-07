#pragma once

#include "CommandPlayerToggle.hpp"

#include "explosion.hpp"

namespace Stand
{
	class CommandPlayerVehSlipperyTyres : public CommandPlayerToggle
	{
	private:
		std::unordered_map<compactplayer_t, time_t> last_sent{};

		[[nodiscard]] bool processSend(compactplayer_t player)
		{
			if (auto entry = last_sent.find(player); entry != last_sent.end())
			{
				if (GET_MILLIS_SINCE(entry->second) <= 5000)
				{
					return false;
				}
				else
				{
					entry->second = get_current_time_millis();
				}
			}
			else
			{
				last_sent.emplace(player, get_current_time_millis());
			}

			return true;
		}

	public:
		explicit CommandPlayerVehSlipperyTyres(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("SLIPPERY"), CMDNAMES_OBF("slippery"), LOC("SLIPPERY_H"), COMMANDPERM_AGGRESSIVE)
		{
		}

		void onEnable(Click& click) final
		{
			PP_PTR->toggleBatch(getPathEnFromParent(COMMAND_LIST_PLAYER), [this](const std::vector<AbstractPlayer>& players)
			{
				for (const auto& p : players)
				{
					if (p.getCurrentVehicleModel().isValidModel())
					{
						if (processSend(p)) // Delay; it helps obfuscate the sender if the target isn't using Stand.
						{
							const auto pos = p.getPos();

							if (p != g_player)
							{
								CExplosionManager::CExplosionArgs args{ EXP_TAG_VEHICLEMINE_SLICK, pos };

								args.m_bMakeSound = false;
								args.m_bNoFx = true;
								args.m_sizeScale = 1.0f;
								args.m_bNoDamage = true;
								args.m_fCamShake = 0.0f;

								if (auto veh = p.getVehicle(); veh.isValid())
								{
									args.m_bAttachedToVehicle = true;
									args.m_pAttachEntity = (CEntity*)veh.getPointer();
								}

								pointers::CExplosionEvent_Trigger(&args, nullptr);
							}
							else
							{
								FIRE::ADD_EXPLOSION(pos.x, pos.y, pos.z, EXP_TAG_VEHICLEMINE_SLICK, 1.0f, FALSE, TRUE, 0.0f, TRUE);
							}
						}
					}
				}
			});
		}
	};
}
