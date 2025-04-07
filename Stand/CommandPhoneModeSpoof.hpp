#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	enum PhoneMode
	{
		Mode_None,
		Mode_ToCall,
		Mode_ToText,
		Mode_ToCamera,
		Mode_Max,
	};

	class CommandPhoneModeSpoof : public CommandListSelect
	{
	public:
		explicit CommandPhoneModeSpoof(CommandList* const parent)
			: CommandListSelect(parent, LOC("PHNSPF"), {}, NOLABEL, {
				{ -1, LOC("DOFF") },
				{ Mode_None, LOC("PHNSPF_0") },
				{ Mode_ToCall, LOC("PHNSPF_1") },
				{ Mode_ToText, LOC("PHNSPF_2") },
				{ Mode_ToCamera, LOC("PHNSPF_3") },
			}, -1)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			g_hooking.phone_mode_spoof = static_cast<uint32_t>(value);
		}
	};
}
