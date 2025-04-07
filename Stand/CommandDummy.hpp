#pragma once

#include "CommandIssuable.hpp"

namespace Stand
{
	// Things like "weather off" would be interpreted as "weather; off[theradar]", so make "off" a no-op.
	class CommandDummy : public CommandIssuable
	{
	public:
		explicit CommandDummy(CommandList* parent)
			: CommandIssuable(parent, { CMDNAME("off") })
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			click.setGenericResponse(LIT("Intentional no-op."));
		}
	};
}
