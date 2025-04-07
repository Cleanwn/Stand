#pragma once

#include "CommandGamer.hpp"

#include "CommandLambdaAction.hpp"
#include "pointers.hpp"
#include "rlGamerInfo.hpp"
#include "SessionSpoofing.hpp"

namespace Stand
{
	class CommandSpoofedHost : public CommandGamer
	{
	public:
		inline static CommandSpoofedHost* instance;

		explicit CommandSpoofedHost(CommandList* parent)
			: CommandGamer(parent, LANG_GET("SPFDHST"), CMDNAMES("spoofedhost"), false)
		{
			this->createChild<CommandLambdaAction>(LOC("YOU"), {}, NOLABEL, [this](Click& click)
			{
				this->onPresetClick(click, pointers::rlPresence_GamerPresences_0_GamerInfo->getHandle().rockstar_id, pointers::rlPresence_GamerPresences_0_GamerInfo->name);
			});

			instance = this;
		}
	};
}
