#pragma once

#include "CommandSlider.hpp"

#include "natives.hpp"
#include "FiberPool.hpp"

namespace Stand
{
	class CommandFakeWanted : public CommandSlider
	{
	public:
		explicit CommandFakeWanted(CommandList* const parent)
			: CommandSlider(parent, LOC("FKWNTD"), { CMDNAME("fakewantedlevel") }, LOC("FKWNTD_H"), 0, 6, 0)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			ensureScriptThread(click, [=]
			{
				const int value = this->value;
				MISC::SET_FAKE_WANTED_LEVEL(value);
				if (value > 0)
				{
					registerScriptTickEventHandler(TC_SCRIPT_NOYIELD, [=]()
					{
						if (value != this->value)
						{
							return false;
						}
						MISC::SET_FAKE_WANTED_LEVEL(value);
						return true;
					});
				}
			});
		}
	};
}
