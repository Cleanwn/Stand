#pragma once

#include "CommandAction.hpp"

#include "JoinUtil.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandGsinfoJoin : public CommandAction
	{
	public:
		explicit CommandGsinfoJoin(CommandList* const parent)
			: CommandAction(parent, LIT("[Debug Build] Join Session by gsinfo"), CMDNAMES("gsinfojoin", "gsjoin"), NOLABEL, CMDFLAGS_ACTION | CMDFLAG_FEATURELIST_SKIP)
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			JoinUtil::join(StringUtils::utf16_to_utf8(args));
			args.clear();
		}
	};
}
