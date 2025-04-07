#pragma once

#include "CommandToggle.hpp"

#include "script_thread.hpp"

namespace Stand
{
	class CommandGhostNearbyDrivers : public CommandToggle
	{
	private:
		[[nodiscard]] static Vector3 getBonePos(AbstractEntity& veh, const char* bone)
		{
			return ENTITY::GET_ENTITY_BONE_POSTION(veh, ENTITY::GET_ENTITY_BONE_INDEX_BY_NAME(veh, bone));
		}

		[[nodiscard]] static bool isDangerous(AbstractEntity& veh) // Could do a standard dist check, but this is misleading for long vehicles. Keeping ghosting to a minimum is nice too.
		{
			// Measure distance from the two front wheels.
			{
				auto pos1 = getBonePos(veh, "wheel_lf");
				auto pos2 = getBonePos(veh, "wheel_rf");
				auto our1 = getBonePos(g_player_veh, "wheel_lf");
				auto our2 = getBonePos(g_player_veh, "wheel_rf");

				if (pos1.distance(our1) <= 6.0f
					|| pos2.distance(our2) <= 6.0f
					)
				{
					return true;
				}
			}

			// Measure distance from the two rear wheels.
			{
				auto pos1 = getBonePos(veh, "wheel_lr");
				auto pos2 = getBonePos(veh, "wheel_rr");
				auto our1 = getBonePos(g_player_veh, "wheel_lr");
				auto our2 = getBonePos(g_player_veh, "wheel_rr");

				if (pos1.distance(our1) <= 4.0f
					|| pos2.distance(our2) <= 4.0f
					)
				{
					return true;
				}
			}

			// Measure distance from their front wheels & our back wheels.
			{
				auto pos1 = getBonePos(veh, "wheel_lf");
				auto pos2 = getBonePos(veh, "wheel_rf");
				auto our1 = getBonePos(g_player_veh, "wheel_lr");
				auto our2 = getBonePos(g_player_veh, "wheel_rr");

				if (pos1.distance(our1) <= 6.0f
					|| pos2.distance(our2) <= 6.0f
					)
				{
					return true;
				}
			}

			// Measure distance from our front wheels & their back wheels.
			{
				auto pos1 = getBonePos(veh, "wheel_lr");
				auto pos2 = getBonePos(veh, "wheel_rr");
				auto our1 = getBonePos(g_player_veh, "wheel_lf");
				auto our2 = getBonePos(g_player_veh, "wheel_rf");

				if (pos1.distance(our1) <= 2.0f
					|| pos2.distance(our2) <= 2.0f
					)
				{
					return true;
				}
			}

			return false;
		}

	public:
		explicit CommandGhostNearbyDrivers(CommandList* const parent)
			: CommandToggle(parent, LOC("GHSTNRBYDR"), CMDNAMES_OBF("ghostnearbydrivers"), LOC("GHSTNRBYDR_H"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				HANDLER_CHECK(this->m_on);
				if (!is_session_freeroam()
					&& GtaThread::fromHash(ATSTRINGHASH("FM_Race_Controler"))
					)
				{
					if (g_player_veh.isValid())
					{
						for (const auto& p : AbstractPlayer::listExcept(true))
						{
							if (auto veh = p.getVehicle(); veh.isValid())
							{
								NETWORK::SET_REMOTE_PLAYER_AS_GHOST(p, isDangerous(veh));
							}
						}
					}
				}
				HANDLER_END;
			});
		}
	};
}