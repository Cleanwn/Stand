#pragma once

#include "CommandAction.hpp"

#include "CNetworkAssetVerifier.hpp"
#include "CommandExtraInfo.hpp"
#include "get_next_arg.hpp"
#include "pointers.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandSetAssetChecksum : public CommandAction
	{
	public:
		explicit CommandSetAssetChecksum(CommandList* const parent)
			: CommandAction(parent, LOC("ASTSET"), CMDNAMES_OBF("assetchecksum", "setassetchecksum"), LOC("ASTSET_H"))
		{
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
				const uint32_t crc = std::stoul(arg, nullptr, 16);
				click.setResponse(LIT(LANG_FMT("ASTSET_T", Util::to_padded_hex_string(crc))));
				if (parent->as<CommandListAssetHashes>()->fix->m_on)
				{
					auto derived = click.deriveAuto();
					parent->as<CommandListAssetHashes>()->fix->setStateBool(derived, false);
				}
				(*pointers::asset_verifier)->m_CRC.Set(crc);
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

		void getExtraInfo(CommandExtraInfo& info, std::wstring& args) final
		{
			CommandAction::getExtraInfo(info, args);
			info.char_limit = 8;
		}
	};
}
