#pragma once

#include "CommandInput.hpp"

#include "get_next_arg.hpp"
#include "lang.hpp"
#include "ScAccount.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandInputGamerName : public CommandInput
	{
	private:
		CommandGamer* const gamer;
		ScAccount* scaccount = nullptr;
		bool needs_completion = true;

	public:
		explicit CommandInputGamerName(CommandList* parent, std::string&& menu_name, const std::vector<CommandName>& command_names, CommandGamer* const gamer)
			: CommandInput(parent, LIT(std::move(menu_name.append(" - ").append(LANG_GET("NME")))), combineCommandNames(command_names, CMDNAME("name")), NOLABEL, {}, CMDFLAGS_ACTION_VALUE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS), gamer(gamer)
		{
		}

		void reset() noexcept
		{
			value.clear();
			scaccount = nullptr;
			needs_completion = true;
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (args.empty())
			{
				return onClick(click);
			}
			value = StringUtils::utf16_to_utf8(get_next_arg(args));
			update_state(click.type);
			ScAccount::name2rid(click, std::string(value), [this](const ScAccount& a)
			{
				if (a.hasRID())
				{
					gamer->setRid(a.rid);
				}
			});
		}
		
		void onTickInGameViewport() final
		{
			if (needs_completion)
			{
				if (scaccount == nullptr)
				{
					scaccount = ScAccount::fromRID(gamer->getRid());
					auto lazy_name = scaccount->getNameLazy();
					if (lazy_name == nullptr)
					{
						setValue(LANG_GET("GIP_Q"));
					}
					else
					{
						setValue(lazy_name);
					}
					scaccount->requestCompletion();
				}
				else
				{
					if (scaccount->isComplete())
					{
						needs_completion = false;
						if (scaccount->hasName())
						{
							setValue(std::string(scaccount->name));
						}
						else
						{
							setValue(LANG_GET("NA"));
						}
					}
				}
			}
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
