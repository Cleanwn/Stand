#pragma once

#include "CommandSpoof.hpp"
#include "CommandSliderFloat.hpp"

#include "as_fraction.hpp"

namespace Stand
{
	class CommandSpoofKd : public CommandSpoof<CommandSliderFloat>
	{
	private:
		int kills;
		int deaths;

	public:
		explicit CommandSpoofKd(CommandList* parent)
			: CommandSpoof(parent, LOC("SPFKD"), LOC("SPFDKD"), { CMDNAME("kd") })
		{
		}

		void onToggle(bool on) const final
		{
			g_hooking.spoof_kd = on;
		}

		void onValueChange() final
		{
			g_hooking.spoofed_kd = slider->getFloatValue();
			auto res = as_fraction(g_hooking.spoofed_kd);
			kills = res.first;
			deaths = res.second;
		}

		void onTick() const final
		{
			*ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(g_player, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_KD).as<float*>() = slider->getFloatValue();
			*ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(g_player, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_KILLS).as<int*>() = kills;
			*ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(g_player, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_DEATHS).as<int*>() = deaths;
		}
	};
}
