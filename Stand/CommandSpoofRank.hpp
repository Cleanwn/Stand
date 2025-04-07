#pragma once

#include "CommandSpoof.hpp"

#include "AbstractPlayer.hpp"
#include "ScriptGlobal.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandSpoofRank : public CommandSpoof<CommandSlider>
	{
	private:
		int rp;

	public:
		explicit CommandSpoofRank(CommandList* const parent)
			: CommandSpoof(parent, LOC("SPFRNK_D"), LOC("SPFDRNK"), CMDNAMES("rank", "level"), LOC("SPFRNK_H"))
		{
		}

		void onToggle(bool on) const final
		{
			g_hooking.spoof_rank = on;
		}

		void onValueChange() final
		{
			g_hooking.spoofed_rank = (float)slider->value;
			rp = Util::get_rp_required_for_rank(slider->value);
		}

		void onTick() const final
		{
			*ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(g_player, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_RANK).as<int*>() = slider->value;
			*ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(g_player, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_RP).as<int*>() = rp;
		}
	};
}
