#pragma once

#include "CommandInputText.hpp"
#include "CommandStatBase.hpp"

namespace Stand
{
	class CommandStatString : public CommandStatBase<CommandInputText, std::string>
	{
	public:
		explicit CommandStatString(CommandList* const parent, Stat& stat)
			: CommandStatBase<CommandInputText, std::string>(parent, stat)
		{
		}

		void onCommandInner(Click& click, std::wstring& args) final override
		{
			setValue(StringUtils::utf16_to_utf8(args), CLICK_AUTO);
			update_state(CLICK_AUTO);

			ensureScriptThread(click, [this]
			{
				setStat(getStatHash());
				onUpdate();
			});

			args.clear();
		}

	protected:
		void getStat(Hash stat, std::string* value) final
		{
			std::string str = {};

			if (this->stat.type == Stats::USERID)
			{
				str = STATS::STAT_GET_USER_ID(stat);
			}
			else
			{
				str = STATS::STAT_GET_STRING(stat, -1);
			}

			*value = str;
		}

		void setStat(Hash stat) final
		{
			const auto cstr = this->value.c_str();

			if (this->stat.type == Stats::USERID)
			{
				STATS::STAT_SET_USER_ID(stat, cstr, TRUE);
			}
			else
			{
				STATS::STAT_SET_STRING(stat, cstr, TRUE);
			}
		}
	};
}