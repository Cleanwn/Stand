#pragma once

#include "CommandPlayerToggle.hpp"

#include "FiberPool.hpp"

namespace Stand
{
	class CommandPlayerSmsSpam : public CommandPlayerToggle
	{
	private:
		CommandPlayerSmsSend* const sender;

	public:
		explicit CommandPlayerSmsSpam(CommandList* const parent, CommandPlayerSmsSend* const sender)
			: CommandPlayerToggle(parent, LOC("SPAM"), CMDNAMES_OBF("smsspam", "smspam")), sender(sender)
		{
		}

		void onEnable(Click& click) final
		{
			FiberPool::queueJob([this]
			{
				PP_PTR->toggleFiberBatch(this, [this](const std::vector<AbstractPlayer>& players)
				{
					sender->send(players, true);
					Script::current()->yield(200);
				});
			});
		}
	};
}
