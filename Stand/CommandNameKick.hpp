#pragma once

#include "CommandAction.hpp"

#include "get_next_arg.hpp"
#include "RemoteGamerConnector.hpp"

namespace Stand
{
	class CommandNameKick : public CommandAction
	{
	public:
		explicit CommandNameKick(CommandList* const parent)
			: CommandAction(parent, LOC("NAMKICK"), CMDNAMES_OBF("namekick"), NOLABEL, CMDFLAG_FEATURELIST_ULTIMATE)
		{
		}

		std::wstring getCommandSyntax() const final
		{
			return std::move(LANG_GET_W("CMD").append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" ").append(LANG_GET_W("ARGNME")));
		}

		void onClick(Click& click) final
		{
			if (click.isUltimateEdition())
			{
				CommandAction::onClick(click);
			}
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (click.isUltimateEdition() && ScAccount::name2rid(click, StringUtils::utf16_to_utf8(get_next_arg(args)), [this](const ScAccount& a)
			{
				auto rid = a.rid;
				FiberPool::queueJob([rid]
				{
					RemoteGamerConnector::kickViaRid(rid);
				});
			}))
			{
				return onClick(click);
			}
		}
	};
}
