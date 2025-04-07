#pragma once

#include "CommandAction.hpp"

#include "Util.hpp"

namespace Stand
{
	class CommandSessionCodeLink : public CommandAction
	{
	public:
		explicit CommandSessionCodeLink(CommandList* parent)
			: CommandAction(parent, LOC("CPYINVL"))
		{
		}

		void onClick(Click& click) final
		{
			auto opt = parent->as<CommandSessionCode>()->getSessionCode();
			if (opt.has_value())
			{
				std::string link(soup::ObfusString("https://standjo.in/").str());
				link.append(opt.value());
				Util::copy_to_clipboard_utf8(click, std::move(link));
			}
			else
			{
				click.uwotm8();
			}
		}
	};
}
