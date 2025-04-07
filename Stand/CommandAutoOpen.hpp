#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"
#include "Worker.hpp"

namespace Stand
{
	class CommandAutoOpen : public CommandToggle
	{
	public:
		inline static bool did_it = false;

		explicit CommandAutoOpen(CommandList* const parent)
			: CommandToggle(parent, LOC("INJOPEN2"))
		{
		}

		void onEnable(Click& click) final
		{
			if (!did_it)
			{
				if (click.type == CLICK_BULK)
				{
					Worker::queueJob([]
					{
						if (!g_gui.opened)
						{
							g_gui.open(TC_WORKER);
						}
					});
				}
				did_it = true;
			}
		}
	};
}
