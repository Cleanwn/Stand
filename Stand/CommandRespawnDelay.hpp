#pragma once

#include "CommandSliderFloat.hpp"

#include "ScriptGlobal.hpp"

namespace Stand
{
	class CommandRespawnDelay : public CommandSliderFloat
	{
	private:
		bool running = false;

	public:
		explicit CommandRespawnDelay(CommandList* const parent)
			: CommandSliderFloat(parent, LOC("RESPWDLY"), CMDNAMES_OBF("respawndelay"), LOC("RESPWDLY_H"), 0, 500, 0, 50)
		{
		}

		std::wstring formatNumber(int num, bool allow_replacements) const final
		{
			if (allow_replacements)
			{
				if (num == 0)
				{
					return LANG_GET_W("OVRDOFF");
				}
			}

			return CommandSliderFloat::formatNumber(num, allow_replacements);
		}

		void onChange(Click& click, int prev_value) final
		{
			const auto fvalue = getFloatValue();
			if (fvalue == 0)
			{
				return;
			}
			registerScriptTickEventHandler([this, fvalue]
			{
				HANDLER_CHECK(getFloatValue() == fvalue);
				if (!running
					&& is_session_started()
					&& g_player_ped.isDead()
					)
				{
					running = true;
					const auto deadline = get_current_time_millis() + (fvalue * 1000.0f);
					FiberPool::queueJob([this, deadline]
					{
						while (!IS_DEADLINE_REACHED(deadline))
						{
							Script::current()->yield();
						}
						if (g_player_ped.isDead()) // Still dead?
						{
							*ScriptGlobal(GLOBAL_RESPAWN_DATA).at(GLOBAL_RESPAWN_BITSET).as<int*>() |= 1 << 1;
						}
						running = false;
					});
				}
				HANDLER_END;
			});
		}
	};
}