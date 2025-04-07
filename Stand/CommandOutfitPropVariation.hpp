#pragma once

#include "CommandSlider.hpp"

#include "outfit_data.hpp"

namespace Stand
{
	class CommandOutfitPropVariation : public CommandSlider
	{
	private:
		const PedProp* const data;

	public:
		explicit CommandOutfitPropVariation(CommandList* const parent, const PedProp* const data)
			: CommandSlider(parent, LOC_RT(data->variation_menu_name), { data->variation_command_name }, NOLABEL, -1, 0, -1, 1, (CMDFLAGS_SLIDER & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS) | CMDFLAG_FEATURELIST_FINISHLIST), data(data)
		{
		}

		void onChange(Click& click, int prev_value) final;

		void onDrawableChange();
		void updateRange();

		void applyDefaultState() final
		{
		}
	};
}
