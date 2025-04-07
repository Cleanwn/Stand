#include "CommandActionModel.hpp"

#include <fmt/core.h>

#include "get_next_arg.hpp"
#include "lang.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandActionModel::CommandActionModel(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const commandflags_t flags, const CommandPerm perm)
		: CommandAction(parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags, perm)
	{
		mustHaveCommandName();
	}

	std::wstring CommandActionModel::getCommandSyntax() const
	{
		std::wstring syntax{ LANG_GET_W("CMD") };
		syntax.append(L": ");
		syntax.append(cmdNameToUtf16(command_names.at(0)));
		syntax.push_back(L' ');
		syntax.append(LANG_GET_W("ARGMDLNME"));
		return syntax;
	}

	void CommandActionModel::onCommand(Click& click, std::wstring& args)
	{
		auto _arg = get_next_arg(args);
		if (_arg.empty())
		{
			return onClick(click);
		}
		click.setNoResponse();
		ensureYieldableScriptThread(click, [this, click, _arg{ std::move(_arg) }]() mutable
		{
			auto arg = StringUtils::utf16_to_utf8(_arg);
			StringUtils::to_lower(arg);
			AbstractModel model{ rage::atStringHash(arg) };
			Label name = LIT(arg);
			if (spawnIfValid(model, click, name))
			{
				return;
			}
			if (arg.substr(0, 2) == "0x")
			{
				arg.erase(0, 2);
				if (auto res = soup::string::hexToInt<uhash_t>(arg); res.has_value())
				{
					model = res.value();
					if (spawnIfValid(model, click, name))
					{
						return;
					}
				}
			}
			else
			{
				if (auto res = soup::string::toInt<hash_t>(arg); res.has_value())
				{
					model = res.value();
					if (spawnIfValid(model, click, name))
					{
						return;
					}
				}
			}
			click.setResponse(LIT(fmt::format(fmt::runtime(Lang::get(getInvalidModelMessage())), arg)));
			std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
			prefill.push_back(L' ');
			prefill.append(_arg);
			click.showCommandBoxIfPossible(std::move(prefill));
		});
	}

	hash_t CommandActionModel::getInvalidModelMessage() const
	{
		return ATSTRINGHASH("INPTMDL_E");
	}
}
