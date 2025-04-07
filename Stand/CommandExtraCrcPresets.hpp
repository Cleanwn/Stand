#pragma once

#include "CommandListAction.hpp"

#include "CommandListActionItem.hpp"

namespace Stand
{
	class CommandExtraCrcPresets : public CommandListAction
	{
	private:
		CommandSpoofedExtraCrc* const spoofed_crc;

	public:
		explicit CommandExtraCrcPresets(CommandList* const parent, CommandSpoofedExtraCrc* spoofed_crc)
			: CommandListAction(parent, LOC("XTRCRC_PRSTS"), {}, NOLABEL, {
				//{0, LOC("XTRCRC_PRSTS_0")},
				{1, LIT_OBF("Stand")},
				{2, LIT_OBF("2Take1")},
				{3, LIT_OBF("Cherax")},
			}), spoofed_crc(spoofed_crc)
		{
		}

		void onItemClick(Click& click, CommandListActionItem* item) final
		{
			CommandListAction::onItemClick(click, item);
			switch (item->value)
			{
			/*case 0:
				spoofed_crc->setCrc(click, g_hooking.legit_extra_content_crc);
				break;*/

			case 1:
				spoofed_crc->setCrc(click, 0x57498);
				break;

			case 2:
				spoofed_crc->setCrc(click, 0x271271);
				break;

			case 3:
				spoofed_crc->setCrc(click, 0x43485258); // 'CHRX'
				break;
			}
		}
	};
}
