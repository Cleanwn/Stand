#pragma once

#include "CommandListSelect.hpp"

#include "CommandPosition2d.hpp"
#include "GameToaster.hpp"
#include "GridToaster.hpp"
#include "NotifyGrid.hpp"

namespace Stand
{
	class CommandNotifyType : public CommandListSelect
	{
	public:
		CommandPosition2d* custom_pos;

		explicit CommandNotifyType(CommandList* parent)
			: CommandListSelect(parent, LOC("TYP"), {}, NOLABEL, {
				{0, LOC("GME")},
				{1, LOC("STNDNTF_M")},
				{2, LOC("STNDNTF_C")},
			}, 1)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (g_gui.isUnloadPending())
			{
				return;
			}
			if (value == 0)
			{
				((CommandListNotifySettings*)parent)->setToaster(click.thread_context, &g_game_toaster);
			}
			else
			{
				((CommandListNotifySettings*)parent)->setToaster(click.thread_context, &g_grid_toaster);
				g_notify_grid.next_to_map = (value == 1);
				if (g_notify_grid.next_to_map)
				{
					g_notify_grid.origin.y = 1060;
					g_notify_grid.setOriginNextToMap();
					g_notify_grid.update();
				}
				else
				{
					g_notify_grid.origin = custom_pos->getPosition();

					if (g_notify_grid.invert_flow)
					{
						// Invert flow becomes active, update whole grid
						g_notify_grid.update();
					}
					else
					{
						g_notify_grid.updatePositions();
					}
				}
			}
		}
	};
}
