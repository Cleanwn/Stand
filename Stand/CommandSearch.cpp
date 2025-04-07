#include "CommandSearch.hpp"

#include "CommandLink.hpp"
//#include "get_next_arg.hpp"
#include "lang.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandSearch::CommandSearch(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const search_flags_t search_flags, const commandflags_t flags)
		: CommandList(parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags, COMMAND_LIST_SEARCH), og_menu_name(this->menu_name), search_flags(search_flags)
	{
	}

	Label CommandSearch::getActivationName() const
	{
		if (menu_name.getLocalisationHash() == ATSTRINGHASH("SRCH"))
		{
			return getActivationNameImplCombineWithParent(": ");
		}
		return CommandList::getActivationName();
	}

	std::wstring CommandSearch::getCommandSyntax() const
	{
		std::wstring syntax{ LANG_GET_W("CMD") };
		syntax.append(L": ").append(cmdNameToUtf16(command_names.at(0))).push_back(L' ');
		syntax.append(LANG_GET_W("ARGCLU"));
		return syntax;
	}

	void CommandSearch::onClick(Click& click)
	{
		if (click.isWeb())
		{
			onClickImplUnavailable(click);
		}
		return CommandPhysical::onClick(click);
	}

	void CommandSearch::onCommand(Click& click, std::wstring& args)
	{
		std::string arg;
		/*if (search_flags & SEARCH_ARG_IS_SINGLE_WORD)
		{
			arg = StringUtils::utf16_to_utf8(get_next_arg(args));
		}
		else*/
		{
			arg = StringUtils::utf16_to_utf8(std::move(args));
			args.clear();
		}
		const bool search_arg_can_be_empty = (arg == "*");
		std::string search_arg{};
		if (!search_arg_can_be_empty)
		{
			search_arg = arg;
			if (search_flags & SEARCH_SIMPLIFIED)
			{
				StringUtils::simplifyCommandName(search_arg);
			}
			else if (search_flags & SEARCH_LOWER)
			{
				StringUtils::to_lower(search_arg);
			}
		}
		if (search_arg.empty()
			&& !search_arg_can_be_empty
			)
		{
			return onClick(click);
		}
		resetChildren();
		doSearch(std::move(search_arg));
		if (children.empty())
		{
			click.setResponse(LOC("SRCHNRES"));
			resetMenuName();
			processChildrenUpdate();
		}
		else
		{
			menu_name.setLiteral(std::move(std::string(og_menu_name.getLocalisedUtf8()).append(": ").append(arg)));
			active = false;
			open(click.thread_context);
			active = true;
		}
	}

	void CommandSearch::checkForMatchByMenuName(const std::string& arg, CommandPhysical* cmd)
	{
		auto name = std::string(cmd->menu_name.getEnglishUtf8());
		StringUtils::simplifyCommandName(name);
		if (name.find(arg) != std::string::npos)
		{
			createChild<CommandLink>(cmd, true);
		}
		else if (!cmd->menu_name.isLiteral() && Lang::activeIsLocalised())
		{
			auto name = std::string(cmd->menu_name.getLocalisedUtf8());
			StringUtils::simplifyCommandName(name);
			if (name.find(arg) != std::string::npos)
			{
				createChild<CommandLink>(cmd, true);
			}
		}
	}

	void CommandSearch::resetMenuName()
	{
		setMenuName(Label(og_menu_name));
	}

	void CommandSearch::onActiveListUpdate()
	{
		CommandList::onActiveListUpdate();

		if (active
			&& !isThisOrSublistActiveInMyTabMenu()
			)
		{
			active = false;
			resetMenuName();
			resetChildren();
		}
	}
}
