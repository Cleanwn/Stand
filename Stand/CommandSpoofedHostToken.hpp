#pragma once

#include "CommandInput.hpp"

#include "CommandSpoofHostToken.hpp"
#include "get_next_arg.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandSpoofedHostToken : public CommandInput
	{
	private:
		CommandSpoofHostToken* const spoof_host_token;

	public:
		explicit CommandSpoofedHostToken(CommandList* const parent, CommandSpoofHostToken* spoof_host_token)
			: CommandInput(parent, LOC("SPFDHT"), CMDNAMES("spoofedhosttoken", "spoofedhostoken"), NOLABEL, "0000000000000000"), spoof_host_token(spoof_host_token)
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
				const auto host_token = std::stoull(arg, nullptr, 16);
				setHostToken(click, host_token);
				update_state(click);
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

		void setHostToken(Click& click, uint64_t host_token)
		{
			if (host_token == -1)
			{
				host_token = -2;
			}
			spoof_host_token->spoofed_host_token = host_token;
			value = Util::to_padded_hex_string(host_token);
			if (spoof_host_token->m_on)
			{
				auto click_ = click.deriveAuto();
				spoof_host_token->onChange(click_);
				if (!click.isAuto())
				{
					click.setSpoofUpdate();
				}
			}
		}

		void getExtraInfo(CommandExtraInfo& info, std::wstring& args) final
		{
			CommandInput::getExtraInfo(info, args);
			info.char_limit = 16;
		}
	};
}
