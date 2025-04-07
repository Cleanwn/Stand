#pragma once

#include "CommandInput.hpp"

namespace Stand
{
	class CommandSpoofedExtraCrc : public CommandInput
	{
	public:
		explicit CommandSpoofedExtraCrc(CommandList* const parent)
			: CommandInput(parent, LOC("XTRCRC"), CMDNAMES_OBF("extravalue"), LOC("XTRCRC_H"), "00057498")
		{
			//value = Util::to_padded_hex_string(hooks::CExtraContentManager_GetCRC(*pointers::extra_content_manager, 0));
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			auto arg = get_next_arg(args);
			if (arg.empty())
			{
				return onClick(click);
			}
			try
			{
				setCrc(click, std::stoul(arg, nullptr, 16));
			}
			catch (const std::exception&)
			{
				click.setResponse(LOC("INVARG"));
				std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
				prefill.push_back(L' ');
				prefill.append(arg);
				click.showCommandBoxIfPossible(std::move(prefill));
			}
		}

		void setCrc(Click& click, uint32_t crc)
		{
			g_hooking.spoofed_extra_content_crc = crc;
			value = Util::to_padded_hex_string(crc);
			update_state(click);
		}

		void getExtraInfo(CommandExtraInfo& info, std::wstring& args) final
		{
			CommandInput::getExtraInfo(info, args);
			info.char_limit = 8;
		}
	};
}
