#pragma once

#include "CommandActionName.hpp"

#include "FiberPool.hpp"
#include "ScAccount.hpp"

namespace Stand
{
	class CommandNameInvite : public CommandActionName
	{
	public:
		explicit CommandNameInvite(CommandList* const parent)
			: CommandActionName(parent, LOC("INV_N"), CMDNAMES_OBF("invite", "nameinvite"))
		{
		}

		void onClick(Click& click) final
		{
			if (JoinUtil::inviteViaRidPreflightCheck(click))
			{
				return CommandActionName::onClick(click);
			}
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (JoinUtil::inviteViaRidPreflightCheck(click))
			{
				if (ScAccount::name2rid(click, StringUtils::utf16_to_utf8(get_next_arg(args)), [](const ScAccount& a)
				{
					const int64_t rid = a.rid;
					FiberPool::queueJob([rid]
					{
						JoinUtil::inviteViaRid(rid);
					});
				}))
				{
					return onClick(click);
				}
			}
		}
	};
}
