#pragma once

#include "CommandInput.hpp"

#include "CommandExtraInfo.hpp"
#include "ScriptGlobal.hpp"
#include "StatUtil.hpp"

namespace Stand
{
	class CommandCeoName : public CommandInput
	{
	public:
		explicit CommandCeoName(CommandList* parent)
			: CommandInput(parent, LOC("NME"), { CMDNAME("ceoname"), CMDNAME("mcname") }, NOLABEL, {}, CMDFLAGS_ACTION_VALUE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS)
		{
		}

		void onClick(Click& click) final
		{
			if (click.isBoss())
			{
				CommandInput::onClick(click);
			}
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (click.isBoss()
				&& checkArgsLength(click, args, 62)
				)
			{
				auto arg = StringUtils::utf16_to_utf8(args);
				StatUtil::setDualString(g_player.isInMc() ? "mc_gang_name" : "gb_gang_name", arg);
				g_player.setOrgName(arg.c_str());
			}
			args.clear();
		}

		void onTickInGameViewport() final
		{
			if (*pointers::is_session_started)
			{
				setValueGta(ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_NAME).as<const char*>());
			}
			else
			{
				setValue(std::string());
			}
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}

		void getExtraInfo(CommandExtraInfo& info, std::wstring& args) final
		{
			CommandInput::getExtraInfo(info, args);
			info.char_limit = 62;
			info.colour_selector = true;
		}
	};
}
