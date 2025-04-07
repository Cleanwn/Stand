#include "CommandSessionScript.hpp"

#include "CNetGamePlayer.hpp"
#include "Gui.hpp"
#include "script_handler.hpp"

namespace Stand
{
	GtaThread* CommandSessionScript::getThread()
	{
		return GtaThread::fromHash(script_hash);
	}

	GtaThread* CommandSessionScript::getThreadAndEnsureControl()
	{
		auto thread = getThread();
		if (thread != nullptr)
		{
			thread->fiberSnatchScriptHost();
		}
		return thread;
	}

	void CommandSessionScript::onTickAsActiveList()
	{
		if (getThread() == nullptr)
		{
			g_gui.inputBack(TC_OTHER, false);
		}
	}

	void CommandSessionScript::onTickInGameViewport()
	{
		auto thread = getThread();
		if (!thread)
		{
			return;
		}
		if (auto netcomp = thread->getNetComponent())
		{
			auto help_text = LANG_GET("PRTCPTS");
			help_text.append(": ");
			help_text.append(fmt::to_string(netcomp->GetNumParticipants()));
			if (netcomp->host_data
				&& netcomp->host_data->net_player
				)
			{
				help_text.push_back('\n');
				help_text.append(LANG_GET("FLAG_HOST"));
				help_text.append(": ");
				help_text.append(AbstractPlayer(netcomp->host_data->net_player->player_id).getName());
			}
			setHelpText(LIT(std::move(help_text)));
		}
	}

	void CommandSessionScript::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}
}
