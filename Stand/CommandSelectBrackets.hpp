#pragma once

#include "CommandListSelect.hpp"

#include "CommandPlayer.hpp"

namespace Stand
{
	class CommandSelectBrackets : public CommandListSelect
	{
	public:
		explicit CommandSelectBrackets(CommandList* const parent)
			: CommandListSelect(parent, LOC("BRCKTS"), {}, NOLABEL, {
				{0, LIT("STD")},
				{1, LIT("[STD]")},
				{2, LIT("(STD)")},
				{3, LIT("{STD}")},
				{4, LIT("<STD>")},
			}, 1)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			switch (value)
			{
			case 0:
				CommandPlayer::flag_prefix = L" ";
				CommandPlayer::flag_suffix = L"";
				break;

			default:
				CommandPlayer::flag_prefix = L" [";
				CommandPlayer::flag_suffix = L"]";
				break;

			case 2:
				CommandPlayer::flag_prefix = L" (";
				CommandPlayer::flag_suffix = L")";
				break;

			case 3:
				CommandPlayer::flag_prefix = L" {";
				CommandPlayer::flag_suffix = L"}";
				break;

			case 4:
				CommandPlayer::flag_prefix = L" <";
				CommandPlayer::flag_suffix = L">";
				break;
			}
			CommandPlayer::force_flag_update = true;
		}
	};
}
