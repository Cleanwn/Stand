#pragma once

#include "CommandTextslider.hpp"

#include "yeet_flow.hpp"

namespace Stand
{
	class CommandYeet : public CommandTextslider
	{
	public:
		explicit CommandYeet(CommandList* const parent)
			: CommandTextslider(parent, LIT_OBF("YEET"), CMDNAMES_OBF("yeet"), NOLABEL, {
				{ 0, LOC("PLYCRSH_E") },
				{ 1, LOC("LGCY") },
			})
		{
		}

		void onClick(Click& click) final
		{
			showWarning(click, LANG_GET_W("YEET_W"), 0, [value{ value }](ThreadContext tc)
			{
				g_yeeting_with_abort = (bool)value;
				yeet_flow(tc);
			});
		}
	};
}
