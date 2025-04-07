#pragma once

#include "CommandActionName.hpp"

#include "FiberPool.hpp"
#include "Gui.hpp"
#include "natives.hpp"
#include "ScAccount.hpp"

namespace Stand
{
	class CommandNameProfile : public CommandActionName
	{
	public:
		explicit CommandNameProfile(CommandList* const parent)
			: CommandActionName(parent, LOC("PRFL_N"), CMDNAMES_OBF("nameprofile"))
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (ScAccount::name2rid(click, StringUtils::utf16_to_utf8(get_next_arg(args)), [](const ScAccount& a)
			{
				const int64_t rid = a.rid;
				FiberPool::queueJob([rid]
				{
					std::string ridstr = fmt::to_string(rid);
					int network_handle[13 * 2] = { 0 };
					NETWORK::NETWORK_HANDLE_FROM_MEMBER_ID(ridstr.c_str(), (Any*)network_handle, 13);
					g_gui.prepareForOverlay();
					NETWORK::NETWORK_SHOW_PROFILE_UI((Any*)network_handle);
				});
			}))
			{
				return onClick(click);
			}
		}
	};
}
