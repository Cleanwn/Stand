#pragma once

#include "CommandListSelect.hpp"

#include "Hooking.hpp"

namespace Stand
{
	class CommandJoinGroupOverride : public CommandListSelect
	{
	public:
		explicit CommandJoinGroupOverride(CommandList* const parent)
			: CommandListSelect(parent, LOC("JGRPOVRD"), {}, LOC("JGRPOVRD_H"), {
				{ -1, LOC("OVRDOFF") },
				{ 4, LOC("JGRPOVRD_0") },
				{ 0, LOC("JGRPOVRD_1") },
			}, -1)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			g_hooking.override_mm_group = static_cast<uint32_t>(static_cast<int32_t>(value));
		}
	};
}
