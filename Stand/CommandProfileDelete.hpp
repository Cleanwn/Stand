#pragma once

#include "CommandAction.hpp"

#include <filesystem>

#include <fmt/format.h>

#include "get_appdata_path.hpp"
#include "lang.hpp"

namespace Stand
{
	class CommandProfileDelete : public CommandAction
	{
	public:
		explicit CommandProfileDelete(CommandList* const parent)
			: CommandAction(parent, LOC("DEL"), {}, NOLABEL)
		{
		}

		void onClick(Click& click) final
		{
			showWarning(click, getGenericIrreversableWarningLabel(), WARNING_COMMAND_BOUND, [this, click](ThreadContext ctx)
			{
				const std::wstring name = parent->menu_name.getLiteralUtf16();

				std::wstring path = get_appdata_path();
				path.append(parent->as<CommandProfile>()->getConfigName());
				path.append(L".txt");

				std::error_code err;
				if (std::filesystem::remove(path, err))
				{
					parent->beGone();
					click.setResponse(LIT(LANG_FMT_W("DELPRFL_S", name)));
				}
				else
				{
					click.setResponse(LIT(LANG_FMT_W("DELPRFL_F", name)));
				}
			});
		}
	};
}
