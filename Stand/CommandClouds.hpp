#pragma once

namespace Stand
{
	class CommandClouds : public CommandListSelect
	{
	public:
		explicit CommandClouds(CommandList* const parent)
			: CommandListSelect(parent, LOC("WTR_CLOUDS"), CMDNAMES("clouds"), LOC("GEN_H_LOC"), {
				{0, LOC("RIGOFF"), CMDNAMES("normal")},
				{reinterpret_cast<long long>("altostratus"), LIT("altostratus"), CMDNAMES("altostratus")},
				{reinterpret_cast<long long>("Cirrus"), LIT("Cirrus"), CMDNAMES("cirrus")},
				{reinterpret_cast<long long>("cirrocumulus"), LIT("cirrocumulus"), CMDNAMES("cirrocumulus")},
				{reinterpret_cast<long long>("Clear 01"), LIT("Clear 01"), CMDNAMES("clear01")},
				{reinterpret_cast<long long>("Cloudy 01"), LIT("Cloudy 01"), CMDNAMES("cloudy01")},
				{reinterpret_cast<long long>("Contrails"), LIT("Contrails"), CMDNAMES("contrails")},
				{reinterpret_cast<long long>("Horizon"), LIT("Horizon"), CMDNAMES("horizon")},
				{reinterpret_cast<long long>("horizonband1"), LIT("horizonband1"), CMDNAMES("horizonband1")},
				{reinterpret_cast<long long>("horizonband2"), LIT("horizonband2"), CMDNAMES("horizonband2")},
				{reinterpret_cast<long long>("horizonband3"), LIT("horizonband3"), CMDNAMES("horizonband3")},
				{reinterpret_cast<long long>("horsey"), LIT("horsey"), CMDNAMES("horsey")},
				{reinterpret_cast<long long>("Nimbus"), LIT("Nimbus"), CMDNAMES("nimbus")},
				{reinterpret_cast<long long>("Puffs"), LIT("Puffs"), CMDNAMES("puffs")},
				{reinterpret_cast<long long>("RAIN"), LIT("RAIN"), CMDNAMES("rain")},
				{reinterpret_cast<long long>("Snowy 01"), LIT("Snowy 01"), CMDNAMES("snowy01")},
				{reinterpret_cast<long long>("Stormy 01"), LIT("Stormy 01"), CMDNAMES("stormy01")},
				{reinterpret_cast<long long>("stratoscumulus"), LIT("stratoscumulus"), CMDNAMES("stratoscumulus")},
				{reinterpret_cast<long long>("Stripey"), LIT("Stripey"), CMDNAMES("stripey")},
				{reinterpret_cast<long long>("shower"), LIT("shower"), CMDNAMES("shower")},
				{reinterpret_cast<long long>("Wispy"), LIT("Wispy"), CMDNAMES("wispy")},
			}, 0, CMDFLAGS_LIST_SELECT, CMDFLAG_FEATURELIST_FINISHLIST)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (value != 0)
			{
				auto value = this->value;
				registerScriptTickEventHandler(click, [this, value]
				{
					if (this->value == value)
					{
						MISC::LOAD_CLOUD_HAT(reinterpret_cast<const char*>(value), 0.0f);
						return true;
					}
					MISC::UNLOAD_CLOUD_HAT(reinterpret_cast<const char*>(value), 0.0f);
					return false;
				});
			}
		}
	};
}
