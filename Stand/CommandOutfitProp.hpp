#pragma once

#include "CommandSlider.hpp"

#include "outfit_data.hpp"

namespace Stand
{
	class CommandOutfitProp : public CommandSlider
	{
	public:
		const PedProp* const data;

		explicit CommandOutfitProp(CommandList* const parent, const PedProp* const data)
			: CommandSlider(parent, LOC_RT(data->menu_name), { data->command_name }, NOLABEL, -1, 0, -1, 1, (CMDFLAGS_SLIDER & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS) | CMDFLAG_FEATURELIST_FINISHLIST), data(data)
		{
		}

		void onChange(Click& click, int prev_value) final;

		void applyDefaultState() final
		{
		}
	};
}
