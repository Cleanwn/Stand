#pragma once

#include "CommandSliderFloat.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandLodscale : public CommandSliderFloat
	{
	public:
		explicit CommandLodscale(CommandList* const parent)
			: CommandSliderFloat(parent, LOC("DSTSCLMUL"), { CMDNAME("lodscale") }, NOLABEL, 0, 20000, 100, 100)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			auto val = this->value;
			if (val != default_value)
			{
				registerScriptTickEventHandler(click, [this, val]
				{
					HANDLER_CHECK(this->value == val);
						STREAMING::OVERRIDE_LODSCALE_THIS_FRAME(getFloatValue());
					HANDLER_END;
				});
			}
			else
			{
				ensureScriptThread(click, []
				{
					STREAMING::OVERRIDE_LODSCALE_THIS_FRAME(1.0f);
				});
			}
		}
	};
}
