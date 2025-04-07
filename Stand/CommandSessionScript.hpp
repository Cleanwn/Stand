#pragma once

#include "CommandListWithOnTickAsActiveList.hpp"
#include "CommandList.hpp"

#include "script_thread.hpp"

namespace Stand
{
	class CommandSessionScript : public CommandListWithOnTickAsActiveList<CommandList>
	{
	public:
		const hash_t script_hash;

		explicit CommandSessionScript(CommandList* const parent, const hash_t script_hash)
			: CommandListWithOnTickAsActiveList(parent, GtaThread::getLabel(script_hash), {}, NOLABEL, CMDFLAGS_LIST | CMDFLAG_FEATURELIST_SKIP), script_hash(script_hash)
		{
		}

		[[nodiscard]] GtaThread* getThread();
		[[nodiscard]] GtaThread* getThreadAndEnsureControl();

		void onTickAsActiveList() final;

		void onTickInGameViewport() final;
		void onTickInWebViewport() final;
	};
}
