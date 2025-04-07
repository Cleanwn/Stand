#pragma once

#include "CommandInputText.hpp"

#include "CommandExtraInfo.hpp"
#include "fmt_arg.hpp"
#include "Gui.hpp"

namespace Stand
{
	class CommandInputTextLimitChars : public CommandInputText
	{
	private:
		int max_char;

	public:
		explicit CommandInputTextLimitChars(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, int max_char = 8, std::string&& default_value = {}, const commandflags_t flags = CMDFLAGS_ACTION_VALUE)
			: CommandInputText(parent, std::move(menu_name), std::move(command_names), std::move(help_text), std::move(default_value), flags), max_char(max_char)
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (args.length() > max_char)
			{
				click.setGenericResponse(LIT(LANG_FMT("CHARLIMIT", FMT_ARG("number", max_char))));
				g_gui.showCommandBox(std::move(std::wstring(cmdNameToUtf16(command_names.front())).append(L" ").append(args)), click);
				args.clear();
			}
			else
			{
				return onCommandInner(click, args);
			}
		}

		void getExtraInfo(CommandExtraInfo& info, std::wstring& args) final
		{
			CommandInputText::getExtraInfo(info, args);
			info.char_limit = max_char;
		}

	protected:
		virtual void onCommandInner(Click& click, std::wstring& args)
		{
			return CommandInput::onCommand(click, args);
		};
	};
}