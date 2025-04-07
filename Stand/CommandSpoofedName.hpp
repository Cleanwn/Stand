#pragma once

#include "CommandInput.hpp"

#include "apply_spoofed_name.hpp"
#include "CommandExtraInfo.hpp"
#include "get_next_arg.hpp"
#include "Hooking.hpp"
#include "pointers.hpp"
#include "rlGamerInfo.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandSpoofedName : public CommandInput
	{
	public:
		inline static CommandSpoofedName* instance = nullptr;

		explicit CommandSpoofedName(CommandList* const parent)
			: CommandInput(parent, LOC("SPFDNME"), CMDNAMES("spoofedname"), NOLABEL, pointers::rlPresence_GamerPresences_0_GamerInfo->name)
		{
			instance = this;
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (checkArgsLength(click, args, 16))
			{
				setName(click, StringUtils::utf16_to_utf8(args));
			}
			args.clear();
		}

		void setName(Click& click, std::string name)
		{
			if (!g_hooking.isSpoofedNameAllowed(name))
			{
				name = "Sussy_Imposter";
			}
			setValueGta(name, click.type);
			g_hooking.spoofed_name = name;
			if (g_hooking.spoof_name)
			{
				apply_spoofed_name(g_hooking.spoofed_name.c_str());
				on_name_spoofing_update();
				//click.setSpoofUpdate();
			}
		}

		void getExtraInfo(CommandExtraInfo& info, std::wstring& args) final
		{
			CommandInput::getExtraInfo(info, args);
			info.char_limit = 16;
			info.colour_selector = true;
		}
	};
}
