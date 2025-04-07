#pragma once

#include "CommandListSelect.hpp"

#include "DlcMgr.hpp"

namespace Stand
{
	class CommandPreOrderBonusOverride : public CommandListSelect
	{
	public:
		explicit CommandPreOrderBonusOverride(CommandList* parent)
			: CommandListSelect(parent, LOC("OVRDPROBNS"), CMDNAMES("preorder"), NOLABEL, {
				{0, LOC("OVRDOFF"), CMDNAMES("default")},
				{2, LOC("JUSTON"), CMDNAMES("on")},
				{1, LOC("OFF"), CMDNAMES("off")},
			}, 0)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			DlcMgr::preorder_bonus_override = (uint8_t)value;
			DlcMgr::update();
		}
	};
}
