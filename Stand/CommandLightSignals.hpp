#pragma once

#include "CommandList.hpp"

#include "AbstractEntity.hpp"
#include "CommandTurnSignal.hpp"
#include "CommandUseBrakelights.hpp"
#include "CommandMuteEmergencySirens.hpp"

namespace Stand
{
	class CommandLightSignals : public CommandList
	{
	public:
		explicit CommandLightSignals(CommandList* const parent)
			: CommandList(parent, LOC("LGTSIGS"), CMDNAMES_OBF("lightsignals"))
		{
			auto turn = createChild<CommandTurnSignal>(LOC("TRNSIG"), CMDNAMES_OBF("signal", "blinker"));
			turn->automatic = createChild<CommandToggle>(LOC("TRNSIG_AUTO"), CMDNAMES_OBF("autosignal"));
			createChild<CommandUseBrakelights>();
			createChild<CommandMuteEmergencySirens>();
		}
	};
}