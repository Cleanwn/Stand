#pragma once

#include "CommandList.hpp"

#include "AptHelper.hpp"
#include "CommandPlayerAptMe.hpp"
#include "CommandPlayerAptWp.hpp"
#include "CommandPlayerApt.hpp"

namespace Stand
{
	class CommandPlayerAptList : public CommandList
	{
	public:
		explicit CommandPlayerAptList(CommandList* parent, commandflags_t flags)
			: CommandList(parent, LOC("PLYAPT"), {}, NOLABEL, flags)
		{
			this->createChild<CommandPlayerAptMe>();
			this->createChild<CommandPlayerAptWp>();

			for (uint8_t property_id = 1; property_id <= AptHelper::max_property_id; property_id++)
			{
				if (property_id != 86) // Private Yacht. Doesn't quite work. Disables minimap, playerlist, & weapon wheel for a few seconds, tho.
				{
					this->createChild<CommandPlayerApt>(property_id);
				}
			}
		}
	};
}
