#pragma once

#include "CommandListAction.hpp"

#include "CommandListActionItem.hpp"

namespace Stand
{
	class CommandHostTokenPresets : public CommandListAction
	{
	private:
		CommandSpoofedHostToken* const spoofed_host_token;

	public:
		explicit CommandHostTokenPresets(CommandList* const parent, CommandSpoofedHostToken* spoofed_host_token)
			: CommandListAction(parent, LOC("PRSTS"), {}, NOLABEL, {
				{1, LOC("SPFDHTPRST_1")},
				{2, LOC("SPFDHTPRST_2")},
				{0, LOC("SPFDHTPRST_0")},
			}), spoofed_host_token(spoofed_host_token)
		{
		}

		void onItemClick(Click& click, CommandListActionItem* item) final
		{
			CommandListAction::onItemClick(click, item);
			switch (item->value)
			{
			case 0:
				spoofed_host_token->setHostToken(click, 0xFFFFFFFFFFFFFFFFi64 - soup::rand(0, 0xFFFF));
				break;

			case 1:
				spoofed_host_token->setHostToken(click,  soup::rand(9000000000000000i64, 10000000000000000i64));
				break;

			case 2:
				spoofed_host_token->setHostToken(click, soup::rand(0, 0xFF));
				break;
			}
		}
	};
}
