#pragma once

#include "CommandListAction.hpp"

#include "CommandListActionItem.hpp"

namespace Stand
{
	class CommandSampleToast : public CommandListAction
	{
	public:
		explicit CommandSampleToast(CommandList* const parent)
			: CommandListAction(parent, LOC("SAMPNOT"), {}, NOLABEL, {
				{0, LOC("SIZE_S")},
				{1, LOC("SIZE_M")},
				{2, LOC("SIZE_L")},
			})
		{
		}

		void onItemClick(Click& click, CommandListActionItem* item) final
		{
			CommandListAction::onItemClick(click, item);

			switch (item->value)
			{
			case 0: // Small
				g_grid_toaster.toast(LANG_GET_W("HI"));
				break;

			case 1: // Medium
				g_grid_toaster.toast(LANG_GET_W("FREE_T"));
				break;

			case 2: // Large
				g_grid_toaster.toast(Util::getWotString().getLocalisedUtf16());
				break;

			default:
				SOUP_UNREACHABLE;
			}
		}
	};
}
