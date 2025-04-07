#pragma once

#include "CommandAction.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandRidProfile : public CommandAction
	{
	public:
		explicit CommandRidProfile(CommandList* const parent)
			: CommandAction(parent, LOC("PRFL_R"), CMDNAMES_OBF("ridprofile"))
		{
		}

		std::wstring getCommandSyntax() const final
		{
			return std::move(LANG_GET_W("CMD").append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" [rid]"));
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			auto arg = get_next_arg(args);
			if (arg.empty())
			{
				return onClick(click);
			}
			auto arg_utf8 = StringUtils::utf16_to_utf8(arg);
			ensureScriptThread(click, [arg_utf8{ std::move(arg_utf8) }]
			{
				int network_handle[13 * 2] = { 0 };
				NETWORK::NETWORK_HANDLE_FROM_MEMBER_ID(arg_utf8.c_str(), (Any*)network_handle, 13);
				g_gui.prepareForOverlay();
				NETWORK::NETWORK_SHOW_PROFILE_UI((Any*)network_handle);
			});
		}
	};
}
