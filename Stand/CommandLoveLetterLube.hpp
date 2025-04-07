#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandLoveLetterLube : public CommandListSelect
	{
	public:
		explicit CommandLoveLetterLube(CommandList* const parent)
			: CommandListSelect(parent, LOC("LLLUBE"), {}, LOC("LLLUBE_H"), {
				{ 2, LOC("DOFF") },
				{ 1, LOC("LLLUBE_1"), {}, LOC("LLLUBE_1_H") },
				{ 0, LOC("SPFDHTPRST_2"), {}, LOC("LLLUBE_0_H") },
			}, 1)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			g_hooking.ll_lube = (uint32_t)this->value;
		}

		void setState(Click& click, const std::string& state) final
		{
			if (state == "On")
			{
				return CommandListSelect::setState(click, soup::ObfusString("Aggressive").str());
			}
			return CommandListSelect::setState(click, state);
		}
	};
}
