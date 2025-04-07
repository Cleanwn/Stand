#pragma once

#include "CommandHandlingFlag.hpp"

#include "CHandlingData.hpp"

namespace Stand
{
	class CommandSmoothCoasting : public CommandHandlingFlag<CHandlingData>
	{
	public:
		explicit CommandSmoothCoasting(CommandList* const parent)
			: CommandHandlingFlag<CHandlingData>(parent, LOC("SMTHCST"), CMDNAMES_OBF("smoothcoasting"), LOC("SMTHCST_H"), &CHandlingData::m_handling_flags, 1 << 8) // HF_FREEWHEEL_NO_GAS
		{
		}
	};
}