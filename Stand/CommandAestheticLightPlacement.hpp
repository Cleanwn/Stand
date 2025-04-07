#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandAestheticLightPlacement : public CommandListSelect
	{
	public:
		explicit CommandAestheticLightPlacement(CommandList* const parent)
			: CommandListSelect(parent, LOC("PLCMNT"), {}, NOLABEL, {
				{0, LOC("PLCMNT_CHR"), {}, LOC("PLCMNT_CHR_H")},
				{1, LOC("PLCMNT_CAM"), {}, LOC("PLCMNT_CAM_H")},
				{2, LOC("PLCMNT_OVL"), {}, LOC("PLCMNT_OVL_H")},
			}, 0)
		{
		}
	};
}
