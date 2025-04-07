#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandHotkeyModkeyBehaviour : public CommandListSelect
	{
	public:
		explicit CommandHotkeyModkeyBehaviour(CommandList* const parent)
			: CommandListSelect(parent, LOC("HMB"), {}, LOC("HMB_H"), {
				{HMB_STRICT, LOC("STRCT"), {}, LOC("HMB_STRCT_H")},
				{HMB_STRICT_IF_HAS_MODKEY, LOC("SMRT"), {}, LOC("HMB_SMRT_H")},
				{HMB_LAX, LOC("LAX"), {}, LOC("HMB_LAX_H")},
			}, (long long)HMB_STRICT_IF_HAS_MODKEY)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			Hotkey::modkey_behaviour = (HotkeyModkeyBehaviour)value;
		}
	};
}
