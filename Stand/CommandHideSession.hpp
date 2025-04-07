#pragma once

#include "CommandListSelect.hpp"

#include <soup/rand.hpp>

#include "AbstractPlayer.hpp"
#include "SessionSpoofing.hpp"
#include "Tunables.hpp"

namespace Stand
{
	class CommandHideSession : public CommandListSelect
	{
	public:
		inline static CommandHideSession* instance = nullptr;

		explicit CommandHideSession(CommandList* parent)
			: CommandListSelect(parent, LOC("HDESESS"), CMDNAMES("spoofsession", "hidesession"), LOC("HDESESS_H"), {
				{0, LOC("DOFF"), CMDNAMES("off", "disabled")},
				{1, LOC("SP"), CMDNAMES("storymode"), NOLABEL, COMMANDPERM_USERONLY, CMDFLAG_FEATURELIST_REGULAR},
				{2, LOC("HDESESS_NX"), CMDNAMES("fake", "nonexistent"), NOLABEL, COMMANDPERM_USERONLY, CMDFLAG_FEATURELIST_ULTIMATE},
				{3, LOC("ONLSTS_O2"), CMDNAMES("offline"), NOLABEL, COMMANDPERM_USERONLY, CMDFLAG_FEATURELIST_ULTIMATE},
			}, 0)
		{
			instance = this;
		}

		~CommandHideSession() final
		{
			if (instance == this)
			{
				instance = nullptr;
			}
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (value == 0)
			{
				SessionSpoofing::onDisable();
			}
			else
			{
				if (g_tunables.getBool(TUNAHASH("DISABLE_SESSION_SPOOFING")))
				{
					click.setResponse(LOC("CMDDISA"));
					return onChangeImplUnavailable(click, prev_value);
				}
				if (value == 1)
				{
					if (!click.isRegularEdition())
					{
						return onChangeImplUnavailable(click, prev_value);
					}
				}
				else if (value == 2)
				{
					if (!click.isUltimateEdition())
					{
						return onChangeImplUnavailable(click, prev_value);
					}
					if (SessionSpoofing::random_session_info.id == 0)
					{
						SessionSpoofing::random_session_info.id = soup::rand(0, 0xFFFFFFFFFFFFFFFE);
						SessionSpoofing::random_session_info.token = SessionSpoofing::random_session_info.id + 1;
						SessionSpoofing::random_session_info.host = *AbstractPlayer::getHost().getPeer();
						SessionSpoofing::random_session_info.host.gamer_handle = soup::rand(1000000, 10000000);
						SessionSpoofing::random_session_info.host.censorOnlineIp();
					}
				}
				else if (value == 3)
				{
					if (!click.isUltimateEdition())
					{
						return onChangeImplUnavailable(click, prev_value);
					}
				}
			}
		}

		void onTickInGameViewport() final
		{
			if (SessionSpoofing::copy_from != nullptr)
			{
				setHelpText(LOC("HDESESS_H_CPY"));
			}
			else
			{
				setHelpText(LOC("HDESESS_H"));
			}
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
	};
}
