#pragma once

#include "CommandPlayerAction.hpp"

namespace Stand
{
	class CommandPlayerSmsSend : public CommandPlayerAction
	{
	private:
		const CommandInput* const text;

	public:
		AbstractPlayer issuer;

		explicit CommandPlayerSmsSend(CommandList* const parent, const CommandInput* const text)
			: CommandPlayerAction(parent, LOC("SMS"), CMDNAMES_OBF("smssend", "smsend")), text(text)
		{
		}

		void onClick(Click& click) final
		{
			setIssuer(click.getEffectiveIssuer());
			send(PP_PTR->getPlayers(), false);
		}

		void setIssuer(AbstractPlayer p)
		{
			if (PP_PTR->getPlayer() == p)
			{
				issuer = g_player;
			}
			else
			{
				issuer = p;
			}
		}

		void send(const std::vector<AbstractPlayer>& recipients, bool spam) const
		{
			if (text->value.empty()) // Empty text seems to break ability to receive further messages
			{
				return;
			}
			const auto issuer = this->issuer;
			FiberPool::queueJob([recipients, msg{ text->value }, issuer]
			{
				for (const auto& p : recipients)
				{
					p.sendSms(msg);
				}
			});
		}
	};
}
